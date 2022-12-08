#include<iostream>
#include<vector>
#include<list>
using namespace std;

int main()
{
    int ia[] = {0,1,1,2,3,5,8,13,21,55,89};
    vector<int> v;
    list<int> l;
    v.assign(ia , ia+11);
    l.assign(ia , ia+11);

    auto it = v.begin();
    while(it != v.end())
    {
        if(!(*it & 1))
        {
            it = v.erase(it);
        }
        else
        {
            it++;
        }
        
    }
    
}