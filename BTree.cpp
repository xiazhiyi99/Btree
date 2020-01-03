//
// Created by Xia on 2019/12/20.
//

#include "BTree.h"
#include <string.h>
#include "iostream"
#include <assert.h>
using namespace std;
Data::Data(int id, char *s){
    idx = id;
    strcpy(data, s);
    belongTo = NULL;
}
Data::Data(Data *oldData) {
    idx = oldData->idx;
    belongTo = oldData->belongTo;
    strcpy(data, oldData->data);
}
Data::~Data()= default;

Node::Node(){
    cout<<"New Node\n";
    n=0;
    pFather = NULL;
    for(int i=0;i<5;i++){
        dataList[i] = NULL;
        nodeList[i] = NULL;
    }
}
Node::~Node(){
    cout<<"Delete Node\n";
    for(int i=0;i<5;i++){
        if (dataList[i]) delete dataList[i];
        if (nodeList[i]) delete nodeList[i];
    }
}
Data* Node::FindIndex(int idx) {
    if (n==0) return NULL;
    else if (dataList[0]->idx > idx) return nodeList[0]?nodeList[0]->FindIndex(idx):NULL;
    else if (dataList[n-1]->idx < idx) return nodeList[n]?nodeList[n]->FindIndex(idx):NULL;
    else if (dataList[0]->idx == idx) return dataList[0];
    else for (int i=0;i<n-1;i++){
        if (dataList[i]->idx < idx && idx < dataList[i+1]->idx) return nodeList[i+1]?nodeList[i+1]->FindIndex(idx):NULL;
        else if (dataList[i+1]->idx == idx) return dataList[i+1];
    }
}
Data* Node::FindBiggestData() {
    if (IsLeaf()) return dataList[n-1];
    else return nodeList[n]->FindBiggestData();
}
Data* Node::FindLeftBiggestData(Data *data) {
    if (IsLeaf()){
        cout<<"FindLeftBiggestData:Is Leaf!";
        return NULL;
    }

    int leftIndex = -1;
    for(int i=0; i<n; i++)
        if (dataList[i]==data){
            leftIndex = i;
            break;
        }

    return nodeList[leftIndex]->FindBiggestData();
}
bool Node::IsRoot() {
    return belongTo->root == this;
}
bool Node::IsLeaf() {
    //cout<< (nodeList[0]!=NULL);
    bool flag = !((nodeList[0]!=NULL) && (nodeList[0]->n!=0));
    return flag;
}
void Node::InsertData(class Data * data) {
    cout<<"Inserting idx:"<<data->idx<<" at Node "<<this<<"\n";
    // very first data
    if (IsRoot() && n==0){
        Insert(0, data);
        return;
    }
    // not leaf
    if (!IsLeaf()){
        if (data->idx < dataList[0]->idx)
            nodeList[0]->InsertData(data);
        else if (dataList[n-1]->idx <= data->idx)
            nodeList[n]->InsertData(data);
        else for(int i=0;i<n-1;i++)
            if (dataList[i]->idx <= data->idx && data->idx < dataList[i+1]->idx){
                nodeList[i+1]->InsertData(data);
                return;
            }
        return;
    }

    // insert data
    if (data->idx < dataList[0]->idx)
        Insert(0, data);
    else if (dataList[n-1]->idx <= data->idx)
        Insert(n, data);
    else for(int i=0;i<n-1;i++)
            if (dataList[i]->idx <= data->idx && data->idx < dataList[i+1]->idx){
                Insert(i+1, data);
                break;
            }
    Check();
}
void Node::Insert(int pos, class Data * data) {
    // insert in datalist
    assert(n<3);
    for(int i=n;i>pos;i--)
        dataList[i] = dataList[i-1];
    dataList[pos] = data;
    data->belongTo = this;
    n++;
}
void Node::DeleteData(class Data * data) {
    cout<<"Deleting "<<data->idx<<'\n';
    int pos = -1;
    for(int i=0; i<n; i++)
        if (dataList[i]==data){
            pos = i;
            break;
        }

    if (IsLeaf()){
        Delete(pos);
        Check();
    }else{
        Data *switchD = FindLeftBiggestData(data);
        Data *newData = new Data(switchD);
        delete dataList[pos];
        dataList[pos] = newData;
        newData->belongTo = this;

        switchD->belongTo->DeleteData(switchD);
    }
}
void Node::Delete(int pos, bool del) {
    assert(n>0);
    if (del) delete dataList[pos];
    for(int i=pos;i<n-1;i++)
        dataList[i] = dataList[i+1];
    dataList[n-1] = NULL;
    n--;
}
bool Node::BorrowFromBrother() {
    int nodePos = -1;
    for(int i=0;i<=pFather->n;i++)
        if (pFather->nodeList[i] == this) {
            nodePos = i;
            break;
        }
    if (nodePos == -1){
        cout<<"BorrowFromBrother: Fail To Find this";
        assert(0);
    }

    if (nodePos != 0)
        // check left bro
        if (pFather->nodeList[nodePos-1]->n == 2){
            //Yes!
            //     ...  |  kp  |  ...
            //     lBro/        \this
            //    ...|lp|        |
            //           \lstr    \..
            //
            //             ||
            //             \/
            //
            //     ...  |  lp  |  ...
            //     lBro/        \this
            //    ...|empty      |kp|
            //         lsubtree /    \..
            Node *lBro = pFather->nodeList[nodePos-1],
                 *lSubTree = lBro->nodeList[lBro->n];
            Data *kp = pFather->dataList[nodePos-1],
                 *lp = lBro->dataList[lBro->n-1];

            Insert(0, kp);
            kp->belongTo = this;
            pFather->dataList[nodePos - 1] = NULL;

            pFather->dataList[nodePos - 1] = lp;
            lp->belongTo = pFather;
            lBro->Delete(lBro->n-1, false);

            for(int i=4;i>0;i--)
                nodeList[i] = nodeList[i - 1];
            nodeList[0] = lSubTree;
            lSubTree->pFather = this;
            lBro->nodeList[lBro->n+1] = NULL;

            return true;
        }

    if (nodePos != pFather->n)
        // check right bro
        if (pFather->nodeList[nodePos + 1]->n == 2){
            //Yes!
            //     ...  |  kp  |  ...
            //     this/        \rBro
            //        |          |rp|...
            //   ... /      rstr/    \...
            //
            //             ||
            //             \/
            //
            //     ...  |  rp  |  ...
            //     this/        \rBro
            //       |kp|        empty| ...
            //  ... /    \rstr       / ...
            Node *rBro = pFather->nodeList[nodePos+1],
                 *rSubTree = rBro->nodeList[0];
            Data *kp = pFather->dataList[nodePos],
                 *rp = rBro->dataList[0];

            Insert(0, kp);
            kp->belongTo = this;
            pFather->dataList[nodePos] = NULL;

            pFather->dataList[nodePos] = rp;
            rp->belongTo = pFather;
            rBro->Delete(0, false);

            nodeList[n] = rSubTree;
            rSubTree->pFather = this;
            rBro->nodeList[0] = NULL;
            for(int i=0;i<4;i++)
                rBro->nodeList[i] = rBro->nodeList[i + 1];
            return true;
        }

    return false;
}
bool Node::BorrowFromParent() {
    int nodePos = -1;
    for(int i=0;i<=pFather->n;i++)
        if (pFather->nodeList[i] == this) {
            nodePos = i;
            break;
        }
    if (nodePos == -1){
        cout<<"BorrowFromParent: Fail To Find this";
        assert(0);
    }

    if(pFather->IsRoot() && (pFather->n == 1)){
        // reduce layer
        if (nodePos==0){
            //this is left child
            pFather->nodeList[0] = nodeList[0];
            if (nodeList[0]) nodeList[0]->pFather = pFather;
            nodeList[0] = NULL;

            Node *rBro = pFather->nodeList[1];
            pFather->Insert(1, rBro->dataList[0]);
            rBro->Delete(0, false);

            pFather->nodeList[1] = rBro->nodeList[0];
            if (rBro->nodeList[0]) rBro->nodeList[0]->pFather = pFather;
            rBro->nodeList[0] = NULL;
            pFather->nodeList[2] = rBro->nodeList[1];
            if (rBro->nodeList[1]) rBro->nodeList[1]->pFather = pFather;
            rBro->nodeList[1] = NULL;

            delete rBro;
            delete this;
            return true;
        }else if(nodePos==1){
            // this is right child
            pFather->nodeList[2] = nodeList[0];
            if (nodeList[0]) nodeList[0]->pFather = pFather;
            nodeList[0] = NULL;

            Node *lBro = pFather->nodeList[0];
            pFather->Insert(0, lBro->dataList[0]);
            pFather->nodeList[0] = lBro->nodeList[0];
            if (lBro->nodeList[0]) lBro->nodeList[0]->pFather = pFather;
            lBro->nodeList[0] = NULL;
            pFather->nodeList[1] = lBro->nodeList[1];
            if (lBro->nodeList[1]) lBro->nodeList[1]->pFather = pFather;
            lBro->nodeList[1] = NULL;

            delete lBro;
            delete this;
        }else assert(0);
        return true;
    }
    else{
        //dont reduce layer
        int broPos = -1, kPos = -1;
        if (nodePos==0) {
            // merge rBro
            broPos = 1;
            kPos = 0;
        }
        else {
            // merge lBro
            broPos = nodePos - 1;
            kPos = nodePos - 1;
        }
        Node *Bro = pFather->nodeList[broPos];
        Data *kp = pFather->dataList[kPos], *bp = Bro->dataList[0];
        //merge

        Bro->Delete(0, false);
        pFather->Delete(kPos, false);
        for(int i=kPos;i<4;i++)
            pFather->nodeList[i] = pFather->nodeList[i+1];
        pFather->nodeList[kPos] = this;

        if (bp->idx < kp->idx){
            // merge lBro
            Insert(0, bp);
            Insert(1, kp);

            nodeList[2] = nodeList[0];

            nodeList[0] = Bro->nodeList[0];
            if (Bro->nodeList[0]) Bro->nodeList[0]->pFather = this;
            Bro->nodeList[0] = NULL;

            nodeList[1] = Bro->nodeList[1];
            if (Bro->nodeList[1]) Bro->nodeList[1]->pFather = this;
            Bro->nodeList[1] = NULL;
        }else{
            // merge rBro
            Insert(0, kp);
            Insert(1, bp);

            nodeList[1] = Bro->nodeList[0];
            if (Bro->nodeList[0]) Bro->nodeList[0]->pFather = this;
            Bro->nodeList[0] = NULL;

            nodeList[2] = Bro->nodeList[1];
            if (Bro->nodeList[1]) Bro->nodeList[1]->pFather = this;
            Bro->nodeList[1] = NULL;
        }

        delete Bro;
        pFather->Check();
        return true;
    }
}
void Node::Check(){
    /* debug
    cout<<"!!!! check this node: ";
    for(int i=0;i<n;i++)
        cout<<dataList[i]->idx<<' ';
    cout<<'\n';
    cout<<"!!!! check this node's father: ";
    if (IsRoot()) cout<<"root";
    else for(int i=0;i<pFather->n;i++)
        cout<<pFather->dataList[i]->idx<<' ';
    cout<<'\n';
    */
    if ((n>3)||(n<0)){
        cout<<"SECOND FUCK";
        assert(false);
    }

    // split
    if (n == 3) {
        if (IsRoot()){
            // this is root, new lBro and rBro
            Node *lBro= new Node,*rBro = new Node;
            lBro->nodeList[0] = nodeList[0];
            if (nodeList[0]) nodeList[0]->pFather = lBro;
            lBro->nodeList[1] = nodeList[1];
            if (nodeList[1]) nodeList[1]->pFather = lBro;
            lBro->dataList[0] = dataList[0];
            dataList[0]->belongTo = lBro;
            lBro->n = 1;
            lBro->pFather = this;
            lBro->belongTo = belongTo;

            rBro->nodeList[0] = nodeList[2];
            if (nodeList[2]) nodeList[2]->pFather = rBro;
            rBro->nodeList[1] = nodeList[3];
            if (nodeList[3]) nodeList[3]->pFather = rBro;
            rBro->dataList[0] = dataList[2];
            dataList[2]->belongTo = rBro;
            rBro->n = 1;
            rBro->pFather = this;
            rBro->belongTo = belongTo;

            dataList[0] = dataList[1];
            dataList[1] = dataList[2] = NULL;
            nodeList[0] = lBro;
            nodeList[1] = rBro;
            nodeList[2] = nodeList[3] = nodeList[4] = NULL;
            n = 1;
            printf("lbro %d rbro %d root %d\n", lBro->dataList[0]->idx, rBro->dataList[0]->idx, dataList[0]->idx);
        }else{
            // None-root split

            // find pointer's index to this node
            int pos = -1;
            for(int i=0;i <= pFather->n;i++)
                if (pFather->nodeList[i] == this)
                    pos = i;
            // shift key in father
            // Insert: n++
            pFather->Insert(pos, dataList[1]);
            // shift node pointer in father
            for(int i=pFather->n;i>pos+1;i--){
                pFather->nodeList[i] = pFather->nodeList[i-1];
            }
            // this address(pFather->nodeList[pos]) save lBro
            // new rBro(pFather->nodeList[pos+1])
            Node *rBro = new Node;
            pFather->nodeList[pos+1] = rBro;

            rBro->nodeList[0] = nodeList[2];
            if (nodeList[2]) nodeList[2]->pFather = rBro;
            rBro->nodeList[1] = nodeList[3];
            if (nodeList[3]) nodeList[3]->pFather = rBro;
            rBro->dataList[0] = dataList[2];
            dataList[2]->belongTo = rBro;
            rBro->n = 1;
            rBro->pFather = pFather;
            rBro->belongTo = belongTo;

            nodeList[2] = nodeList[3] = nodeList[4] = NULL;
            dataList[1] = dataList[2] = NULL;
            n = 1;

            // check upwards
            printf("lbro %d rbro %d parent %d\n", dataList[0]->idx, rBro->dataList[0]->idx, pFather->dataList[0]->idx);
            pFather->Check();
        }
    }

    // borrow
    if ((n == 0) && (!IsRoot())){
        // cout<<" n == 0!\n";
        if (BorrowFromBrother());
        else if (BorrowFromParent());
        else {
            cout<<"Check Fail!";
        }
    }
}
void Node::Display() {
    cout<<'{';
    for (int i=0;i<n;i++) cout<<dataList[i]->idx<<',';
    if (!IsLeaf()){
        for(int i=0;i<=n;i++){
            nodeList[i]->Display();
            if (i!=n)cout<<',';
        }
    }
    cout<<'}';
}

BTree::BTree(){
    root = NULL;
};
BTree::~BTree() {
    delete root;
}
void BTree::InsertData(class Data * data) {
    if (root == NULL) {
        root = new Node;
        root->belongTo = this;
    }
    root->InsertData(data);
}
void BTree::FindIndex(int idx) {
    if (root){
        Data *d = root->FindIndex(idx);
        if (d) {
            cout << d->data <<'\n';
            return;
        }else{
            cout<<"no found!\n";
        }
    }else{
        cout<<"empty tree!\n";
    }
}
void BTree::DeleteIndex(int idx) {
    if (root==NULL){
        cout<<"Empty Tree!";
        return;
    }
    // 1.search
    Data *D = root->FindIndex(idx);
    if (D==NULL){
        cout<<"Index Not Found!";
        return;
    }
    // 2.delete 3.check
    Node *N = D->belongTo;
    N->DeleteData(D);
}
void BTree::Display() {
    if (!root) cout<<"no root!";
    else root->Display();
    cout<<'\n';
}
