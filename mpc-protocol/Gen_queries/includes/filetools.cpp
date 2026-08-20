#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<deque>
#include<random>

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>


void linefetcher(const std::string &file_path, std::vector<int> &linestorer, int rowindex, int columnindex)
{
    linestorer.clear();  // clear previous content

    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        linestorer.assign(columnindex, 0); // fill with 0s if file cannot be opened
        return;
    }

    std::string line;
    int currentRow = 0;
    bool rowFound = false;

    // Read until the desired row
    while (std::getline(file, line)) {
        if (currentRow == rowindex) {
            rowFound = true;
            break;
        }
        currentRow++;
    }

    file.close();

    if (!rowFound || line.empty()) {
        // Row missing or empty → fill with 0s
        linestorer.assign(columnindex, 0);
        return;
    }

    // Special case: if columnindex == 1, just store the first integer
    if (columnindex == 1) {
        size_t pos = 0;
        linestorer.push_back(std::stoi(line, &pos));
        return;
    }

    // General case: multiple elements
    std::stringstream ss(line);
    int value;
    while (ss >> value) {
        linestorer.push_back(value);
    }

    // Fill remaining elements with 0 if line had fewer integers than columnindex
    while (linestorer.size() < static_cast<size_t>(columnindex)) {
        linestorer.push_back(0);
    }
}

void linewriter(const std::string &file_path, std::vector<int> &sample,int no_of_rows, int rownumber, int columnnumber)
{
    // sample = std::vector<int>(columnnumber);
    std::vector<std::string> tempo(no_of_rows);
    std::ifstream opener;
    opener.open(file_path);
    std::string s;
    int i = 0;
    // std::cout<<"Line by line file check kr ra hu if shi lines jaa rhi h\n";    //DEBUGGG
    while(getline(opener,s))
    {
        // std::cout<<s<<"\n";                                                    //DEBUGGG
        tempo[i] = s;
        i++;
    }
    std::string to_be_written;
    int index;
    for(int i = 0; i<columnnumber-1 ; i++)
    {
        std::string te = std::to_string(sample[i]);
        // std::cout<<"Check if sample array se shi values utha  rha h\n";        //DEBUGGG
        // std::cout<<sample[i]<<"\n";                                            //DEBUGGG
        // std::cout<<"Check if temporary string shi values utha  rha h\n";       //DEBUGGG
        // std::cout<<te<<"\n";                                                   //DEBUGGG 
        to_be_written +=te;
        to_be_written.push_back(' ');
        index = i;
    }
    index++;
    std::string te = std::to_string(sample[index]);
    to_be_written +=te;
    // std::cout<<"Dekhte h if temporary string shi bni h\n";                      //DEBUGGG
    // std::cout<<to_be_written<<"\n";                                                //DEBUGGG
    
    tempo[rownumber] = to_be_written;
    opener.close();
    std::ofstream writer;
    writer.open(file_path);
    for(int i = 0; i<no_of_rows ; i++)
    {
        writer<<tempo[i]<<std::endl;
    }
    writer.close();
}

void shareswriter(const std::string& filePath,std::vector<std::vector<int>> &source,int row,int column)
{
    std::ofstream file_access;
    file_access.open(filePath, std::ios::trunc);
    for(int i = 0; i<row ; i++)
    {
        int temp=0;
        while(temp<column-1)
        {
            file_access<<source[i][temp]<<" ";
            temp++;
        }
        file_access<<source[i][temp];
        file_access<<std::endl;
    }
    file_access.close();
}

int getRandomInt(int minVal, int maxVal) {
    static std::random_device rd;   // Non-deterministic seed
    static std::mt19937 gen(rd());  // Mersenne Twister engine
    std::uniform_int_distribution<> dist(minVal, maxVal);
    return dist(gen);
}


void newlinewriter(const std::string& filePath, std::vector<int>& source, int k)
{
    std::ofstream opener;
    opener.open(filePath, std::ios::app);

    if (opener.is_open())
    {
        for (int i = 0; i < k; i++)
        {
            opener << source[i];
            if (i < k - 1)
            {
                opener << " ";
            }
        }
        opener << "\n";
        opener.close();
    }
}


void copyFile(const std::string& dominantFilePath, const std::string& victimFilePath) {
    std::ifstream dominantFile(dominantFilePath, std::ios::binary);
    std::ofstream victimFile(victimFilePath, std::ios::binary);

    if (!dominantFile.is_open()) {
        std::cerr << "Error: Could not open dominant file " << dominantFilePath << std::endl;
        return;
    }

    if (!victimFile.is_open()) {
        std::cerr << "Error: Could not open victim file " << victimFilePath << std::endl;
        return;
    }

    victimFile << dominantFile.rdbuf();

    if (!dominantFile.eof() && dominantFile.fail()) {
        std::cerr << "Error: Reading from dominant file failed." << std::endl;
    }

    if (victimFile.fail()) {
        std::cerr << "Error: Writing to victim file failed." << std::endl;
    }

    dominantFile.close();
    victimFile.close();
}

void vectorwriter(const std::string& filePath, const std::vector<int>& source)
{
    // open in truncate mode -> clears the file
    std::ofstream opener(filePath, std::ios::trunc);

    if (opener.is_open())
    {
        for (size_t i = 0; i < source.size(); i++)
        {
            opener << source[i];
            if (i < source.size() - 1)
            {
                opener << " ";
            }
        }
    }
}
