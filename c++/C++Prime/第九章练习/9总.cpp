/**
 *编写函数，接受一对迭代器和一个　ｉｎｔ　值，在两个迭代器指定的范围中查找指定的值 
 * 返回一个布尔值来指出是否找到
 */
#include<iostream>
#include<vector>
#include<list>
#include<deque>
using namespace std;





/*------9.16------
int main()
{
    vector<int> v = {1,2,3,4,5,6};
    list<int> l = {1,2,3,4,5,6,7};
    if(v.size() != l.size())
    {
        cout << "not equal!" << endl;
        return 0;
    }
    auto lb = l.begin();
    auto le = l.end();
    auto vb = v.begin();
    //auto ve = v.end();
    for( ; lb != le ; lb++,vb++)
    {
        if(*lb != *vb)
        {
            cout << "not equal!" << endl;
            return 0;
        }
    }
    cout << "equal" << endl;
}
*/

/*------9.15------
int main()
{
    vector<int> v1 = {1,2,3,4,5,6};
    vector<int> v2 = {1,2,3,4,5,6,7,8};
    if(v1 == v2)
    {
        cout << "equal" << endl;
    }
    else
    {
        cout << "not equal" << endl;
    }
    
}
*/

/*---9.14---
int main()
{
    list<char*> i1={"hello" , "world" , "!"};
    vector<string> s1 ;
    s1.assign(i1.begin(), i1.end());
}
*/


/*-----------9.13----------------
int main()
{
    vector<int> ivec = {7,6,5,4,3,2,1};
    list<int> ilist = {1,2,3,4,5,6,7};
    //容器类型不同，不能使用拷贝初始化
    //vector<double> ivec(ilist);
    //使用范围初始化
    vector<double> dvec(ilist.begin(), ilist.end());
}
*/



/*------------9.4----------------
bool find(vector<int>::iterator begin , vector<int>::iterator end , int x)
{
    for( ; begin != end ; begin++)
    {
        if(*begin == x)
        {
            return true;
        }
    }
    return false;
}
int main()
{
    vector<int> list={1,2,3,4,5,6,7,8,9,10};
    cout << find(list.begin() , list.end(), 3) << endl;
    cout << find(list.begin() , list.end(),8) << endl;

}
*/