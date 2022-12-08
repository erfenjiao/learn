#include<iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<fstream>
using namespace std;

inline void output_words(vector<string> &words)
{
    for(auto it = words.begin() ; it != words.end() ; it++)
    {
        cout << *it << "  ";
    }
    cout << endl;
}

void elimDups(vector<string> & words)
{
    output_words(words);

    sort(words.begin() , words.end());
    output_words(words);

    auto end_unique = unique(words.begin() , words.end());
    output_words(words);

    words.erase(end_unique , words.end());
    output_words(words);
}


int main()
{
    vector<string>words;
    string word;
    while(cin >> word)
    {
        words.push_back(word);
    }
    elimDups(words);
}