#include<stdio.h> 
#include<stdlib.h> 
#define MAXVEX 20
typedef char Vextype;
typedef struct
{
    int arcs[MAXVEX+1][MAXVEX+1];
    Vextype vex[MAXVEX+1];
    int vexnum;      /*顶点数*/
    int arcnum;     //弧度数
}AdjMatrix;

typedef struct ArcNode
{
    int adjvex;
    struct ArcNode *next;
}ArcNode;
typedef struct 
{
    Vextype vexdata;
    ArcNode *head;
}VertexNode;
typedef struct
{
    VertexNode vertex[MAXVEX];
    int vexnum;
    int arcnum;
}AdjList;

void Creat_AdjMatrix(AdjMatrix *G);  //创建邻接矩阵
int LocateVex_AdjMatrix(AdjMatrix *G,Vextype v);  

void Print_AdjList(AdjList *G);  //输出邻接表

void AdjMatrixToAdjList(AdjMatrix *M,AdjList *L);  //邻接矩阵转换为邻接表

main()
{
    AdjMatrix gM;
    AdjList gL;
    Creat_AdjMatrix(&gM);
    AdjMatrixToAdjList(&gM,&gL);
    Print_AdjList(&gL);
}

void Creat_AdjMatrix(AdjMatrix *G)//创建邻接矩阵
{
    int i,j,k;
    char v1,v2;
    scanf("%d\n",&(G->vexnum));
    for(i=1;i<=G->vexnum;i++)     /*顶点数*/
    { 
        scanf("%c",&(G->vex[i]));  //顶点
        for(j=1;j<=G->vexnum;j++)
            G->arcs[i][j]=0;       //矩阵
    }
    scanf("%d\n",&(G->arcnum));   
    for(k=1;k<=G->arcnum;k++)      //弧度数
    {
        scanf("<%c,%c>",&v1,&v2);
        i=LocateVex_AdjMatrix(G,v1);
        j=LocateVex_AdjMatrix(G,v2);
        G->arcs[i][j]=1; 
    }
}

int LocateVex_AdjMatrix(AdjMatrix *G,Vextype v)//输出邻接表
{
    int i;
    for(i=1;i<=G->vexnum;i++)
        if(G->vex[i]==v) 
            return i;     //顶点下标
    return 0;
}


void Print_AdjList(AdjList *G)
{
    int i;
    ArcNode *p;
    for(i=1;i<=G->vexnum;i++)
    { 
        printf("%d:(%c)",i,G->vertex[i].vexdata);
        for(p=G->vertex[i].head;p;p=p->next)
        {
            printf("->%d",p->adjvex);
        }
        printf("\n");
    }
}

/* 请在这里填写答案
typedef struct ArcNode
{
    int adjvex;
    struct ArcNode *next;
}ArcNode;
typedef struct 
{
    Vextype vexdata;
    ArcNode *head;
}VertexNode;
typedef struct
{
    VertexNode vertex[MAXVEX];
    int vexnum;
    int arcnum;
}AdjList;
 */
void AdjMatrixToAdjList(AdjMatrix *M,AdjList *L)
{
    L = (AdjList*)malloc(sizeof(AdjList));
    L->vexnum = M->vexnum;
    L->arcnum = M->arcnum;
    for(int i = 1 ; i <= L->vexnum ; i++)  //完成初始化
    {
        L->vertex[i].vexdata = M->vex[i];
        L->vertex[i].head = NULL;
    }
    for(int i = 1 ; i <= L->vexnum ; i++)  //完成初始化
    {
        for(int j = 1 ; j <= L->vexnum ; j++)
        {
            if(M->arcs[i][j] == 1)
            {
                ArcNode * arcnode = (ArcNode *)malloc(sizeof (ArcNode));
                arcnode->adjvex = j;
                arcnode->next = L->vertex[i].head;
                L->vertex[i].head = arcnode;
            }
        }
    }

}