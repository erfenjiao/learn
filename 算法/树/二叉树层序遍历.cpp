#include <iostream>
#include <queue>
using namespace std;

struct BinaryTree_Node
{
	char value;              // 节点的值
	
	BinaryTree_Node* lChild; // 左子节点
	BinaryTree_Node* rChild; // 右子节点
	
	//  节点的构造函数
	BinaryTree_Node() : lChild(NULL), rChild(NULL)
	{
	}
}; 

typedef BinaryTree_Node * BT_Node;

// 创建二叉树（先序遍历）
BT_Node CreateBinaryTree()
{
	BT_Node root = new BinaryTree_Node();
	char v;
	cin >> v;
	
	if (v != '#')
	{
		root->value = v;
		root->lChild = CreateBinaryTree();
		root->rChild = CreateBinaryTree();
	}
	
	else
		root = NULL;
		
	return root;
}

// 层次遍历（队列解法）
void Level_traverse(BT_Node root)
{
	queue<BT_Node> q;
	q.push(root);
	
	while (!q.empty())
	{
		BT_Node op = q.front();
		q.pop();
		cout << op->value;
		if (op->lChild != NULL)
			q.push(op->lChild);
			
		if (op->rChild != NULL)
			q.push(op->rChild);  
	}
}

int main()
{
	BT_Node root;
	root = CreateBinaryTree();
	Level_traverse(root);
} 
