#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <map>
#include <unordered_map>
#include <dirent.h>

#include <iostream>

#include "mpi.h"

using namespace std;

void getFiles(string path, vector<string>& filenames)
{
    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str()))){
        return;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0){
            filenames.push_back(path + "/" + ptr->d_name);
    }
    }
    closedir(pDir);
}

std::string readFile(std::string filename) {
    std::ifstream in(filename);

    in.seekg(0, std::ios::end);
    size_t len = in.tellg();
    in.seekg(0);
    std::string contents(len + 1, '\0');
    in.read(&contents[0], len);
    return contents;
}

std::vector<std::string> split(std::string const &input) { 
    std::istringstream buffer(input);
    std::vector<std::string> ret((std::istream_iterator<std::string>(buffer)), 
                                 std::istream_iterator<std::string>());
    return ret;
}

std::vector<std::string> getWords(
    std::string &content, int rank, int worldsize) {
    std::vector<std::string> wordList = split(content);
    std::vector<std::string> re;
    std::string tmp;
    for (int i = 0 ; i < wordList.size(); i++) {
        if (i % worldsize) {
            // re.push_back(wordList[i]);
            tmp += " " + wordList[i];
        }
    }
    re.push_back(tmp);
    return re;
}

std::vector<pair<std::string, int>> countWords(
    std::vector<std::string> &contentList) {
    // split words
    std::vector<std::string> wordList;
    std::string concat_content;
    for (auto it = contentList.begin(); it != contentList.end(); it++) {
        std::string content = (*it);
        concat_content += " " + content;
    }
    wordList = split(concat_content);

    // do the word count
    std::map<std::string, int> counts;
    for (auto it = wordList.begin(); it != wordList.end(); it++) {
        if (counts.find(*it) != counts.end()) {
            counts[*it] += 1;
        } else {
            counts[*it] = 1;
        }
    }
    std::vector<pair<std::string, int>> res;
    for (auto it = counts.begin(); it != counts.end(); it++) {
        res.push_back(std::make_pair(it->first, it->second));
    }
    return res;
}

std::vector<pair<std::string, int>> mergeCounts(
    std::vector<pair<std::string, int>> &countListA,
    std::vector<pair<std::string, int>> &countListB) {
    std::map<std::string, int> counts;
    for (auto it = countListA.begin(); it != countListA.end(); it++) {
        counts[it->first] = it->second;
    }
    for (auto it = countListB.begin(); it != countListB.end(); it++) {
        if (counts.find(it->first) == counts.end())
            counts[it->first] = it->second;
        else
            counts[it->first] += it->second;
    }
    std::vector<pair<std::string, int>> res;
    for (auto it = counts.begin(); it != counts.end(); it++) {
        res.push_back(std::make_pair(it->first, it->second));
    }
    return res;
}

void sendLocalCounts(int from, int to,
                     std::vector<pair<std::string, int>> &counts) {
    int num = counts.size();
    MPI_Send(&num, 1, MPI_INT, to, from, MPI_COMM_WORLD);

    if (num) {
        int *counts_array = new int[num];
        int i = 0;
        for (auto it = counts.begin(); it != counts.end(); it++, i++) {
            counts_array[i] = it->second;
        }
        MPI_Send(counts_array, num, MPI_INT, to, from, MPI_COMM_WORLD);
        delete counts_array;
    }

    std::string words = " ";
    for (auto it = counts.begin(); it != counts.end(); it++) {
        words += it->first;
        words += " ";
    }
    num = words.length();
    MPI_Send(&num, 1, MPI_INT, to, from, MPI_COMM_WORLD);
    if (num) {
        char *_words = new char[num];
        words.copy(_words, num);
        MPI_Send(_words, num, MPI_CHAR, to, from, MPI_COMM_WORLD);
        delete _words;
    }
}

