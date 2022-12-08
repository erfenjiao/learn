/**
 *从 list<int> 到 vector<double> 
 * vector<int>
 */

#include<iostream>
#include<vector>
#include<list>
using namespace std;

int main()
{
    vector<int> ivec = {7,6,5,4,3,2,1};
    list<int> ilist = {1,2,3,4,5,6,7};
    //容器类型不同，不能使用拷贝初始化
    //vector<double> ivec(ilist);
    //使用范围初始化
    vector<double> dvec(ilist.begin(), ilist.end());
}
