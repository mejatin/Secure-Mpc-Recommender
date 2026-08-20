#ifndef vectools  
#define vectools

#include<iostream>
#include<vector>

std::vector<int> addition(std::vector<int> vec1, std::vector<int> vec2);
std::vector<int> subtraction(std::vector<int> vec1, std::vector<int> vec2);
int iproduct(std::vector<int> vec1, std::vector<int> vec2);
std::vector<int> vecT_mat(const std::vector<int>& x,const std::vector<std::vector<int>>& V);


#endif