void recvCounts(int from, int to, std::vector<pair<std::string, int>> &counts) {
    MPI_Status status;
    int _num = 0, num = 0;
    int *counts_array;
    char *_words;
    std::string words;
    MPI_Recv(&_num, 1, MPI_INT, from, from, MPI_COMM_WORLD, &status);
    if (_num) {
        counts_array = new int[_num];
        MPI_Recv(counts_array, _num, MPI_INT, from, from, MPI_COMM_WORLD, &status);
    }

    MPI_Recv(&num, 1, MPI_INT, from, from, MPI_COMM_WORLD, &status);
    if (num) {
        _words = new char[num];
        MPI_Recv(_words, num, MPI_CHAR, from, from, MPI_COMM_WORLD, &status);
        
        for (int _i = 0; _i < num;  _i++) words+=_words[_i];
        delete _words;
    }

    if (_num) {
        std::vector<std::string> word_vec = split(words);
        for (int i = 0; i < _num; i++) {
            counts.push_back(std::make_pair(word_vec[i], counts_array[i]));
        }
        delete counts_array;
    }
}

void treeMerge(int id, int worldSize,
               std::vector<pair<std::string, int>> &counts) {
    int participants = worldSize;
    while (participants > 1) {
        MPI_Barrier(MPI_COMM_WORLD);
        int _participants = participants / 2 + (participants % 2 ? 1 : 0);
        if (id < _participants) {
            if (id + _participants < participants) {
                std::vector<pair<std::string, int>> _counts;
                std::vector<pair<std::string, int>> temp;
                recvCounts(id + _participants, id, _counts);
                temp = mergeCounts(_counts, counts);
                counts = temp;
            }
        }
        if (id >= _participants && id < participants) {
            sendLocalCounts(id, id - _participants, counts);
        }
        participants = _participants;
    }
}

int main(int argc, char *argv[]) {
    int rank;
    int worldSize;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /*
    * Word Count for big file
    */
{
    struct timeval start, stop;
    gettimeofday(&start, NULL);
    std::string big_file = "./project_file/big_file/big_100.txt";
    auto content = readFile(big_file);
    auto partContent = getWords(content, rank, worldSize);

    auto counts = countWords(partContent);

    treeMerge(rank, worldSize, counts);
    gettimeofday(&stop, NULL);
    if (rank == 0) {
        cout << "word count: "
             << (stop.tv_sec - start.tv_sec) * 1000.0 +
                    (stop.tv_usec - start.tv_usec) / 1000.0
             << " ms"<< endl;
    }
    /* 
    if (rank == 0) {
        for (auto it = counts.begin(); it != counts.end(); it++) {
            std::cout << it->first << " : " << it->second << endl;
        }
    }
    */ 
}
    /*
    * Word Count for small file
    */
{
    struct timeval start, stop;
    gettimeofday(&start, NULL);
    std::string small_file_folder = "./project_file/small_file/";
    std::vector<std::string> _small_files, small_files;
    getFiles(small_file_folder, _small_files);

    for (auto it = _small_files.begin(); it!= _small_files.end(); it++) {
        std::size_t _hs = std::hash<std::string>{}(*it);
        if (int(_hs % worldSize) == rank) {
            small_files.push_back(*it);
        }
    }

    std::vector<std::string> wdlst;
    for (int i = 0; i < small_files.size(); i++) {
        auto content = readFile(small_files[i]);
        wdlst.push_back(content);
    }
    auto counts = countWords(wdlst);

    treeMerge(rank, worldSize, counts);
    gettimeofday(&stop, NULL);
    if (rank == 0) {
        cout << "word count: "
             << (stop.tv_sec - start.tv_sec) * 1000.0 +
                    (stop.tv_usec - start.tv_usec) / 1000.0
             << " ms"<< endl;
    }

    // if (rank == 0) {
    //     for (auto it = counts.begin(); it != counts.end(); it++) {
    //         std::cout << it->first << " : " << it->second << endl;
    //     }
    // }
}
    MPI_Finalize();
    return 0;
}
