/**
 * 1.建立哈夫曼树：读入文件(*.souce)，统计文件中字符出现的频度，并以这些字符的频度作为权值，建立哈夫曼树。
 * 2.编码：利用已建立好的哈夫曼树，获得各个字符的哈夫曼编码，并对正文进行编码，然后输出编码结果，并存入文件(*.code)中。
 * 3.译码：利用已建立好的哈夫曼树将文件(*.code)中的代码进行译码，并输出译码结果，并存入文件(*.decode)中。
 * 4.利用位操作，实现文件的压缩与解压。(选)
 * 
 */
#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include"huffman.h"
#define INTERNAL 1
#define LEAF 0


typedef struct node
{
	char x;
	int freq;
	char *code;
	int type;
	struct node *next;
	struct node *left;
	struct node *right;
}node;

node *HEAD,*ROOT;

void printll();
void makeTree();
void genCode(node *p,char* code);
void insert(node *p,node *m);
void addSymbol(char c);
void writeHeader(FILE *f);
void writeBit(int b,FILE *f);
void writeCode(char ch,FILE *f);
//void writeCode2(char ch,FILE *f , FILE* f2);
char *getCode(char ch);
void codefile(FILE *f , FILE *f2);

node* newNode(char c)
{
	node *q;
	q=(node *)malloc(sizeof(node));
	q->x=c;
	q->type=LEAF;	//leafnode
	q->freq=1;
	q->next=NULL;
	q->left=NULL;
	q->right=NULL;
	return q;
}

char codex[100];
char codecode[100];

int main(int argc , char ** argv)
{
	FILE *fp , *fp2;
	char ch;
	int t;
	HEAD = NULL;
	ROOT = NULL;

	if(argc <= 2)
	{
		printf("----------\n");
		printf("Usage:\n %s <input-file-to-zip> <zipped-output-file>\n***Huffman File Encoder***\nAuthor: erfenjiao\n",argv[0]);
		if(argc==2)
		{
			argv[2]=(char *)malloc(sizeof(char)*(strlen(argv[1])+strlen(ext)+1));
			strcpy(argv[2],argv[1]);
			strcat(argv[2],ext);
			argc++;
		}
		else
			return 0;
	}
	fp=fopen(argv[1],"rb");
	if(fp==NULL)
	{
		printf("[!]Input file cannot be opened.\n");
		return -1;
	}
	printf("----------\n");
	printf("Reading file : %s\n",argv[1]);
	while(fread(&ch , sizeof(char) , 1 , fp) != 0)
	{
		addSymbol(ch);
	}

	printf("----------\n");
	printf("construct huaffmanTree\n");
	makeTree();

	//printf("----------\n");
	

	printf("----------\n");
	printf("\nAssigning Codewords.\n");
	genCode(ROOT,"\0");			//preorder traversal

	printf("----------\n");
	printf("*.souce --->  *.code :\n");
	fp = fopen(argv[1],"r");
	if(fp == NULL)
	{
		printf("\n[!]Input file cannot be opened.\n");
		return -1;
	}
	fp2 = fopen(argv[2],"wb");
	if(fp2 == NULL)
	{
		printf("\n[!]Output file cannot be opened.\n");
		return -2;
	}
	//codefile(fp , fp2); 已废弃
	writeHeader(fp2);
	printf("----------\n");
	//printf("WriteCode() :\n");
	while(fread(&ch , sizeof(char) , 1 , fp)!=0)
	{	
		//printf("\n%c replaced with ",ch);
		writeCode(ch , fp2);	//write corresponding code into fp2
	}
	
}//main 

