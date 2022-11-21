#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include <fstream>
#include <iostream>

typedef struct {uint32_t value; bool exists;} Optional;

std::vector<char> readFile(std::string filename);

std::vector<char> readFile(std::string filePath) {
    // open file, throw error if fails
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if(!file.is_open()) {
        std::cout << "Failed to open file: " << filePath << std::endl;
        exit(-1);
    }

    // create vector to hold data (seeks to end of file stream to get size of file)
    uint32_t fileSize = (uint32_t)file.tellg();
    std::vector<char> buffer(fileSize);

    // go to beginning of file and read data
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

#endif