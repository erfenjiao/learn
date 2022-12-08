/*
9.4 vector 对象是如何增长的
管理容量的成员函数
    c.capacity()
    c.reserve(n)   //分配空间
cppacity and size
    size : 已经保存的元素数目
    capacity : 在不分配多余的内存空间下它可以最多保持很多信息

９.5.1 构造　String　的其他方法
    原方法
        string s1;
        string s2(s1);
        string s2 = s1;
        string s3("value");
        string s3 = "value";
        string s4(n , 'c');

    现：
        string s(cp , n);
        string s(s2 , pos2);
        string s(s2 , pos2 , len2);
        s.substr(pos , n);

*/
//[9.41] 从　vector<char>　中初始化　string
#include<iostream>
#include<forward_list>
#include<vector>
#include<string>
using namespace std;
int main()
{
    vector<char> vc = {'H' , 'e' , 'l' , 'l' , 'o'};
    string s(vc.data() , vc.size());
    cout << s;
}