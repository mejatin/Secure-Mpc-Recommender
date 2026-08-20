// server.cpp
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include "includes/filetools.h"
#include "includes/vectools.h"
#include "includes/dpf_tools.h"
#include <functional>


int field(int x)
{
    int r = x % 64; 
    if (r < 0)
        r += 64;
    return r; 
}

int main(int argv, char **argc)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Retry bind if port temporarily unavailable
    while (bind(sockfd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        usleep(500000);
    }

    listen(sockfd, 1);

    int client = -1;
    while (client < 0)
    {
        client = accept(sockfd, nullptr, nullptr);
        if (client < 0)
            usleep(500000);
    }

    char buffer[16];

    // --- Step 1 ---
    // std::cout << "Server: performing operation 1...\n";
    // perform task 1
    int x0row = std::atoi(argc[1]); // commandline - fetch row from queries
    int y0row = std::atoi(argc[2]); // same
    std::vector<int> k_value;
    std::string s = "k.txt";
    linefetcher(std::ref(s), std::ref(k_value), 0, 1);
    int k = k_value[0];
    std::vector<int> x0;
    std::vector<int> y0;
    std::vector<int> X0;
    std::vector<int> Y0;
    std::string x0_path = "U0_party0.txt";
    std::string y0_path = "V0_party0.txt";
    std::string X0_path = "Recieved/X0.txt";
    std::string Y0_path = "Recieved/Y0.txt";
    linefetcher(std::ref(x0_path), std::ref(x0), x0row, k); // 0 to be replaced
    linefetcher(std::ref(y0_path), std::ref(y0), y0row, k); // 0 to be replaced
    linefetcher(std::ref(X0_path), std::ref(X0), 0, k);
    linefetcher(std::ref(Y0_path), std::ref(Y0), 0, k);
    std::vector<int> x0addX0;
    x0addX0 = addition(std::ref(x0), std::ref(X0));
    std::vector<int> y0addY0;
    y0addY0 = addition(std::ref(y0), std::ref(Y0));
    std::string writemaskedx0 = "../party1/Recieved/x0addX0.txt";
    std::string writemaskedy0 = "../party1/Recieved/y0addY0.txt";
    newlinewriter(std::ref(writemaskedx0), std::ref(x0addX0), k);
    newlinewriter(std::ref(writemaskedy0), std::ref(y0addY0), k);

    // FOR DPF
    std::vector<int> user_profile_vector;
    std::string path_to_user_profile = "../../globalfiles/U.txt";
    linefetcher(std::ref(path_to_user_profile), std::ref(user_profile_vector), 0, k); // take care of it

    send(client, "done1", 6, 0); // Sender1 - Done

    recv(client, buffer, sizeof(buffer), 0); // wait for client ack

    // --- Step 2 ---
    // std::cout << "Server: performing operation 2...\n";
    // perform task 2
    // std::vector<int> x0;      // Ui
    // std::vector<int> y0;      // Vi
    // std::vector<int> X0;
    // std::vector<int> Y0;
    std::vector<int> X0Y1masked;
    std::vector<int> x1addX1;
    std::vector<int> y1addY1;

    // std::string x0_path = "U0_party0.txt";
    // std::string y0_path = "V0_party0.txt";
    // std::string X0_path = "Recieved/X0.txt";
    // std::string Y0_path = "Recieved/Y0.txt";
    std::string X1Y0_path = "Recieved/X0Y1masked.txt";
    std::string x1addX1_path = "Recieved/x1addX1.txt";
    std::string y1addY1_path = "Recieved/y1addY1.txt";
    // linefetcher(std::ref(x0_path), std::ref(x0) , x0row , k);      // swap x0row
    // linefetcher(std::ref(y0_path), std::ref(y0) , y0row , k);      // swap y0row
    // linefetcher(std::ref(X0_path), std::ref(X0) , 0 , k);
    // linefetcher(std::ref(Y0_path), std::ref(Y0) , 0 , k);
    linefetcher(std::ref(X1Y0_path), std::ref(X0Y1masked), 0, 1);

if (X0Y1masked.empty()) {
    // n==1 case: no cross terms
    X0Y1masked.push_back(0);
}
int X0Y1 = X0Y1masked[0];

    linefetcher(std::ref(x1addX1_path), std::ref(x1addX1), 0, k);
    linefetcher(std::ref(y1addY1_path), std::ref(y1addY1), 0, k);
    int share1 = (iproduct(x0, addition(y0, y1addY1))) - (iproduct(Y0, x1addX1)) + (X0Y1);
    // std::cout<<"The share of multiplication for party0 is : "<<share1<<"\n";
    std::vector<int> oneshare0;
    std::string oneshare0_path = "Recieved/oneshare0.txt";
    linefetcher(std::ref(oneshare0_path), std::ref(oneshare0), 0, 1);
    int shareofone0 = oneshare0.front();
    // std::cout<<"The share of 1 for party0  is : "<<shareofone0<<"\n";
    int delta1 = shareofone0 - share1;
    // std::cout<<"The share of delta for party0  is : "<<delta1<<"\n";
    std::string D0_path = "D0.txt";
    std::vector<int> D0_storage;
    D0_storage.push_back(delta1);
    newlinewriter(std::ref(D0_path), std::ref(D0_storage), 1);

    send(client, "done2", 6, 0); // share1 - done

    recv(client, buffer, sizeof(buffer), 0); // wait for client ack

    // ...and so on for steps 3 & 4
    // std::cout << "Server: performing operation 2...\n";
    // task3
    int i = y0row;
    std::vector<int> V0; // vector
    std::vector<int> L0; // vector
    std::vector<int> D0; // int
    std::vector<int> M0; // int
    std::string V0_path = "V0_party0.txt";
    std::string L0_path = "Recieved2/L0.txt";
    // std::string D0_path = "D0.txt";
    std::string M0_path = "Recieved2/M0.txt";
    linefetcher(V0_path, std::ref(V0), i, k); // 0 to be swapped by row index
    linefetcher(L0_path, std::ref(L0), 0, k);
    linefetcher(D0_path, std::ref(D0), 0, 1);
    linefetcher(M0_path, std::ref(M0), 0, 1);

    std::vector<int> v0addl0(k);
    for (int i = 0; i < k; i++)
    {
        v0addl0[i] = V0[i] + L0[i];
    }
    std::vector<int> d0addm0(1);
    d0addm0[0] = D0[0] + M0[0];

    std::string voaddlo_targetpath = "../party1/Recieved2/v0addl0.txt";
    std::string doaddmo_targetpath = "../party1/Recieved2/d0addm0.txt";
    newlinewriter(std::ref(voaddlo_targetpath), std::ref(v0addl0), k);
    newlinewriter(std::ref(doaddmo_targetpath), std::ref(d0addm0), 1); // sender11 hogaya

    send(client, "done2", 6, 0);
    recv(client, buffer, sizeof(buffer), 0); // wait for client ack
    // task4

    int m = std::atoi(argc[3]);
    std::vector<int> U0;
    // std::vector<int> V0;    // vec
    // std::vector<int> D0;    //int
    // std::vector<int> L0;    //vec
    // std::vector<int> M0;    //int
    std::vector<int> L0M1;    // vec
    std::vector<int> v1addl1; // vec
    std::vector<int> d1addm1; // int
    std::string U0_path = "U0_party0.txt";
    // std::string V0_path = "V0_party0.txt";
    // std::string D0_path =  "D0.txt";
    // std::string L0_path =  "Recieved2/L0.txt";
    // std::string M0_path =  "Recieved2/M0.txt";
    std::string L0M1_path = "Recieved2/maskedL0M1.txt";
    std::string v1addl1_path = "Recieved2/v1addl1.txt";
    std::string d1addm1_path = "Recieved2/d1addm1.txt";
    // linefetcher(std::ref(V0_path), std::ref(V0), y0row, k);     // 0 to be swapped
    // linefetcher(std::ref(D0_path), std::ref(D0), 0, 1);
    // linefetcher(std::ref(L0_path), std::ref(L0), 0, k);
    // linefetcher(std::ref(M0_path), std::ref(M0), 0, 1);
    linefetcher(std::ref(L0M1_path), std::ref(L0M1), 0, k);
    linefetcher(std::ref(v1addl1_path), std::ref(v1addl1), 0, k);
    linefetcher(std::ref(d1addm1_path), std::ref(d1addm1), 0, 1);
    linefetcher(std::ref(U0_path), std::ref(U0), x0row, k); // 0 to be swapped

    int D0_int = D0.front();
    int d1addm1_int = d1addm1.front();
    int final_D = D0_int + d1addm1_int;

    for (int &i : V0)
    {
        i = i * final_D;
    }

    int M0_int = M0.front();
    for (int &i : v1addl1)
    {
        i = i * M0_int;
    }

    std::vector<int> temp(k);
    std::vector<int> temp1(k);

    temp = subtraction(std::ref(V0), std::ref(v1addl1));
    temp1 = addition(std::ref(temp), std::ref(L0M1));
    std::vector<int> updatedUi0(k);
    updatedUi0 = addition(temp1, U0);
    for (int i = 0; i < updatedUi0.size(); i++)
    {
       updatedUi0[i] = field(updatedUi0[i]);
    }
    std::string Ui0_path = "../../Updater/newUi0.txt";
    newlinewriter(std::ref(Ui0_path), std::ref(updatedUi0), k);
    linewriter(std::ref(U0_path), std::ref(updatedUi0), m, x0row, k);
    // FIX: Send signal and wait for client ack
    send(client, "done_task4", 11, 0);
    recv(client, buffer, sizeof(buffer), 0); // Wait for "ack_task4"

    // Sampling the DPF

    std::vector<int> seed;
    std::vector<int> seed_cw;
    std::vector<int> flag_cw;
    std::vector<int> final_cw;
    std::vector<int> diff_index;
    std::vector<int> FinalSeedIndexVec;
    std::vector<int> Depth;
    std::vector<int> optimized_size;
    std::vector<int> real_size;

    // paths
    std::string seed_path = "Item/seed.txt";
    std::string seed_cw_path = "Item/seed_cw.txt";
    std::string flag_cw_path = "Item/flag_cw.txt";
    std::string final_cw_path = "Item/finalcw.txt";
    std::string diff_index_path = "Item/diffindex.txt";
    std::string FinalSeedIndexVec_path = "Item/finalseedindexvec.txt";
    std::string Depth_path = "Item/depth.txt";
    std::string optimized_size_path = "Item/optimized_size.txt";
    std::string real_size_path = "Item/real_size.txt";

    // Fetching the data
    linefetcher_new(seed_path, seed);
    linefetcher_new(seed_cw_path, seed_cw);
    linefetcher_new(flag_cw_path, flag_cw);
    linefetcher_new(final_cw_path, final_cw);
    linefetcher_new(diff_index_path, diff_index);
    linefetcher_new(FinalSeedIndexVec_path, FinalSeedIndexVec);
    linefetcher_new(Depth_path, Depth);
    linefetcher_new(optimized_size_path, optimized_size);
    linefetcher_new(real_size_path, real_size);
    int seed1 = seed[0];
    int depth1 = Depth[0];
    int real_size1 = real_size[0];
    int initial_flag = 1;

    // Reading the share of V in a vector for further calculations
    std::vector<std::vector<int>> Share_v0;
    std::string path_to_V0 = "V0_party0.txt";
    matrix_reader(path_to_V0, Share_v0);

    // Sampling the second DPF
    std::vector<int> DPF1_seed;
    std::vector<int> DPF1_seed_cw;
    std::vector<int> DPF1_flag_cw;
    std::vector<int> DPF1_final_cw;
    std::vector<int> DPF1_diff_index;
    std::vector<int> DPF1_FinalSeedIndexVec;

    // paths
    std::string DPF1_seed_path = "Item1/seed.txt";
    std::string DPF1_seed_cw_path = "Item1/seed_cw.txt";
    std::string DPF1_flag_cw_path = "Item1/flag_cw.txt";
    std::string DPF1_final_cw_path = "Item1/finalcw.txt";
    std::string DPF1_diff_index_path = "Item1/diffindex.txt";
    std::string DPF1_FinalSeedIndexVec_path = "Item1/finalseedindexvec.txt";

    // Fetching the data
    linefetcher_new(DPF1_seed_path, DPF1_seed);
    linefetcher_new(DPF1_seed_cw_path, DPF1_seed_cw);
    linefetcher_new(DPF1_flag_cw_path, DPF1_flag_cw);
    linefetcher_new(DPF1_final_cw_path, DPF1_final_cw);
    linefetcher_new(DPF1_diff_index_path, DPF1_diff_index);
    linefetcher_new(DPF1_FinalSeedIndexVec_path, DPF1_FinalSeedIndexVec);
    int DPF1_seed1 = DPF1_seed[0];

    // Expanding the DPF
    std::vector<int> DPF1_dpf_vector = evalFull(DPF1_seed1, initial_flag, DPF1_seed_cw, DPF1_flag_cw, DPF1_final_cw, DPF1_diff_index, DPF1_FinalSeedIndexVec, depth1, real_size1);

    // finding U.Vj = t0
    std::vector<int> t0 = vecT_mat(Share_v0, user_profile_vector);

    // loading the beaver triplets

    // Storage for them
    std::vector<int> a0;
    std::vector<int> b0;
    std::vector<int> c0;

    // paths to access
    std::string a0_path = "last_item/a0.txt";
    std::string b0_path = "last_item/b0.txt";
    std::string c0_path = "last_item/c0.txt";

    // fetching the beaver triplets
    linefetcher_new(a0_path, a0);
    linefetcher_new(b0_path, b0);
    linefetcher_new(c0_path, c0);

    // computing masks
    std::vector<int> d0;
    d0 = subtraction(DPF1_dpf_vector, a0);

    std::vector<int> e0;
    e0 = subtraction(t0, b0);

    // sending d0 and e0

    // setting paths
    std::string d0_send_path = "../party1/last_item/d0.txt";
    std::string e0_send_path = "../party1/last_item/e0.txt";

    // ... (code to calculate d0 and e0) ...

    // sending (writing d0 and e0)
    vectorwriter(d0_send_path, d0);
    vectorwriter(e0_send_path, e0);

    // 1. Tell client "I have written d0 and e0, you can read them."
    send(client, "server_done", 12, 0);

    // 4. Wait for client to signal "I have written d1 and e1."
    recv(client, buffer, sizeof(buffer), 0);
    // std::cout << "Server: Received client_done signal\n";

    // --- NOW it is safe to read d1 and e1 ---
    std::vector<int> d1;
    std::vector<int> e1;
    std::string path_to_d1 = "last_item/d1.txt";
    std::string path_to_e1 = "last_item/e1.txt";
    // finally fetching them
    linefetcher_new(path_to_d1, d1);
    linefetcher_new(path_to_e1, e1);

    // constructing d and e
    std::vector<int> d = addition(d0, d1);
    std::vector<int> e = addition(e0, e1);

    int c0_int = c0[0];

    int s0 = c0_int + iproduct(d, b0) + iproduct(a0, e) + iproduct(d, e);

    std::vector<int> m0;
    for (int i = 0; i < user_profile_vector.size(); i++)
    {
        m0.push_back(user_profile_vector[i] * (1 - s0));
    }
    for (int i = 0; i < k; i++)
    {
        Share_v0[y0row][i] = field(Share_v0[y0row][i] + m0[i]);
    }
    //------------------printing m0
    // std::cout << "M0 : ";
    // for (int i : m0)
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << "\n";

    // creating the vector fcw
    std::vector<int> fcw1_vector;
    for (int i = 0; i < k; i++)
    {
        fcw1_vector.push_back(final_cw[0]);
    }

    // calculating p0
    std::vector<int> p0 = subtraction(m0, fcw1_vector);

    // write p0
    std::string p0_path = "../party1/p0/p0.txt";
    vectorwriter(p0_path, p0);
    int n = std::atoi(argc[4]);

    // tell the client i have written p0
    send(client, "server_done", 12, 0);
    // wait for the client acknowledgement to tell that he have written p1
    recv(client, buffer, sizeof(buffer), 0);

    // CAN CONTINUE

    // reading p1
    std::string p1_path = "p1/p1.txt";
    std::vector<int> p1;
    linefetcher_new(p1_path, p1);

    // finding p0+p1 / fcw_m
    std::vector<int> fcw_m = addition(p0, p1);

    // printing fcw_m to debugg
    // std::cout << "FCW_M : ";
    // for (int i : fcw_m)
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << "\n";

    // Expanding the DPF's
    std::vector<int> dpf_vector = evalFull(seed1, initial_flag, seed_cw, flag_cw, final_cw, diff_index, FinalSeedIndexVec, depth1, real_size1);

    int different_index = diff_index[0];
    dpf_vector[different_index] = Xor(dpf_vector[different_index], final_cw[0]);

    // std::cout << "Evalfull 0 : ";
    // for (int i : dpf_vector)
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << "\n";

    // printing fcw
    // std::cout << "FCW's : ";
    // for (int i : final_cw)
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << "\n";

    // making matrix from dpf_vector

    std::vector<std::vector<int>> dpf_matrix;
    for (int i = 0; i < n; i++)
    {
        dpf_matrix.push_back(dpf_vector);
    }

    // for(int i = 0; i<dpf_matrix.size(); i++)
    // {
    //     for(int j = 0; j<dpf_matrix[0].size(); j++)
    //     {
    //         std::cout<<dpf_matrix[i][j]<<" ";
    //     }
    //     std::cout<<"\n";
    // }
    // std::cout<<"\n";

    // applying the update to share of V0 :

    shareswriter(path_to_V0, Share_v0, Share_v0.size(), Share_v0[0].size());

    close(client);
    close(sockfd);
}
