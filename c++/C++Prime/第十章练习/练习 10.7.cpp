#include<iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<fstream>
using namespace std;

int main(int argc , char * argv[])
{
    ifstream in(argv[1]);
    if( !in )
    {
        cout << "打开文件失败" << endl;
        exit(1);
    }

    vector<int> vec;
    list<int> lst;
    int val;
    while(in >> val)
    {
        lst.push_back(val);
    }
    copy(lst.begin() , lst.end() , back_inserter(vec));

    cout <<  equal(lst.begin() , lst.end() , vec.begin()) << endl;

    for(auto it = lst.begin() ; it != lst.end() ; it++)
    {
        cout << *it << "  ";
    }
    cout << endl;

}