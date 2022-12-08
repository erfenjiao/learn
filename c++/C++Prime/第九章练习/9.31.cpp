#include<iostream>
#include<forward_list>
#include<list>
using namespace std;
/*
9.3.5  改变容器大小 resize
[9.29] 假定 vec 　包含 25 个元素，那么　vec.resize(100)会做什么？
[9.30] 限制：提供默认构造函数
9.3.6  容器操作可能会使迭代器失效（vector\string\deque\list\forward_list）
编写改变容器的循环程序
程序必须保证每一步都更新迭代器、引用或者指针

[9.31] list forward_list 迭代器不支持加减操作
链表中元素并非顺序存储，应多次调用＋＋来实现迭代器加法操作
*/

//[9.31] 删除偶数元素并复制奇数元素

int main()
{
    list<int> lst = {0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10};
    auto curr = lst.begin();

    while(curr != lst.end())
    {
        if(*curr & 1)
        {
            curr = lst.insert(curr , *curr);  //插入到当前元素之前
            curr++;
            curr++;
        }
        else
        {
            curr = lst.erase(curr);  //删除，指向下一元素
        }
            for(curr = lst.begin() ; curr != lst.end() ; curr++)
        {
            cout << *curr << " ";
        }
        cout << endl;

        return 0;
    }
}