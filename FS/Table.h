#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "File.h"
#include "SB.h"

using namespace std;

struct Table {
    map<u_int32_t, u_int32_t> FT;
    u_int32_t chunkSize;
    u_int32_t dataOffset;
    static const unsigned int EOC = 0xFFFFFFFF;
    static const unsigned int ERROR = 0xFFFFFFFE;

    Table(FILE* fd, u_int32_t offset, u_int32_t FTSize, SB& sb);
    Table(map<u_int32_t, u_int32_t> FT, u_int32_t chunkSize, u_int32_t dataOffset);
    vector<unsigned char> getFile(vector<unsigned char> data, File fi);
    vector<unsigned char> getFile(FILE* fd, File fi);
    void eraseFrom(int startChunk);
    vector<unsigned char> strip(const vector<unsigned char>& bytes);
    bool IsEmptyLB(const vector<unsigned char>& bytes, u_int32_t blockSize);
    vector<unsigned char> strip(const vector<unsigned char>& bytes, u_int32_t blockSize);
    Table scale(int scale, SB& sb);
    void print();
    vector<unsigned char> getRaw();
};

#endif
