#include<iostream>
#include<vector>
#include<list>
using namespace std;

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