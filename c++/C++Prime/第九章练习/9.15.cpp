#include<iostream>
#include<vector>
using namespace std;

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