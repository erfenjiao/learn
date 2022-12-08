#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<string.h>


typedef struct BinaryTreeNode
{
	char data;
	struct BinaryTreeNode* left;
	struct BinaryTreeNode* right;
}BiNode , *BiTree;

typedef struct sqQueue
{
	BiNode * data;
	struct sqQueue * next;
}sqQueue;

typedef struct Que
{
	sqQueue * front;
	sqQueue * tail;
}Que;



void Create(BiTree * T)
{
	char a;
	scanf("%c",&a);
	if(a != '#')
	{
		*T = (BiTree)malloc(sizeof(BiTree));
		(*T)->data = a;
		Create(&(*T)->left);
		Create(&(*T)->right);
	}
	else
		*T = NULL; 
}

void Pre(BiTree root)
{
	if(root ==NULL)
	{
		return ;
	}
	printf("%c",root->data);
	Pre(root->left);
	Pre(root->right);
}

void Mid(BiTree root)
{
	if(root == NULL)
		return ;
	Mid(root->left);
	printf("%c",root->data);
	Mid(root->right);
}

void Post(BiTree root)
{
	if(root == NULL)
		return ;
	Post(root->left);
	Post(root->right);
	printf("%c",root->data);
}

//层序遍历

void QueInit(Que *q)
{
	assert(q);
	q->front = q->tail = NULL;
}

void QueDestory(Que *q)
{
	assert(q);
	sqQueue * cur = q->front;
	while(cur)
	{
		sqQueue * next = cur->next;
		free(cur);
		cur = next;
	}
	q->front = q->tail = NULL;
}
int QueEmpty(Que *q)
{
	return q->front == NULL ? 0 : 1;
}
void QuePush(Que *q, BiNode * x)
{
	assert(q);
	sqQueue * node = (sqQueue *)malloc(sizeof(sqQueue));
	node->data = x;
	node->next = NULL;
	if(q->tail == NULL)
	{
		q->front = q->tail = node;
	}
	else
	{
		q->tail->next = q->front->next;
		q->tail = node; 
	}
	
}
void QuePop(Que *q)
{
	assert(q);
	sqQueue* front = q->front->next;
	free(q->front);
	q->front = front;

	if (front == NULL)
		q->tail = NULL;
}
BiNode * QueFront(Que *q)
{
	assert(q);
	BiNode * tmp = q->front->data;
	return tmp;
}


int main()
{
	
	BiTree b;
	Create(&b);
}




