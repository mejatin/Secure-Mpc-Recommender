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

#include <vector>

std::vector<int> vecT_mat(
        const std::vector<std::vector<int>>& V,
        const std::vector<int>& u)
{
    int n_items = V.size();
    if (n_items == 0) return {};
    
    int k_features = u.size();
    if (V[0].size() != k_features) {
        // Error: Dimensions don't match
        return {}; 
    }

    std::vector<int> t(n_items, 0);

    // Loop for each item (row)
    for (int i = 0; i < n_items; i++) {
        // Compute the dot product for this row
        for (int j = 0; j < k_features; j++) {
            t[i] += V[i][j] * u[j];
        }
    }

    return t;
}



