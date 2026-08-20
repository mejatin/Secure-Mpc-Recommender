#include <iostream>
#include <vector>
#include "includes/filetools.h"
#include "includes/vectools.h"
#include <functional>
#include <cstdlib>

int field(int x)
{
    int r = x % 64;
    if (r < 0)
        r += 64;
    return r;
}

int main(int argv, char **argc)
{
    int i = std::atoi(argc[1]);
    int j = std::atoi(argc[2]);
    int n = std::atoi(argc[3]);
    std::vector<int> k_value;
    std::string s = "k.txt";
    linefetcher(std::ref(s), std::ref(k_value), 0, 1);
    int k = k_value[0];

    std::vector<int> m_value;
    std::string m_path = "m.txt";
    linefetcher(std::ref(m_path), std::ref(m_value), 0, 1);
    int m = m_value[0];

    std::vector<int> Ui0(k);
    std::vector<int> Ui1(k);
    std::string Ui0_path = "newUi0.txt";
    std::string Ui1_path = "newUi1.txt";
    linefetcher(std::ref(Ui0_path), std::ref(Ui0), 0, k);
    linefetcher(std::ref(Ui1_path), std::ref(Ui1), 0, k);
    std::vector<int> final(k);
    final = addition(Ui0, Ui1);
    for (int i = 0; i < final.size(); i++)
    {
        final[i] = field(final[i]);
    }
    std::string U_path = "../globalfiles/U.txt";
    linewriter(std::ref(U_path), std::ref(final), m, i, k); // commandline arguement 0

    // update driver for V matrix. Just for debugging purposes :
    std::string path_to_v0 = "../files/party0/V0_party0.txt";
    std::string path_to_v1 = "../files/party1/V1_party1.txt";

    // defining vectors
    std::vector<int> v0;
    std::vector<int> v1;

    // reading the updated values
    linefetcher(path_to_v0, v0, j, k);
    linefetcher(path_to_v1, v1, j, k);

    // debugging
    //  std::cout<<"v0 : ";
    //  for(int i : v0)
    //  {
    //      std::cout<<i<<" ";
    //  }
    //  std::cout<<"\n";

    // std::cout<<"v1 : ";
    // for(int i : v1)
    // {
    //     std::cout<<i<<" ";
    // }
    // std::cout<<"\n";

    // reads right
    // applying updates

    std::vector<int> final_v = addition(v0, v1);
    for (int i = 0; i < final_v.size(); i++)
    {
        final_v[i] = field(final_v[i]);
    }
    std::string V_path = "../globalfiles/V.txt";
    linewriter(V_path, final_v, n, j, k);

    return 0;
}