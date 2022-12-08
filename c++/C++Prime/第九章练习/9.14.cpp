#include<iostream>
#include<vector>
#include<list>
using namespace std;

int main()
{
    list<char*> i1={"hello" , "world" , "!"};
    vector<string> s1 ;
    s1.assign(i1.begin(), i1.end());
}