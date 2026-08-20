// client.cpp
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

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    // Keep retrying until server available
    while (connect(sockfd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        usleep(500000);
    }

    char buffer[16];

    // --- Step 1 ---
    recv(sockfd, buffer, sizeof(buffer), 0); // wait for server done
                                             // std::cout << "Client: got signal for step1\n";
                                             // perform task 1
    int x1row = std::atoi(argc[1]);          // 0 to be replaced by commandline row
    int y1row = std::atoi(argc[2]);          // same
    std::vector<int> k_value;
    std::string s = "k.txt";
    linefetcher(std::ref(s), std::ref(k_value), 0, 1);
    int k = k_value[0];
    std::vector<int> x1;
    std::vector<int> y1;
    std::vector<int> X1;
    std::vector<int> Y1;
    std::string x1_path = "U1_party1.txt";
    std::string y1_path = "V1_party1.txt";
    std::string X1_path = "Recieved/X1.txt";
    std::string Y1_path = "Recieved/Y1.txt";
    linefetcher(std::ref(x1_path), std::ref(x1), x1row, k); // 0 to be replaced
    linefetcher(std::ref(y1_path), std::ref(y1), y1row, k); // 0 to be replaced
    linefetcher(std::ref(X1_path), std::ref(X1), 0, k);
    linefetcher(std::ref(Y1_path), std::ref(Y1), 0, k);
    std::vector<int> x1addX1;
    x1addX1 = addition(std::ref(x1), std::ref(X1));
    std::vector<int> y1addY1;
    y1addY1 = addition(std::ref(y1), std::ref(Y1));
    std::string writemaskedx1 = "../party0/Recieved/x1addX1.txt";
    std::string writemaskedy1 = "../party0/Recieved/y1addY1.txt";
    newlinewriter(std::ref(writemaskedx1), std::ref(x1addX1), k);
    newlinewriter(std::ref(writemaskedy1), std::ref(y1addY1), k);

    // For dpf
    std::vector<int> user_profile_vector;
    std::string path_to_user_profile = "../../globalfiles/U.txt";
    linefetcher(std::ref(path_to_user_profile), std::ref(user_profile_vector), 0, k);

    send(sockfd, "ack1", 5, 0); // notify server - Sender2 done

    // --- Step 2 ---
    recv(sockfd, buffer, sizeof(buffer), 0);
    // std::cout << "Client: got signal for step2\n";
    // std::cout<<"PArt3"<<std::endl;
    // std::vector<int> x1;
    // std::vector<int> y1;
    // std::vector<int> X1;
    // std::vector<int> Y1;
    std::vector<int> X1Y0masked;
    std::vector<int> x0addX0;
    std::vector<int> y0addY0;

    // std::string x1_path = "U1_party1.txt";
    // std::string y1_path = "V1_party1.txt";
    // std::string X1_path = "Recieved/X1.txt";
    // std::string Y1_path = "Recieved/Y1.txt";
    std::string X0Y1_path = "Recieved/X1Y0masked.txt";
    std::string x0addX0_path = "Recieved/x0addX0.txt";
    std::string y0addY0_path = "Recieved/y0addY0.txt";
    // linefetcher(std::ref(x1_path), std::ref(x1) , x1row , k);      // to be swapped
    // linefetcher(std::ref(y1_path), std::ref(y1) , y1row , k);     // to be swapped
    // linefetcher(std::ref(X1_path), std::ref(X1) , 0 , k);
    // linefetcher(std::ref(Y1_path), std::ref(Y1) , 0 , k);
    linefetcher(std::ref(X0Y1_path), std::ref(X1Y0masked), 0, 1);

if (X1Y0masked.empty()) {
    X1Y0masked.push_back(0);
}
int X1Y0 = X1Y0masked[0];

    linefetcher(std::ref(x0addX0_path), std::ref(x0addX0), 0, k);
    linefetcher(std::ref(y0addY0_path), std::ref(y0addY0), 0, k);
    int share2 = (iproduct(x1, addition(y1, y0addY0))) - (iproduct(Y1, x0addX0)) + (X1Y0);
    //  std::cout<<"The share of multiplication for party1 is : "<<share2<<"\n";
    std::vector<int> oneshare1;
    std::string oneshare1_path = "Recieved/oneshare1.txt";
    linefetcher(std::ref(oneshare1_path), std::ref(oneshare1), 0, 1);
    int shareofone1 = oneshare1.front();
    // std::cout<<"The share of 1 for party0  is : "<<shareofone1<<"\n";
    int delta2 = shareofone1 - share2;
    // std::cout<<"The share of delta for party0  is : "<<delta2<<"\n";
    std::string D1_path = "D1.txt";
    std::vector<int> D1_storage;
    D1_storage.push_back(delta2);
    newlinewriter(std::ref(D1_path), std::ref(D1_storage), 1);

    send(sockfd, "ack2", 5, 0); // Share2 done
    recv(sockfd, buffer, sizeof(buffer), 0);

    // Next task kro
    int i = y1row;
    std::vector<int> V1; // vector
    std::vector<int> L1; // vector
    std::vector<int> D1; // int
    std::vector<int> M1; // int
    std::string V1_path = "V1_party1.txt";
    std::string L1_path = "Recieved2/L1.txt";
    // std::string D1_path = "D1.txt";
    std::string M1_path = "Recieved2/M1.txt";
    linefetcher(V1_path, std::ref(V1), i, k); // 0 to be swapped
    linefetcher(L1_path, std::ref(L1), 0, k);
    linefetcher(D1_path, std::ref(D1), 0, 1);
    linefetcher(M1_path, std::ref(M1), 0, 1);

    std::vector<int> v1addl1(k);
    for (int i = 0; i < k; i++)
    {
        v1addl1[i] = V1[i] + L1[i];
    }
    std::vector<int> d1addm1(1);
    d1addm1[0] = D1[0] + M1[0];

    std::string v1addl1_targetpath = "../party0/Recieved2/v1addl1.txt";
    std::string d1addm1_targetpath = "../party0/Recieved2/d1addm1.txt";
    newlinewriter(std::ref(v1addl1_targetpath), std::ref(v1addl1), k);
    newlinewriter(std::ref(d1addm1_targetpath), std::ref(d1addm1), 1);

    send(sockfd, "ack2", 5, 0);
    recv(sockfd, buffer, sizeof(buffer), 0);

    // next
    //  int x0row = std::atoi(argc[1]);            // Maybe initialize them from commandline and pass on later in functions
    //  int y0row = std::atoi(argc[2]);
    int m = std::atoi(argc[3]);
    std::vector<int> U1;
    // std::vector<int> V1;    // vec        // commandline arguements
    // std::vector<int> D1;    //int
    // std::vector<int> L1;    //vec
    // std::vector<int> M1;    //int
    std::vector<int> L1M0;    // vec
    std::vector<int> v0addl0; // vec
    std::vector<int> d0addm0; // int
    std::string U1_path = "U1_party1.txt";
    // std::string V1_path = "V1_party1.txt";
    // std::string D1_path =  "D1.txt";
    // std::string L1_path =  "Recieved2/L1.txt";
    // std::string M1_path =  "Recieved2/M1.txt";
    std::string L1M0_path = "Recieved2/maskedL1M0.txt";
    std::string v0addl0_path = "Recieved2/v0addl0.txt";
    std::string d0addm0_path = "Recieved2/d0addm0.txt";
    // linefetcher(std::ref(V1_path), std::ref(V1), y0row, k);       // 0 to be swapped
    // linefetcher(std::ref(D1_path), std::ref(D1), 0, 1);
    // linefetcher(std::ref(L1_path), std::ref(L1), 0, k);
    // linefetcher(std::ref(M1_path), std::ref(M1), 0, 1);
    linefetcher(std::ref(L1M0_path), std::ref(L1M0), 0, k);
    linefetcher(std::ref(v0addl0_path), std::ref(v0addl0), 0, k);
    linefetcher(std::ref(d0addm0_path), std::ref(d0addm0), 0, 1);
    linefetcher(std::ref(U1_path), std::ref(U1), x1row, k); // 0 to be swapped

    int D1_int = D1.front();
    int d0addm0_int = d0addm0.front();
    int final_D = D1_int + d0addm0_int;

    for (int &i : V1)
    {
        i = i * final_D;
    }

    int M1_int = M1.front();
    for (int &i : v0addl0)
    {
        i = i * M1_int;
    }

    std::vector<int> temp(k);
    std::vector<int> temp1(k);

    temp = subtraction(std::ref(V1), std::ref(v0addl0));
    temp1 = addition(std::ref(temp), std::ref(L1M0));

    std::vector<int> updatedUi1(k);
    updatedUi1 = addition(temp1, U1);
    for(int i = 0; i<updatedUi1.size(); i++)
    {
        updatedUi1[i] = field(updatedUi1[i]);
    }
    std::string Ui1_path = "../../Updater/newUi1.txt";
    newlinewriter(std::ref(Ui1_path), std::ref(updatedUi1), k);
    linewriter(std::ref(U1_path), std::ref(updatedUi1), m, x1row, k);

    // FIX: Send acknowledgement
    send(sockfd, "ack_task4", 10, 0);

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
    int initial_flag = 0;

    // Reading the share of V in a vector for further calculations
    std::vector<std::vector<int>> Share_v1;
    std::string path_to_V1 = "V1_party1.txt";
    matrix_reader(path_to_V1, Share_v1);

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

    // Negating for additive conversion
    for (int i = 0; i < DPF1_dpf_vector.size(); i++)
    {
        DPF1_dpf_vector[i] = DPF1_dpf_vector[i] * -1;
    }

    // finding U.Vj = t1
    std::vector<int> t1 = vecT_mat(Share_v1, user_profile_vector);

    // loading the beaver triplets

    // Storage for them
    std::vector<int> a1;
    std::vector<int> b1;
    std::vector<int> c1;

    // paths to access
    std::string a1_path = "last_item/a1.txt";
    std::string b1_path = "last_item/b1.txt";
    std::string c1_path = "last_item/c1.txt";

    // fetching the beaver triplets
    linefetcher_new(a1_path, a1);
    linefetcher_new(b1_path, b1);
    linefetcher_new(c1_path, c1);

    // computing masks
    std::vector<int> d1;
    d1 = subtraction(DPF1_dpf_vector, a1);

    // ... (code to calculate d1 and e1) ...

    std::vector<int> e1;
    e1 = subtraction(t1, b1);

    // 2. Wait for server to signal "I have written d0 and e0."
    recv(sockfd, buffer, sizeof(buffer), 0);
    // std::cout << "Client: Received server_done signal\n";

    // --- NOW it is safe to write d1 and e1 ---
    std::string d1_send_path = "../party0/last_item/d1.txt";
    std::string e1_send_path = "../party0/last_item/e1.txt";
    vectorwriter(d1_send_path, d1);
    vectorwriter(e1_send_path, e1);

    // 3. Tell server "I have written d1 and e1, you can read them."
    send(sockfd, "client_done", 12, 0);

    // --- NOW it is safe to read d0 and e0 ---
    std::vector<int> d0;
    std::vector<int> e0;
    std::string path_to_d0 = "last_item/d0.txt";
    std::string path_to_e0 = "last_item/e0.txt";

    // finally fetching them
    linefetcher_new(path_to_d0, d0);
    linefetcher_new(path_to_e0, e0);

    // constructing d and e
    std::vector<int> d = addition(d0, d1);
    std::vector<int> e = addition(e0, e1);

    int c1_int = c1[0];

    int s1 = c1_int + iproduct(d, b1) + iproduct(a1, e);

    std::vector<int> m1;
    for (int i = 0; i < user_profile_vector.size(); i++)
    {
        m1.push_back(user_profile_vector[i] * (-1 * s1));
    }
    for (int i = 0; i < k; i++)
    {
        Share_v1[y1row][i] = field(Share_v1[y1row][i] + m1[i]);
    }
    //------------------printing m1
    // std::cout << "M1 : ";
    // for (int i : m1)
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

    // calculating p1
    std::vector<int> p1 = subtraction(m1, fcw1_vector);

    // wait for the server0 to write p0
    recv(sockfd, buffer, sizeof(buffer), 0);

    // write p1
    std::string p1_path = "../party0/p1/p1.txt";
    vectorwriter(p1_path, p1);
    int n = std::atoi(argc[4]);

    // tell server that i have written p1
    send(sockfd, "client_done", 12, 0);

    // can continue
    // reading p0
    std::string p0_path = "p0/p0.txt";
    std::vector<int> p0;
    linefetcher_new(p0_path, p0);

    // finding p0+p1 / fcw_m
    std::vector<int> fcw_m = addition(p0, p1);

    // printing fcw_m to debugg
    // std::cout << "FCW_M : ";
    // for (int i : fcw_m)
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << "\n";

    // Expanding the DPF
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

    //applying the update
    shareswriter(path_to_V1, Share_v1, Share_v1.size(), Share_v1[0].size());

    close(sockfd);
}
