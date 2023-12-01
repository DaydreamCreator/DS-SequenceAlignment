#include "fasta_reader.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <functional>
#define sequence vector<string>
using namespace std;

//  TODO: malloc ???

//  TODO: change the return table as string vector

//  TODO: separate if too long  

//Q:没有做database还是pattern的标识符

void readFASTAfile(std::function<void(const std::string&, const std::string&)> callback, const std::string& filename= "sequences/GCA_009858895.3_ASM985889v3_genomic.fna") {
    std::ifstream input(filename);
    std::string id, line, res_sequence;

    if (!input.good()) {
        std::cerr << "Error opening " << filename << ". Please check the filename." << std::endl;
        return;
    }

    while (getline(input, line)) {
        if (line.empty()) continue;
        if (line[0] == '>') {
            if (!id.empty()) {
                callback(id, res_sequence);
            }
            id = line.substr(1);
            res_sequence.clear();
        } else {
            res_sequence += line;
        }
    }

    if (!id.empty()) {
        callback(id, res_sequence);
    }
}

int main() {
    readFASTAfile([](const std::string& id, const std::string& sequence) {
        std::cout << id << " : " << sequence << std::endl;
    });
    return 0;
}
