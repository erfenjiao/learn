#include<stdio.h>
#include<stdbool.h>
typedef struct
{
	int row; //行数 
	int col; //列数 
	int val; //元素值 
}num; 
typedef struct
{
	int m;//总行数 
	int n; //总列数 
	int len; //非零元个数 
	num data[100];//存储每个非零数 
}Matrix; 
int i;
    Matrix a,b,c;

static bool matrix_plus(Matrix a, Matrix b, Matrix *c)//矩阵加法 
{
	int i = 0; 
	int j = 0; 
	int k = 0; 
	int v;
	if (a.m!=b.m||a.n!=b.n) //位置不对别加 
		return false;
	c->m = a.m;
	c->n = a.n;
	while (i<a.len&&j<b.len) 
	{
		if (a.data[i].row==b.data[j].row)//行数同时 
		{
			if (a.data[i].col<b.data[j].col)//若列数不同1 
			{
				c->data[k].row=a.data[i].row ;
				c->data[k].col=a.data[i].col;
				c->data[k].val=a.data[i].val;
				k++;
				i++;
			}
			else if (a.data[i].col>b.data[j].col) //列数不同2 
			{
				c->data[k].row=b.data[j].row;
				c->data[k].col=b.data[j].col;
				c->data[k].val=b.data[j].val;
				k++;
				j++;
			}
			else //行数列数相同，加就完事儿了 
			{
				v = a.data[i].val+b.data[j].val;
				if (v != 0) //这里要考虑如果加完出现零元的情况 
				{
					c->data[k].row=a.data[i].row;
					c->data[k].col=a.data[i].col;
					c->data[k].val=v;
					k++;
				}
				i++;
				j++;
			}
		}
		else if (a.data[i].row<b.data[j].row) //行数不同判断且对应位置一个矩阵没元素 
		{
			c->data[k].row=a.data[i].row;
			c->data[k].col=a.data[i].col;
			c->data[k].val=a.data[i].val;
			k++;
			i++;
		}
		else // 
		{
			c->data[k].row=b.data[j].row;
			c->data[k].col=b.data[j].col;
			c->data[k].val=b.data[j].val;
			k++;
			j++;
		}
	}
	while(i==a.len&&j<b.len){//以后元素全和b矩阵一样时 
		c->data[k].row=b.data[j].row;
		c->data[k].col=b.data[j].col;
		c->data[k].val=b.data[j].val;
		k++;
		j++;
	}
	while(i<a.len&&j==b.len){//以后元素全和a矩阵一样时 
		c->data[k].row=a.data[j].row;
		c->data[k].col=a.data[j].col;
		c->data[k].val=a.data[j].val;
		k++;
		i++;
	}
	c->len = k;
	return true;
}


static bool matrix_decrease(Matrix a, Matrix b, Matrix *c) //矩阵减法 
{
	int i = 0; 
	int j = 0; 
	int k = 0; 
	int v;
	if (a.m!=b.m||a.n!=b.n) 
		return false;
	c->m = a.m;
	c->n = a.n;
	while (i<a.len&&j<b.len) 
	{
		if (a.data[i].row==b.data[j].row)
		{
			if (a.data[i].col<b.data[j].col)
			{
				c->data[k].row=a.data[i].row ;
				c->data[k].col=a.data[i].col;
				c->data[k].val=a.data[i].val;
				k++;
				i++;
			}
			else if (a.data[i].col>b.data[j].col) 
			{
				c->data[k].row=b.data[j].row;
				c->data[k].col=b.data[j].col;
				c->data[k].val=0-b.data[j].val;
				k++;
				j++;
			}
			else 
			{
				v = a.data[i].val-b.data[j].val;
				if (v != 0) 
				{
					c->data[k].row=a.data[i].row;
					c->data[k].col=a.data[i].col;
					c->data[k].val=v;
					k++;
				}
				i++;
				j++;
			}
		}
		else if (a.data[i].row<b.data[j].row) 
		{
			c->data[k].row=a.data[i].row;
			c->data[k].col=a.data[i].col;
			c->data[k].val=a.data[i].val;
			k++;
			i++;
		}
		else 
		{
			c->data[k].row=b.data[j].row;
			c->data[k].col=b.data[j].col;
			c->data[k].val=0-b.data[j].val;
			k++;
			j++;
		}
	}
	while(i==a.len&&j<b.len){
		c->data[k].row=b.data[j].row;
		c->data[k].col=b.data[j].col;
		c->data[k].val=0-b.data[j].val;
		k++;
		j++;
	}
	while(i<a.len&&j==b.len){
		c->data[k].row=a.data[j].row;
		c->data[k].col=a.data[j].col;
		c->data[k].val=a.data[j].val;
		k++;
		i++;
	}
	c->len=k;
	return true;
}


int main()
{
    
	scanf("%d %d %d",&a.m,&a.n,&a.len);
	for(i=0;i<a.len;i++)
		scanf("%d,%d,%d",&a.data[i].row,&a.data[i].col,&a.data[i].val);
	scanf("%d %d %d",&b.m,&b.n,&b.len);
	for(i=0;i<b.len;i++)
		scanf("%d,%d,%d",&b.data[i].row,&b.data[i].col,&b.data[i].val);
	matrix_plus(a, b, &c);
	printf("%d %d %d\n",c.m,c.n,c.len);
	for(i=0;i<c.len;i++)
		printf("%d,%d,%d ",c.data[i].row,c.data[i].col,c.data[i].val);
	matrix_decrease(a, b, &c);
	printf("\n%d %d %d\n",c.m,c.n,c.len);
	for(i=0;i<c.len;i++)
		printf("%d,%d,%d ",c.data[i].row,c.data[i].col,c.data[i].val);
}

