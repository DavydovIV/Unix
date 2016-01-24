#ifndef SB_H
#define SB_H

#include <sys/types.h>
#include <string>
#include <iostream>
#include <vector>
#include <string.h>

using namespace std;

struct SB {
    u_int32_t blockSize;
    u_int32_t fatSize;
    u_int32_t rootFSize;

    SB(string filename) : SB(fopen(filename.c_str(), "r")) {}
    SB(FILE * fd);
    SB(u_int32_t blockSize, u_int32_t fatSize, u_int32_t rootFSize);
    void print();
    vector<unsigned char> getRaw();

    static u_int32_t getSize() {
        return sizeof(blockSize) + sizeof(fatSize) + sizeof(rootFSize);
    }
};


#endif
