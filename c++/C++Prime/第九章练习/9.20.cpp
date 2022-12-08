#include<iostream>
#include<vector>
#include<list>
#include<deque>
using namespace std;

int main()
{
    list<int> l = {1,2,3,4,5,6,7,8,9,10};
    deque<int> d1,d2;
    for(auto it = l.begin() ; it != l.end() ; it++)
    {
        if(*it & 1)
        {
            d1.push_back(*it);
        }
        else
        {
            d2.push_back(*it);
        }
        
    }
    for(auto it = d1.begin() ; it != d1.end() ; it++)
    {
        cout << *it << "  " ;
        
    }
}