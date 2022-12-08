#include<iostream>
#include<forward_list>
using namespace std;
/*
特殊的 forward_list 添加操作
添加后删除元素的操作是通过改变给定元素之后的元素完成的
编写函数，接受一个　forward_list<String>　和两个 String 共三个参数，函数应在链表中查找第一个 String ，并将第二个　
String 插入到紧接着第一个 String 之后的位置。若第一个　String 未在链表中，则将第二个插入到链表末尾
输出：
Hello 你好 ! world ! 
Hello 你好 ! ? world ! ? 
Hello 你好 ! ? world ! ? 再见 
*/  

void test_and_insert(forward_list<string> &sflst , const string &s1 , const string &s2)
{
    auto prev = sflst.before_begin();
    auto curr = sflst.begin();
    bool inserted = false;

    while(curr != sflst.end())
    {
        if(*curr == s1)
        {
            curr = sflst.insert_after(curr , s2);  //插入新字符串，curr指向它
            inserted = true;
        }
        prev = curr;
        curr++;
    }

    if(!inserted)
    {
        sflst.insert_after(prev , s2);
    }
}

int main()
{
    forward_list<string> sflst = {"Hello" , "!" , "world" , "!"};

    test_and_insert(sflst , "Hello" , "你好");
    for(auto curr = sflst.cbegin() ; curr != sflst.cend() ; curr++)
    {
        cout << *curr << " ";
    }
    cout << endl;

    test_and_insert(sflst , "!" , "?");
    for(auto curr = sflst.cbegin() ; curr != sflst.cend() ; curr++)
    {
        cout << *curr << " ";
    }
    cout << endl;

    test_and_insert(sflst , "Bye" , "再见");
    for(auto curr = sflst.cbegin() ; curr != sflst.cend() ; curr++)
    {
        cout << *curr << " ";
    }
    cout << endl;

}                                                