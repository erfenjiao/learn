#include<iostream>
#include<string>
#include<algorithm>
using namespace std;
 
int main()
{
    string a, b;
    
    getline(cin, a);
    
    for(int i =0; i < a.size(); i++)
    {
        b.push_back(a.at(a.size() - i -1));
    }
    
    cout << b.c_str() << endl;
    reverse(a.begin(),a.end());
    cout << a;
    return 0;
}