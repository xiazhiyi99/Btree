//
// Created by Xia on 2019/12/20.
//

#ifndef BTREE_BTREE_H
#define BTREE_BTREE_H
class Node;
class BTree;

class Data{
public:
    int idx;
    char data[30];
    Node *belongTo;
    Data(int idx, char *s);
    Data(Data *newData);
    ~Data();
};

class Node{
public:
    int n;
    BTree *belongTo;
    Node *nodeList[5], *pFather;
    Data *dataList[5];
    Node();
    ~Node();
    Data *FindIndex(int idx);

    bool IsRoot();
    bool IsLeaf();

    void InsertData(Data *data);
    void Insert(int pos, Data *data);

    void DeleteData(Data *data);
    void Delete(int pos, bool del=true);
    Data *FindBiggestData();
    Data *FindLeftBiggestData(Data *data);
    bool BorrowFromBrother();
    bool BorrowFromParent();
    void Check();
    void Display();
};

class BTree {
public:
    Node *root;
    BTree();
    ~BTree();
    void InsertData(Data *data);
    void FindIndex(int idx);
    void DeleteIndex(int idx);
    void Display();
};


#endif //BTREE_BTREE_H
