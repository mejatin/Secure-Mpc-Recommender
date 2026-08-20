#include <iostream>
#include <functional>
#include <vector>
#include <functional>
#include "includes/vectools.h"
#include <cstdlib>
#include "includes/filetools.h"

// 1 arguement needed at compile time, that is 'n' (size of item profiles)

int main(int argv, char **argc)
{
    int n = std::atoi(argc[1]);
    std::vector<int> k_value;
    std::string s = "k.txt";
    linefetcher(std::ref(s), std::ref(k_value), 0, 1);
    int k = k_value[0];

    std::vector<int> X0(k);
    std::vector<int> Y0(k);
    std::vector<int> X0Y1;
    std::vector<int> oneshare0;
    std::vector<int> X1(k);
    std::vector<int> Y1(k);
    std::vector<int> X1Y0;
    std::vector<int> oneshare1;
    std::string X0_path = "../files/party0/Recieved/X0.txt";
    std::string Y0_path = "../files/party0/Recieved/Y0.txt";
    std::string X0Y1_path = "../files/party0/Recieved/X0Y1masked.txt";
    std::string oneshare0_path = "../files/party0/Recieved/oneshare0.txt";
    std::string X1_path = "../files/party1/Recieved/X1.txt";
    std::string Y1_path = "../files/party1/Recieved/Y1.txt";
    std::string X1Y0_path = "../files/party1/Recieved/X1Y0masked.txt";
    std::string oneshare1_path = "../files/party1/Recieved/oneshare1.txt";
    for (int i = 0; i < k; i++)
    {
        X0[i] = getRandomInt(-10, 10);
        Y0[i] = getRandomInt(-10, 10);
        X1[i] = getRandomInt(-10, 10);
        Y1[i] = getRandomInt(-10, 10);
    }

    int alpha = getRandomInt(-5, 5);
    int maskedX0Y1;
    int maskedX1Y0;

    maskedX0Y1 = iproduct(X0, Y1);
    maskedX0Y1 = maskedX0Y1 + alpha;
    maskedX1Y0 = iproduct(X1, Y0);
    maskedX1Y0 = maskedX1Y0 - alpha;
    int one_share0 = getRandomInt(-5, 5);
    int one_share1 = 1 - one_share0;

    X0Y1.push_back(maskedX0Y1);
    X1Y0.push_back(maskedX1Y0);
    oneshare0.push_back(one_share0);
    oneshare1.push_back(one_share1);

    newlinewriter(std::ref(X0_path), std::ref(X0), k);
    newlinewriter(std::ref(Y0_path), std::ref(Y0), k);
    newlinewriter(std::ref(X0Y1_path), std::ref(X0Y1), 1);
    newlinewriter(std::ref(oneshare0_path), std::ref(oneshare0), 1);

    newlinewriter(std::ref(X1_path), std::ref(X1), k);
    newlinewriter(std::ref(Y1_path), std::ref(Y1), k);
    newlinewriter(std::ref(X1Y0_path), std::ref(X1Y0), 1);
    newlinewriter(std::ref(oneshare1_path), std::ref(oneshare1), 1);

    

    // Doing it for DPF's using actual Beaver triplets
    std::vector<int> a; // size n
    std::vector<int> b; // size n

    // initializing a
    for (int i = 0; i < n; i++)
    {
        a.push_back(getRandomInt(-5, 5));
    }

    // initializing b
    for (int i = 0; i < n; i++)
    {
        b.push_back(getRandomInt(-5, 5));
    }

    // c
    int c = iproduct(a, b);

    // Producing Additive shares of a,b,c [the beaver triplets]
    std::vector<int> a0;
    // initializing a0
    for (int i = 0; i < n; i++)
    {
        a0.push_back(getRandomInt(-5, 5));
    }

    std::vector<int> b0;
    // initializing b0
    for (int i = 0; i < n; i++)
    {
        b0.push_back(getRandomInt(-5, 5));
    }

    // Initializing C0
    int c00 = getRandomInt(-5, 5);
    std::vector<int> c0;
    c0.push_back(c00);

    std::vector<int> a1;
    // Filling a1
    for (int i = 0; i < n; i++)
    {
        a1.push_back(a[i] - a0[i]);
    }

    std::vector<int> b1;
    // Filling b1
    for (int i = 0; i < n; i++)
    {
        b1.push_back(b[i] - b0[i]);
    }

    // Filling c1
    int c11 = c - c00;
    std::vector<int> c1;
    c1.push_back(c11);

    // Sending a0,b0,c0,a1,b1,c1 to thier respective targets

    // paths for server0
    std::string a0_path = "../files/party0/last_item/a0.txt";
    std::string b0_path = "../files/party0/last_item/b0.txt";
    std::string c0_path = "../files/party0/last_item/c0.txt";

    // paths for server1
    std::string a1_path = "../files/party1/last_item/a1.txt";
    std::string b1_path = "../files/party1/last_item/b1.txt";
    std::string c1_path = "../files/party1/last_item/c1.txt";

    // sending to server0
    vectorwriter(a0_path, a0);
    vectorwriter(b0_path, b0);
    vectorwriter(c0_path, c0);

    // sending to server1
    vectorwriter(a1_path, a1);
    vectorwriter(b1_path, b1);
    vectorwriter(c1_path, c1);

    //Debugging----------------------------------------------------------------------------
    // std::cout<<"a : ";
    // for(int i : a)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";

    // std::cout<<"b : ";
    // for(int i : b)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";

    // std::cout<<"c : "<<c<<"\n";

    // std::cout<<"a0 : ";
    // for(int i : a0)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";

    // std::cout<<"b0 : ";
    // for(int i : b0)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";

    // std::cout<<"c0 : ";
    // for(int i : c0)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";

    // std::cout<<"a1 : ";
    // for(int i : a1)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";

    // std::cout<<"b1 : ";
    // for(int i : b1)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";

    // std::cout<<"c1 : ";
    // for(int i : c1)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";

    return 0;
}