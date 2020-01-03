#include <iostream>
#include <stdio.h>
#include <string.h>
#include "BTree.h"
using namespace std;

int main() {
    FILE *f = fopen("..//input.txt","r");
    int idx;
    char s[30];
    Data *d=NULL;
    BTree BT;

    while(fscanf(f, "%d %s\n", &idx, s)!=EOF){
        d = new Data(idx, s);
        BT.InsertData(d);
    }
    BT.FindIndex(98);
    BT.Display();
    BT.DeleteIndex(58);
    BT.Display();
    BT.DeleteIndex(47);
    BT.Display();
    BT.DeleteIndex(50);
    BT.Display();

    cout<<"\n-------Complete--------\n";
}
