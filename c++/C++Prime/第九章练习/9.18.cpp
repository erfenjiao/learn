#include<iostream>
#include<vector>
#include<deque>
#include<string>
using namespace std;

int main()
{
    deque<string> d = {"hello" , "world" , "erfenjiao"};
    string word;
    while(cin >> word)
    {
        d.push_back(word);
    }
    for(auto di = d.cbegin() ; di != d.cend() ; di++)
    {
        cout << *di << endl;
    }
}
