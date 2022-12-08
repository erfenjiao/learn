#include<iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<fstream>
using namespace std;

int main()
{
    vector<int> vec;
    vec.reserve(10);    //有空间，但无元素
    //fill_n(vec.begin() , 10 , 0);　　算法并不具备向容器中添加元素的功能
    //应改为
    fill_n(back_inserter(vec) , 10 , 0);
    for(auto it = vec.begin() ; it != vec.end() ; it++)
    {
        cout << *it << "  ";
    }
}