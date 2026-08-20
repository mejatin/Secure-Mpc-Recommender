#include <iostream>
#include <random>
#include <functional>
#include <vector>
#include <string>
#include "includes/MPCshares.h"
#include "includes/filetools.h"
#include "includes/vectools.h"
#include <cstdlib>
#include "includes/dpf_tools.h"

int field(int x)
{
    int r = x % 64;
    if (r < 0)
        r += 64;
    return r;
}

void init(std::vector<std::vector<int>> &victim, int row, int column)
{
    victim = std::vector<std::vector<int>>(row, std::vector<int>(column));
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            victim[i][j] = getRandomInt(0, 63);
        }
    }
}

int query_index_creator(int row, int column)
{
    return getRandomInt(row, column);
}

int main(int argv, char **argc)
{
    int m, n, k, number_of_queries;
    // setter(m,n,k,number_of_queries);
    m = atoi(argc[1]);
    n = atoi(argc[2]);
    k = atoi(argc[3]);
    number_of_queries = atoi(argc[4]);
    MPCstorage U;
    init(std::ref(U.victim), m, k);
    MPCstorage V;
    init(std::ref(V.victim), n, k);
    std::string U_path = "../globalfiles/U.txt";
    std::string V_path = "../globalfiles/V.txt";
    shareswriter(std::ref(U_path), std::ref(U.victim), m, k);
    shareswriter(std::ref(V_path), std::ref(V.victim), n, k);

    MPCstorage U0_party0;
    MPCstorage V0_party0;
    MPCstorage U1_party1;
    MPCstorage V1_party1;
    U1_party1.victim.resize(m, std::vector<int>(k));
    V1_party1.victim.resize(n, std::vector<int>(k));
    init(std::ref(U0_party0.victim), m, k);
    init(std::ref(V0_party0.victim), n, k);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < k; j++)
        {
            U1_party1.victim[i][j] = field(U.victim[i][j] - U0_party0.victim[i][j]);
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < k; j++)
        {
            V1_party1.victim[i][j] = field(V.victim[i][j] - V0_party0.victim[i][j]);
        }
    }
    std::string U0_party0_file = "../files/party0/U0_party0.txt";
    std::string V0_party0_file = "../files/party0/V0_party0.txt";
    std::string U1_party1_file = "../files/party1/U1_party1.txt";
    std::string V1_party1_file = "../files/party1/V1_party1.txt";
    shareswriter(std::ref(U0_party0_file), std::ref(U0_party0.victim), m, k);
    shareswriter(std::ref(V0_party0_file), std::ref(V0_party0.victim), n, k);
    shareswriter(std::ref(U1_party1_file), std::ref(U1_party1.victim), m, k);
    shareswriter(std::ref(V1_party1_file), std::ref(V1_party1.victim), n, k);
    std::vector<std::vector<int>> queries(number_of_queries, std::vector<int>(2));
    for (int i = 0; i < number_of_queries; i++)
    {
        queries[i][0] = query_index_creator(0, m - 1);
        queries[i][1] = query_index_creator(0, n - 1);
    }
    std::string queries_path = "../globalfiles/queries.txt";
    shareswriter(std::ref(queries_path), std::ref(queries), number_of_queries, 2);
    std::string xy = "../Communication/k.txt";
    std::vector<std::vector<int>> kkk(1, std::vector<int>(1));
    kkk[0][0] = k;
    shareswriter(std::ref(xy), std::ref(kkk), 1, 1);
    std::string party0 = "../files/party0/k.txt";
    std::vector<std::vector<int>> kkkk(1, std::vector<int>(1));
    kkkk[0][0] = k;
    shareswriter(std::ref(party0), std::ref(kkkk), 1, 1);
    std::string party1 = "../files/party1/k.txt";
    std::vector<std::vector<int>> kkkkk(1, std::vector<int>(1));
    kkkkk[0][0] = k;
    shareswriter(std::ref(party1), std::ref(kkkkk), 1, 1);

    std::string updater_k = "../Updater/k.txt";
    std::vector<std::vector<int>> updater_k_(1, std::vector<int>(1));
    updater_k_[0][0] = k;
    shareswriter(std::ref(updater_k), std::ref(updater_k_), 1, 1);

    std::string updater_m = "../Updater/m.txt";
    std::vector<std::vector<int>> updater_m_(1, std::vector<int>(1));
    updater_m_[0][0] = m;
    shareswriter(std::ref(updater_m), std::ref(updater_m_), 1, 1);

    return 0;
}