//统计，一条链表
void addSymbol(char c)
{
	node *p , *q , *m;
	if(HEAD == NULL)      //link is null , newNode.
	{
		HEAD = newNode(c);
		return ;
	}
	p = HEAD;
	q = NULL;
	if(p->x == c)           //find
	{
		p->freq++;          
		if(p->next == NULL)  //p node is end code in this link
		{
			return ;
		}
		if(p->freq > p->next->freq)  
		{
			HEAD = p->next;
			p->next = NULL;
			insert(p , HEAD);
		}
		return ;
	}

	while(p->next != NULL && p->x != c)  //依次往后查找
	{
		q = p;
		p = p->next;
	}

	if(p->x == c)
	{
		p->freq++;
        if(p->next == NULL)
			return;	
		if(p->freq > p->next->freq)
		{
			m = p->next;
			q->next = p->next;
			p->next = NULL;
			insert(p , HEAD);
		}
	}
	else          //not find
	{
		q = newNode(c);
		q->next = HEAD;
		HEAD = q;
	}
	node *t = HEAD;
	//printf("\t\t----------\n");
	while(t != NULL)
	{
		//printf("\t\tt->x = %c\tt->freq = %d\n",t->x , t->freq);
		t = t->next;
	}
}

//按频率在列表中插入p，从m开始向右
//我们不能放置小于m的节点，因为我们没有ptr到m左边的节点
void insert(node *p,node *m)
{
	if(m->next==NULL)
	{  
		m->next=p; 
		return;
	}
	while(m->next->freq < p->freq)
	{  
		m = m->next;
	    if(m->next == NULL)
	    { 
			m->next = p;   //append
			return; 
		}
	}
  	p->next=m->next;  //insert
  	m->next=p;
}

void makeTree()
{
	// printf("\t----------\n");
	// printf("\tmakeTree() begin...\n");
	node  *p,*q;
	p = HEAD;
	int i = 1;
	int m = 0;
	while(p != NULL)
	{
		q = newNode('@');
		q->type = INTERNAL;	 //internal node
		q->left = p;		
		q->freq = p->freq;
		m = p->freq;
		// printf("\t\t第　%d 次:\n",i++);
		// printf("\t\tq->left->freq = %d\tq->left->x = %c\n",q->freq , p->x); 
		//printf("q->left :%d:\n",*p);
		if(p->next != NULL)  
		{
			p = p->next;
			q->right = p;	
			q->freq += p->freq;
			//printf("\t\tq->right->freq = %d\tq->right->x = %c\n",q->freq-m , p->x);
		}
		//printf("\t\tq->freq = %d\tq->x = %c\n",q->freq , q->x);
		p = p->next;	
		if(p == NULL)	
			break;
		if(q->freq <= p->freq)
		{
			q->next = p;
			p = q;
		}
		else
			insert(q , p);	//find appropriate position
	}//while
	ROOT=q; //q created at last iteration is ROOT of h-tree
}

//genCode(ROOT,"\0");	preorder traversal
void genCode(node *p,char* code)
{
	char *lcode , *rcode;
	static node *s;
	static int flag;
	if(p != NULL)
	{
		//sort linked list as it was
		if(p->type == LEAF)   //leaf node
		{	if(flag == 0) //first leaf node
			{
				flag = 1 ; 
				HEAD = p;
			}
			else	//other leaf nodes
			{ 
				s->next = p;
			}		//sorting LL
			p->next = NULL;
			s = p;
		}

		//assign code
		p->code = code;	//assign code to current node
		if(p->x != '@')
			printf("\t\t[p->x = %c|p->freq = %d|p->code = %s|p->type = %d]\n",p->x,p->freq,p->code,p->type);
		lcode=(char *)malloc(strlen(code)+2);
		rcode=(char *)malloc(strlen(code)+2);
		sprintf(lcode , "%s0" , code);
		sprintf(rcode , "%s1" , code);
		//recursive DFS
		genCode(p->left  , lcode);		//left child has 0 appended to current node's code
		genCode(p->right , rcode);
	}
}

void writeCode(char ch , FILE *f) //fp2
{
	char *code;
	code = getCode(ch);
	//printf("\t\tWriteCode() :\n");
	//printf("\t\tcode = %s\n",code);
	while(*code != '\0')
	{
		if(*code == '1')
			writeBit(1 , f); 	//write bit 1 into file f
		else
			writeBit(0 , f);
		//fwrite(code , sizeof(char) , 1 , f);
		code++;
	}
	return;
}

char *getCode(char ch)
{
	node *p = HEAD;
	while(p != NULL)
	{
	    if(p->x == ch)
		  return p->code;
	    p = p->next;
	}
	return NULL; //not found
}

