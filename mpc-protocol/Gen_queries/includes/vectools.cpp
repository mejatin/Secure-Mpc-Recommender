#include<iostream>
#include<vector>

std::vector<int> addition(std::vector<int> vec1, std::vector<int> vec2)
{
    std::vector<int> temp(vec1.size());
    for(int i = 0; i<vec1.size(); i++)
    {
        temp[i] = vec1[i]+vec2[i];
    }
    return temp;
}

std::vector<int> subtraction(std::vector<int> vec1, std::vector<int> vec2)
{
    std::vector<int> temp(vec1.size());
    for(int i = 0; i<vec1.size(); i++)
    {
        temp[i] = vec1[i]-vec2[i];
    }
    return temp;
}


int iproduct(std::vector<int> vec1, std::vector<int> vec2)
{
    int product = 0;
    for(int i = 0; i<vec1.size(); i++)
    {
        product += vec1[i]*vec2[i];
    }
    return product;
}
