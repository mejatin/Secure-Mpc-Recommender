#ifndef target_fetcher
#define target_fetcher

#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<random>
#include<deque>

void linefetcher(const std::string &file_path, std::vector<int> &linestorer, int rowindex, int columnindex);
void linewriter(const std::string &file_path, std::vector<int> &sample,int no_of_rows, int rownumber, int columnnumber);
void shareswriter(const std::string &filePath,std::vector<std::vector<int>> &source,int row,int column);
int getRandomInt(int , int);
void newlinewriter(const std::string& filePath, std::vector<int>& source, int k);
void copyFile(const std::string& dominantFilePath, const std::string& victimFilePath); 
void vectorwriter(const std::string& filePath, const std::vector<int>& source);
#endif