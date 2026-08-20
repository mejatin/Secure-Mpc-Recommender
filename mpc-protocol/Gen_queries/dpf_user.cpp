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

int main(int argv, char **argc)
{
    int n = atoi(argc[1]);
    int path = atoi(argc[2]);
    DPFOutput DPF;
    int optimized_size = nextPowerOf2(n);
    int depth = std::log2(optimized_size);
    DPF = DPF_generator(optimized_size, depth, 0, path); // Needs to be taken care of (Last arguement)

    std::vector<int> tree1_leaves = std::get<7>(DPF);
    std::vector<int> tree2_leaves = std::get<8>(DPF);

    // To be Splitted
    std::vector<int> Seeds = std::get<0>(DPF); // Needs to be cracked

    // Party0 Things
    std::vector<int> seed1;
    seed1.push_back(Seeds[0]);
    std::vector<int> SeedCW_p1 = std::get<1>(DPF);  // As it is
    std::vector<int> FlagCW_p1 = std::get<3>(DPF);  // As it is
    std::vector<int> FinalCW_p1 = std::get<5>(DPF); // As it is

    // Party1 Things
    std::vector<int> seed2;
    seed2.push_back(Seeds[1]);
    std::vector<int> SeedCW_p2 = std::get<2>(DPF);  // As it is
    std::vector<int> FlagCW_p2 = std::get<4>(DPF);  // As it is
    std::vector<int> FinalCW_p2 = std::get<6>(DPF); // As it is

    // Send to both
    std::vector<int> DiffIndex = std::get<9>(DPF);          // As it is
    std::vector<int> FinalSeedIndexVec = std::get<10>(DPF); // As it is

    // Paths for party 0
    std::string p0_seed_path = "../files/party0/Item/seed.txt";
    std::string p0_Seed_CW_path = "../files/party0/Item/seed_cw.txt";
    std::string p0_Flag_CW_path = "../files/party0/Item/flag_cw.txt";
    std::string p0_Final_CW_path = "../files/party0/Item/finalcw.txt";
    std::string p0_diffindex_path = "../files/party0/Item/diffindex.txt";
    std::string p0_FinalSeedIndexVec_path = "../files/party0/Item/finalseedindexvec.txt";

    // Paths For party 1
    std::string p1_seed_path = "../files/party1/Item/seed.txt";
    std::string p1_Seed_CW_path = "../files/party1/Item/seed_cw.txt";
    std::string p1_Flag_CW_path = "../files/party1/Item/flag_cw.txt";
    std::string p1_Final_CW_path = "../files/party1/Item/finalcw.txt";
    std::string p1_diffindex_path = "../files/party1/Item/diffindex.txt";
    std::string p1_FinalSeedIndexVec_path = "../files/party1/Item/finalseedindexvec.txt";

    // Sending to party1
    vectorwriter(p0_seed_path, seed1);
    vectorwriter(p0_Seed_CW_path, SeedCW_p1);
    vectorwriter(p0_Flag_CW_path, FlagCW_p1);
    vectorwriter(p0_Final_CW_path, FinalCW_p1);
    vectorwriter(p0_diffindex_path, DiffIndex);
    vectorwriter(p0_FinalSeedIndexVec_path, FinalSeedIndexVec);
    vectorwriter("../files/party0/Item/depth.txt", std::vector<int>{depth});
    vectorwriter("../files/party0/Item/optimized_size.txt", std::vector<int>{optimized_size});
    vectorwriter("../files/party0/Item/real_size.txt", std::vector<int>{n});

    // Sending to party2
    vectorwriter(p1_seed_path, seed2);
    vectorwriter(p1_Seed_CW_path, SeedCW_p2);
    vectorwriter(p1_Flag_CW_path, FlagCW_p2);
    vectorwriter(p1_Final_CW_path, FinalCW_p2);
    vectorwriter(p1_diffindex_path, DiffIndex);
    vectorwriter(p1_FinalSeedIndexVec_path, FinalSeedIndexVec);
    vectorwriter("../files/party1/Item/depth.txt", std::vector<int>{depth});
    vectorwriter("../files/party1/Item/optimized_size.txt", std::vector<int>{optimized_size});
    vectorwriter("../files/party1/Item/real_size.txt", std::vector<int>{n});

    // for (int i : tree1_leaves)
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << "\n";

    // for (int i : tree2_leaves)
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << "\n";

    // DPF for selector-----------------------------------
    DPFOutput DPF1;
    DPF1 = DPF_generator(optimized_size, depth, 1, path); // Needs to be taken care of

    std::vector<int> DPF1_tree1_leaves = std::get<7>(DPF1);
    std::vector<int> DPF1_tree2_leaves = std::get<8>(DPF1);

    // To be Splitted
    std::vector<int> DPF1_Seeds = std::get<0>(DPF1); // Needs to be cracked

    // Party0 Things
    std::vector<int> DPF1_seed1;
    DPF1_seed1.push_back(DPF1_Seeds[0]);
    std::vector<int> DPF1_SeedCW_p1 = std::get<1>(DPF1);  // As it is
    std::vector<int> DPF1_FlagCW_p1 = std::get<3>(DPF1);  // As it is
    std::vector<int> DPF1_FinalCW_p1 = std::get<5>(DPF1); // As it is

    // Party1 Things
    std::vector<int> DPF1_seed2;
    DPF1_seed2.push_back(DPF1_Seeds[1]);
    std::vector<int> DPF1_SeedCW_p2 = std::get<2>(DPF1);  // As it is
    std::vector<int> DPF1_FlagCW_p2 = std::get<4>(DPF1);  // As it is
    std::vector<int> DPF1_FinalCW_p2 = std::get<6>(DPF1); // As it is

    // Send to both
    std::vector<int> DPF1_DiffIndex = std::get<9>(DPF1);          // As it is
    std::vector<int> DPF1_FinalSeedIndexVec = std::get<10>(DPF1); // As it is

    // Paths for party 0
    std::string DPF1_p0_seed_path = "../files/party0/Item1/seed.txt";
    std::string DPF1_p0_Seed_CW_path = "../files/party0/Item1/seed_cw.txt";
    std::string DPF1_p0_Flag_CW_path = "../files/party0/Item1/flag_cw.txt";
    std::string DPF1_p0_Final_CW_path = "../files/party0/Item1/finalcw.txt";
    std::string DPF1_p0_diffindex_path = "../files/party0/Item1/diffindex.txt";
    std::string DPF1_p0_FinalSeedIndexVec_path = "../files/party0/Item1/finalseedindexvec.txt";

    // Paths For party 1
    std::string DPF1_p1_seed_path = "../files/party1/Item1/seed.txt";
    std::string DPF1_p1_Seed_CW_path = "../files/party1/Item1/seed_cw.txt";
    std::string DPF1_p1_Flag_CW_path = "../files/party1/Item1/flag_cw.txt";
    std::string DPF1_p1_Final_CW_path = "../files/party1/Item1/finalcw.txt";
    std::string DPF1_p1_diffindex_path = "../files/party1/Item1/diffindex.txt";
    std::string DPF1_p1_FinalSeedIndexVec_path = "../files/party1/Item1/finalseedindexvec.txt";

    // Sending to party1
    vectorwriter(DPF1_p0_seed_path, DPF1_seed1);
    vectorwriter(DPF1_p0_Seed_CW_path, DPF1_SeedCW_p1);
    vectorwriter(DPF1_p0_Flag_CW_path, DPF1_FlagCW_p1);
    vectorwriter(DPF1_p0_Final_CW_path, DPF1_FinalCW_p1);
    vectorwriter(DPF1_p0_diffindex_path, DPF1_DiffIndex);
    vectorwriter(DPF1_p0_FinalSeedIndexVec_path, DPF1_FinalSeedIndexVec);

    // Sending to party2
    vectorwriter(DPF1_p1_seed_path, DPF1_seed2);
    vectorwriter(DPF1_p1_Seed_CW_path, DPF1_SeedCW_p2);
    vectorwriter(DPF1_p1_Flag_CW_path, DPF1_FlagCW_p2);
    vectorwriter(DPF1_p1_Final_CW_path, DPF1_FinalCW_p2);
    vectorwriter(DPF1_p1_diffindex_path, DPF1_DiffIndex);
    vectorwriter(DPF1_p1_FinalSeedIndexVec_path, DPF1_FinalSeedIndexVec);

    // Printing leaves for debugging purposes
    // std::cout << "Leaves of tree1 for dpf 1 : ";
    // for (int i : DPF1_tree1_leaves)
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << "\n";

    // std::cout << "Leaves of tree2 for dpf 1 : ";
    // for (int i : DPF1_tree2_leaves)
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << "\n";
    return 0;
}