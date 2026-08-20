#include "dpf_tools.h"
#include <random>
#include <climits>
#include <cmath>

int getInt(int minVal, int maxVal)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(minVal, maxVal);
    return dist(gen);
}

std::vector<int> path_maker(int n, int depth)
{
    std::vector<int> binary(depth, 0);
    for (int i = depth - 1; i >= 0; --i)
    {
        binary[i] = n % 2;
        n /= 2;
    }
    return binary;
}

std::pair<int, int> PRG(int seed)
{
    std::mt19937 gen(seed);
    return {gen(), gen()};
}

std::pair<int, int> flag_generator(int seed)
{
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(0, 1);
    return {dist(gen), dist(gen)};
}

int Xor(int a, int b)
{
    return a ^ b;
}

DPFOutput DPF_generator(int /*size*/, int depth, int value, int path_int)
{
    std::vector<int> Seeds, SeedCW_p1, SeedCW_p2, FlagCW_p1, FlagCW_p2, FinalCW_p1, FinalCW_p2, DiffIndex, FinalSeedIndexVec;
    std::vector<int> path = path_maker(path_int, depth);

    int seed1 = getInt(0, INT32_MAX);
    int seed2 = getInt(0, INT32_MAX);
    Seeds.push_back(seed1);
    Seeds.push_back(seed2);

    int f1 = 1, f2 = 0;
    std::vector<std::vector<node>> tree1(depth + 1), tree2(depth + 1);
    tree1[0].push_back({seed1, f1});
    tree2[0].push_back({seed2, f2});

    for (int i = 0; i < depth; i++)
    {
        // expand both trees
        for (auto &n : tree1[i])
        {
            auto [s1, s2] = PRG(n.seed);
            auto [f1b, f2b] = flag_generator(n.flag);
            tree1[i + 1].push_back({s1, f1b});
            tree1[i + 1].push_back({s2, f2b});
        }
        for (auto &n : tree2[i])
        {
            auto [s1, s2] = PRG(n.seed);
            auto [f1b, f2b] = flag_generator(n.flag);
            tree2[i + 1].push_back({s1, f1b});
            tree2[i + 1].push_back({s2, f2b});
        }

        // find first index where flags differ
        int index_scw = -1;
        for (size_t j = 0; j < tree1[i].size(); j++)
        {
            if (tree1[i][j].flag != tree2[i][j].flag)
            {
                index_scw = static_cast<int>(j);
                break;
            }
        }
        if (index_scw == -1) index_scw = 0;
        DiffIndex.push_back(index_scw);

        int left_child  = 2 * index_scw;
        int right_child = 2 * index_scw + 1;

        // seed correction word (SCW) selection based on path bit
        int SCW = (path[i] == 1)
                  ? Xor(tree1[i + 1][left_child].seed,  tree2[i + 1][left_child].seed)
                  : Xor(tree1[i + 1][right_child].seed, tree2[i + 1][right_child].seed);

        SeedCW_p1.push_back(SCW);
        SeedCW_p2.push_back(0);

        // apply SCW to both children in tree1
        tree1[i + 1][left_child].seed  = Xor(tree1[i + 1][left_child].seed,  SCW);
        tree1[i + 1][right_child].seed = Xor(tree1[i + 1][right_child].seed, SCW);

        // flag correction words (FCWs)
        int FCW0, FCW1;
        if (path[i] == 1)
        {
            FCW0 = Xor(tree1[i + 1][left_child].flag,  tree2[i + 1][left_child].flag);
            FCW1 = Xor(tree1[i + 1][right_child].flag, tree2[i + 1][right_child].flag);
            FCW1 = Xor(FCW1, 1);
        }
        else
        {
            FCW0 = Xor(tree1[i + 1][left_child].flag,  tree2[i + 1][left_child].flag);
            FCW0 = Xor(FCW0, 1);
            FCW1 = Xor(tree1[i + 1][right_child].flag, tree2[i + 1][right_child].flag);
        }
        FlagCW_p1.push_back(FCW0);
        FlagCW_p1.push_back(FCW1);
        FlagCW_p2.push_back(0);
        FlagCW_p2.push_back(0);

        // apply FCWs to tree1
        tree1[i + 1][left_child].flag  = Xor(tree1[i + 1][left_child].flag,  FCW0);
        tree1[i + 1][right_child].flag = Xor(tree1[i + 1][right_child].flag, FCW1);

        // final correction on last level
        if (i == depth - 1)
        {
            int final_idx = -1;
            for (size_t k = 0; k < tree1[i + 1].size(); k++)
            {
                if (tree1[i + 1][k].flag != tree2[i + 1][k].flag)
                {
                    final_idx = static_cast<int>(k);
                    break;
                }
            }
            if (final_idx == -1) final_idx = 0;

            int final_cw = Xor(tree1[i + 1][final_idx].seed, tree2[i + 1][final_idx].seed);
            final_cw = Xor(final_cw, value);
            tree1[i + 1][final_idx].seed = Xor(tree1[i + 1][final_idx].seed, final_cw);

            FinalCW_p1.push_back(final_cw);
            FinalCW_p2.push_back(0);
            FinalSeedIndexVec.push_back(final_idx);
        }
    }

    // collect leaves
    std::vector<int> tree1_leaves, tree2_leaves;
    for (auto &n : tree1[depth]) tree1_leaves.push_back(n.seed);
    for (auto &n : tree2[depth]) tree2_leaves.push_back(n.seed);

    return {
        Seeds, SeedCW_p1, SeedCW_p2, FlagCW_p1, FlagCW_p2,
        FinalCW_p1, FinalCW_p2, tree1_leaves, tree2_leaves,
        DiffIndex, FinalSeedIndexVec
    };
}

