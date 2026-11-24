#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <functional>
#include <cstdlib>
#include <limits.h>
#include <thread>
#include "includes/filetools.h"

void runExe(const std::string &exePath, const std::string &args = "")
{
    // get current working directory
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    std::string base = cwd;

    // split exePath into dir + exe
    size_t pos = exePath.find_last_of('/');
    std::string dir = (pos == std::string::npos) ? "." : exePath.substr(0, pos);
    std::string exe = (pos == std::string::npos) ? exePath : exePath.substr(pos + 1);

    // build absolute directory
    std::string absDir;
    if (dir[0] == '/')
        absDir = dir;
    else
        absDir = base + "/" + dir;

    // final execution command
    std::string cmd = "cd \"" + absDir + "\" && ./" + exe;
    if (!args.empty())
        cmd += " " + args;

    int ret = system(cmd.c_str());
    if (ret != 0)
        std::cerr << "[ERROR] Failed: " << cmd << "\n";
}

std::vector<std::vector<int>> readFileToMatrix(const std::string &file_path, int n, int columns)
{
    std::vector<std::vector<int>> matrix;
    for (int i = 0; i < n; ++i)
    {
        std::vector<int> row;
        linefetcher(file_path, row, i, columns);
        matrix.push_back(row);
    }
    return matrix;
}

int main(int argv, char **argc)
{
    struct ExeInfo
    {
        std::string path;
        std::string args;
    };

    int m = std::atoi(argc[1]);
    int n = std::atoi(argc[2]);
    int k = std::atoi(argc[3]);
    int q = std::atoi(argc[4]);


    runExe("../Gen_queries/run1", std::to_string(m) + " " + std::to_string(n) + " " + std::to_string(k) + " " + std::to_string(q));

    std::string path = "../globalfiles/queries.txt";
    std::vector<std::vector<int>> matrix = readFileToMatrix(path, q, 2);
    
    std::string U_path = "../globalfiles/U.txt";
    std::string V_path = "../globalfiles/V.txt";
    std::string U0_path = "../files/party0/U0_party0.txt";
    std::string V0_path = "../files/party0/V0_party0.txt";
    std::string U1_path = "../files/party1/U1_party1.txt";
    std::string V1_path = "../files/party1/V1_party1.txt";

    std::vector<std::vector<int>> U_martix = readFileToMatrix(U_path, m, k);
    std::vector<std::vector<int>> V_martix = readFileToMatrix(V_path, n, k);
    std::vector<std::vector<int>> U0_share = readFileToMatrix(U0_path, m, k);
    std::vector<std::vector<int>> V0_share = readFileToMatrix(V0_path, n, k);
    std::vector<std::vector<int>> U1_share = readFileToMatrix(U1_path, m, k);
    std::vector<std::vector<int>> V1_share = readFileToMatrix(V1_path, n, k);

    std::cout << "The matrix U is : \n";
    for (const auto &row : U_martix)
    {
        for (int val : row)
            std::cout << val << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "The matrix V is : \n";
    for (const auto &row : V_martix)
    {
        for (int val : row)
            std::cout << val << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "The share U0 is : \n";
    for (const auto &row : U0_share)
    {
        for (int val : row)
            std::cout << val << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "The share U1 is : \n";
    for (const auto &row : U1_share)
    {
        for (int val : row)
            std::cout << val << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "The share V0 is : \n";
    for (const auto &row : V0_share)
    {
        for (int val : row)
            std::cout << val << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "The share V1 is : \n";
    for (const auto &row : V1_share)
    {
        for (int val : row)
            std::cout << val << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;

    for (int i = 0; i < q; i++)
    {
        int U = matrix[i][0];
        int V = matrix[i][1];

        std::cout<<"----------------------------------------------------Query "<<i+1<<"-----------------------------------------------------------------\n\n";

        std::cout << "The Query for this iteration is ( " << U << " , " << V << " ). Executing the Qurey : \n";

        // Updated sequence
        runExe("../Gen_queries/run2", std::to_string(n) + " " + std::to_string(V));
        runExe("../Communication/run3", std::to_string(n));
        runExe("../Communication/run4", "");

        std::string args = std::to_string(U) + " " + std::to_string(V) + " " + std::to_string(m) + " " + std::to_string(n);

        // run5 first
        std::thread server_thread([&]()
        { 
            runExe("../files/party0/run5", args); 
        });

        sleep(1); // allow server0 to start first

        // run6 second
        std::thread client_thread([&]()
        { 
            runExe("../files/party1/run6", args); 
        });

        server_thread.join();
        client_thread.join();

        runExe("../Updater/run7", std::to_string(U) + " " + std::to_string(V) + " " + std::to_string(n));

        std::cout << "The Updated U0 after this query is : \n";
        std::vector<std::vector<int>> U00_share = readFileToMatrix(U0_path, m, k);
        for (const auto &row : U00_share)
        {
            for (int val : row)
                std::cout << val << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "The Updated U1 after this query is : \n";
        std::vector<std::vector<int>> U01_share = readFileToMatrix(U1_path, m, k);
        for (const auto &row : U01_share)
        {
            for (int val : row)
                std::cout << val << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "(For Easy Debugging)The Updated matrix U after this iteration is : \n";
        std::vector<std::vector<int>> updated_u = readFileToMatrix(U_path, m, k);
        for (const auto &row : updated_u)
        {
            for (int val : row)
                std::cout << val << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << "The Updated V0 after this query is : \n";
        std::vector<std::vector<int>> V00_share = readFileToMatrix(V0_path, n, k);
        for (const auto &row : V00_share)
        {
            for (int val : row)
                std::cout << val << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "The Updated V1 after this query is : \n";
        std::vector<std::vector<int>> V01_share = readFileToMatrix(V1_path, n, k);
        for (const auto &row : V01_share)
        {
            for (int val : row)
                std::cout << val << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "(For Easy Debugging)The Updated matrix V after this iteration is : \n";
        std::vector<std::vector<int>> updated_v = readFileToMatrix(V_path, n, k);
        for (const auto &row : updated_v)
        {
            for (int val : row)
                std::cout << val << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
