#ifndef DPF_H
#define DPF_H

#include <vector>
#include <tuple>

int getInt(int minVal, int maxVal);
std::vector<int> path_maker(int n, int depth);
std::pair<int, int> PRG(int seed);
std::pair<int, int> flag_generator(int seed);
int Xor(int a, int b);

struct node
{
    int seed;
    int flag;
};

typedef std::tuple<
    std::vector<int>,
    std::vector<int>,
    std::vector<int>,
    std::vector<int>,
    std::vector<int>,
    std::vector<int>,
    std::vector<int>,
    std::vector<int>,
    std::vector<int>,
    std::vector<int>,
    std::vector<int>
> DPFOutput;

DPFOutput DPF_generator(int size, int depth, int value, int path_int);

std::vector<int> evalFull(int initial_seed, int initial_flag,
                          std::vector<int> SeedCW, std::vector<int> FlagCW, std::vector<int> FinalCW,
                          const std::vector<int> &DiffIndex, const std::vector<int> &FinalSeedIndexVec,
                          int depth, int real_size);

bool evaluator(std::pair<std::vector<int>, std::vector<int>> dpf, int value, int path);
int nextPowerOf2(int n);

#endif