std::vector<int> evalFull(int initial_seed, int initial_flag,
                          std::vector<int> SeedCW, std::vector<int> FlagCW, std::vector<int> FinalCW,
                          const std::vector<int> &DiffIndex, const std::vector<int> &FinalSeedIndexVec,
                          int depth, int real_size)
{
    std::vector<std::vector<node>> tree(depth + 1);
    tree[0].push_back({initial_seed, initial_flag});

    for (int i = 0; i < depth; i++)
    {
        // expand
        for (auto &n : tree[i])
        {
            auto [s1, s2] = PRG(n.seed);
            auto [f1, f2] = flag_generator(n.flag);
            tree[i + 1].push_back({s1, f1});
            tree[i + 1].push_back({s2, f2});
        }

        // apply correction words at the diff index
        int idx = DiffIndex[i];
        int left = 2 * idx, right = 2 * idx + 1;

        int SCW = SeedCW.front(); SeedCW.erase(SeedCW.begin());
        tree[i + 1][left].seed  = Xor(tree[i + 1][left].seed,  SCW);
        tree[i + 1][right].seed = Xor(tree[i + 1][right].seed, SCW);

        int FCW0 = FlagCW.front(); FlagCW.erase(FlagCW.begin());
        int FCW1 = FlagCW.front(); FlagCW.erase(FlagCW.begin());
        tree[i + 1][left].flag  = Xor(tree[i + 1][left].flag,  FCW0);
        tree[i + 1][right].flag = Xor(tree[i + 1][right].flag, FCW1);

        // final correction at last level
        if (i == depth - 1 && !FinalSeedIndexVec.empty())
        {
            int final_idx = FinalSeedIndexVec[0];
            int final_cw  = FinalCW.front();
            tree[i + 1][final_idx].seed = Xor(tree[i + 1][final_idx].seed, final_cw);
        }
    }

    std::vector<int> leaves;
    for (auto &n : tree[depth]) leaves.push_back(n.seed);
    if (real_size >= 0 && real_size < static_cast<int>(leaves.size()))
        leaves.resize(real_size);
    return leaves;
}

bool evaluator(std::pair<std::vector<int>, std::vector<int>> dpf, int value, int path)
{
    const auto &a = dpf.first;
    const auto &b = dpf.second;
    for (size_t i = 0; i < a.size() && i < b.size(); i++)
    {
        int x = Xor(a[i], b[i]);
        if (static_cast<int>(i) == path)
        {
            if (x != value) return false;
        }
        else
        {
            if (x != 0) return false;
        }
    }
    return true;
}

int nextPowerOf2(int n)
{
    if (n <= 0) return 1;
    if ((n & (n - 1)) == 0) return n;
    int p = 1;
    while (p < n) p <<= 1;
    return p;
}
