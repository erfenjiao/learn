#include<iostream>
using namespace std;
//-------------------2.14-------------------------
/*
int main()
{
    int i = 100 , sum = 0;
    for(int i = 0 ; i != 10 ;++i)
    {
        sum += i;
        cout << sum << endl;
    }
    std::cout << i << " " << sum << std::endl;
}
*/
//-------------------2.16-------------------------
/*
int main()
{
    int i = 1 , &r1 = i;
    double d = 2 , &r2 = d;
    r2 = r1;
    cout << r2 << endl;        //1
    i = r2;
    r1 = d;
}
*/
//-------------------2.17-------------------------
/*
int main()
{
   int i , &ri = i;
   i = 5 , ri = 10;
   cout << i << " " << ri << endl; // 10  10
}
*/
////-------------------2.20------------------------
/*
int main()
{
    int i = 42;
    int *pi = &i;
    cout << "*pi = " << *pi << endl;
    *pi = *pi * *pi;
    cout << "*pi = " << *pi << endl;
}
*/
//-------------------2.20------------------------