void writeHeader(FILE *f)
{//Table mapping 'codewords' to actual symbols
	// printf("\t\t----------\n");
	// printf("\t\twriteHeader():\n");
	symCode record;
	node *p;
	int temp = 0 , i = 0;
	p = HEAD;
	while(p != NULL)	
	{
		temp += (strlen(p->code)) * (p->freq);		//temp stores padding
		if(strlen(p->code) > MAX) 
			printf("\n[!] Codewords are longer than usual.");	//TODO: Solve this case
		temp %= 8;
		i++;
		p = p->next;
	}

	if(i == 256)
		N = 0;	//if 256 diff bit combinations exist, then alias 256 as 0
	else 
		N = i;

	fwrite(&N , sizeof(unsigned char) , 1 , f);	//read these many structures while reading
	printf("\t\tN = %u\n" , i);

	p = HEAD;
	while(p != NULL)	//start from HEAD, write each char & its code
	{
		record.x = p->x;
		strcpy(record.code , p->code);
		fwrite(&record , sizeof(symCode) , 1 , f);
		//printf("\t\t[%c|%s]\n",record.x,record.code);
		p = p->next;
	}
	//discard 'padding' bits before data, while reading
	padding = 8 - (char)temp;	//int to char & padding = 8-bitsExtra
	fwrite(&padding , sizeof(char) , 1 , f);
	printf("\t\tPadding = %d\n" , padding);

	for(i = 0 ; i < padding ; i++)   //补齐
		writeBit(0 , f);

}//fun



void writeBit(int b , FILE *f)
{
	static char byte;
	static int cnt;
	char temp;
	printf("\t\t------------------\t");
	printf("\t\twriteBit():\t");
	printf("\t\tSetting %dth bit = %d of %d \n" , cnt , b , byte);
	if(b == 1)
	{	
		temp = 1;
		temp = temp << (7 - cnt);	
		printf("\t\t----------if(b == 1)----------\n");
		printf("\t\ttemp = %d , byte_1 = %d\n" , temp , byte);	
		byte = byte | temp;
		printf("\t\tbyte_2 = %d\n" , byte);
		//printf("\t\t----------------------\n");
	}
	cnt++;
	//printf("\t\tbyte_3 = %d\n" , byte);
	//printf("\t\tbyte = %\n" , byte);
	if(cnt == 8)		//buffer full
	{
		//printf("\t\tbyte = %d\n",byte);
		//printf("\t\tbitsChar = %s\n" ,bitsInChar(byte));
		fwrite(&byte , sizeof(char) , 1 , f);
		cnt = 0; 
		byte = 0;				//reset buffer
		return;				   
	}
	return;
}

// void codefile(FILE *f , FILE* f2)
// {
// 	int i = 0 , j = 0 , count = 0;
// 	printf("\t----------\n");
// 	printf("\tcodefile():\n");
// 	char ch , ch2;
// 	while(fread(&ch , sizeof(char) , 1 , f) != 0)
// 	{
// 		symCode code;
// 		node *p = HEAD;
// 		while(p!=NULL)	//start from HEAD, write each char & its code
// 		{
// 			//code.x = p->x;
// 			code->x = p->x;
// 			strcpy(code->code , p->code);
// 			//fwrite(&record,sizeof(symCode),1,f);
// 			//printf("\t\t%c|%s\n",code.x , code.code);
// 			codex[i++] = code->x;
// 			codecode[j++] = *code->code;
// 			//fwrite(&codecode , sizeof(symCode) , 1 , f2);
// 			//fwrite(&codex , sizeof(symCode) , 1 , f2);
// 			p = p->next;
// 			count++;
// 		}
// 		for(int k = 0 ; k < count ; k++)
// 		{
// 			if(ch == codex[k])
// 			{
// 				printf("\t\tch = %c , codecode[%d] = %c\n",ch , k , codecode[k]);
// 				ch2 = codecode[k];
// 				fwrite(&ch2 , sizeof(char) , 1 , f2);
// 			}
// 		}
// 	}
// }