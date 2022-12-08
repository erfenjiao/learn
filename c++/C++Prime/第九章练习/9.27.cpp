#include<iostream>
#include<vector>
#include<list>
#include<deque>
#include<forward_list>
using namespace std;
/*
查找并删除　forward_list<int>　中的奇数元素
*/

int main()
{
    forward_list<int> f = {1,2,3,4,5,6,7,8,9};
    auto prev = f.before_begin();  //前驱元素
    auto cur = f.begin();          //当前元素
    while(cur != f.end())
    {
        if(*cur & 1)  //奇数
        {
            cur = f.erase_after(prev);  //删除，移动到下一位元素
        }
        else  //向前推进
        {
            prev = cur;
            cur++;
        }
    }
}