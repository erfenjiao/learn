#include<iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<fstream>
#include<string>
using namespace std;
/*
10.3.2 lambda　表达式
    有时候希望进行的操作有更多的参数
介绍lambda
    [capture list] (parameter list) -> return type { function body }
    可以忽略参数列表和返回类型，但永远有捕获列表和函数体
    auto f = [] {return 42;}
向lambda传递参数
    //空捕获列表表示此lambda不使用它所在函数中的任何局部变量
        [] (const string &s1 , const string &s2)  
        {
            return s1.size() < s2.size();
        };
使用捕获列表
    lambda　只有在其捕获列表中捕获一个它所在函数中的局部变量，才能在函数体中使用该变量
调用 find_if

*/
void elimDups(vector<string> &words)
{
    //安字典序排序ｗｏｒｄｓ　，　以便查找重复单词
    sort(words.begin() , words.end());

    auto end_unique = unique(words.begin() , words.end());

    words.erase(end_unique , words.end());

}

// void biggies(vector<string> & words , vector<string>::size_type sz)
// {
//     elimDups(words);

//     stdble_sort(words.begin() , words.end() , [](const string &a , const string &b)
//                                               {return a.size() >= sz;});

//     auto wc = find_if()

// }
// {

// }
//[10.15]
void add(int a)
{
    auto sum2 = [a](int b) {return a+b;};

    cout << sum2(1) << endl;
}

int main()
{
    // auto f = [] { return 42; };
    // cout << f() << endl;
    //空捕获列表表示此lambda不使用它所在函数中的任何局部变量
    // [] (const string &s1 , const string &s2)  
    // {
    //     return s1.size() < s2.size();
    // };
    
    // [sz] (const string &a)
    // {
    //     return a.size() >= sz;
    // };

    //[10.14] 编写一个lambda,接受两个ｉｎｔ，返回他们的和
    //auto sum = [](int a , int b){return a+b;};

    add(1);
    add(2);

    

}