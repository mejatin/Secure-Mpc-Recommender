#include<iostream>
#include<vector>
#include "includes/filetools.h"
#include "includes/vectools.h"
#include<functional>

int main()
{
    std::vector<int> k_value;
    std::string s = "k.txt";
    linefetcher(std::ref(s) , std::ref(k_value) , 0 ,1);
    int k   = k_value[0];
    std::vector<int> L0(k);
    std::vector<int> L1(k);
    int M0int;
    int M1int;
    std::vector<int> M0;
    std::vector<int> M1;
    std::vector<int> maskedL0M1(k);
    std::vector<int> maskedL1M0(k);
    std::vector<int> alpha(k);
    for(int i = 0; i<k ; i++)
    {
        L0[i] = getRandomInt(-10,10);
        L1[i] = getRandomInt(-10,10);
        alpha[i] = getRandomInt(-10,10);
    }
    M0int = getRandomInt(-10,10);
    M1int = getRandomInt(-10,10);
    M0.push_back(M0int);
    M1.push_back(M1int);
    for(int i = 0; i<k ; i++)
    {
        maskedL0M1[i] = (L0[i] * M1int) + alpha[i];
        maskedL1M0[i] = (L1[i] * M0int) - alpha[i];
    }
 

    // std::cout<<"L0 : ";
    // for(int i : L0)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";
    // std::cout<<"L1 : ";
    // for(int i : L1)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";
    // std::cout<<"alpha : ";
    // for(int i : alpha)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";
    // std::cout<<"M0 : ";
    // std::cout<<M0int;
    // std::cout<<"\n";
    // std::cout<<"M1 : ";
    // std::cout<<M1int;
    // std::cout<<"\n";
    // std::cout<<"L0M1+ALPHA : ";
    // for(int i : maskedL0M1)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";
    // std::cout<<"L1m0-ALPHA : ";
    // for(int i : maskedL1M0)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";

    std::string L0_path = "../files/party0/Recieved2/L0.txt";
    std::string M0_path = "../files/party0/Recieved2/M0.txt";
    std::string L0M1_path = "../files/party0/Recieved2/maskedL0M1.txt";
    std::string L1_path = "../files/party1/Recieved2/L1.txt";
    std::string M1_path = "../files/party1/Recieved2/M1.txt";
    std::string L1M0_path = "../files/party1/Recieved2/maskedL1M0.txt";
    newlinewriter(std::ref(L0_path), std::ref(L0), k);
    newlinewriter(std::ref(M0_path), std::ref(M0), 1);
    newlinewriter(std::ref(L0M1_path), std::ref(maskedL0M1), k);
    newlinewriter(std::ref(L1_path), std::ref(L1), k);
    newlinewriter(std::ref(M1_path), std::ref(M1), 1);
    newlinewriter(std::ref(L1M0_path), std::ref(maskedL1M0), k);

    
    return 0;
}