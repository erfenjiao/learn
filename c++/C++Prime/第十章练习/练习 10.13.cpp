#include<iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<fstream>
#include<string>
using namespace std;

inline void output_words(vector<string>::iterator beg , vector<string>::iterator end)
{
    for(auto it = beg ; it != end ; it++)
    {
        cout << *it << "  ";
    }
    cout << endl;
}

bool five_or_more(const string &s1)
{
    return s1.size() >= 5;
}

int main(int args , char * argv[])
{
    ifstream in(argv[1]);
    if( !in )
    {
        cout << "打开文件失败" << endl;
        exit(1);
    }

    vector<int> words;
    string word;
    // while(in >> word)
    // {
    //     words.push_back(word);
    // }
    // output_words(words.begin() , words.end() , five_or_more);
    // output_words(words.begin() , words.end() , five_or_more);                                                                           

}