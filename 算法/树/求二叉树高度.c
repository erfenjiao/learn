#include <stdio.h>
#include <stdlib.h>

typedef char ElementType;
typedef struct TNode *Position;
typedef Position BinTree;
struct TNode{
    ElementType Data;
    BinTree Left;
    BinTree Right;
};

BinTree CreatBinTree(); /* 实现细节忽略 */
int GetHeight( BinTree BT );

int main()
{
    BinTree BT = CreatBinTree();
    printf("%d\n", GetHeight(BT));
    return 0;
}
/* 你的代码将被嵌在这里 */

int GetHeight( BinTree BT )
{
    int l=0,r=0,sum=0;
    if(BT)
    {
        l = GetHeight(BT->Left);
        r = GetHeight(BT->Right);
        sum = (l>r)?l:r;
        return sum+1;
    }
    else
    {
        return sum;
    }
}