#ifndef FOLDER_H
#define FOLDER_H

#include <stdio.h>
#include <sys/types.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "File.h"

using namespace std;

struct Folder {
    vector<File> files;
    u_int32_t rootFSize;
    u_int32_t start;
    Folder(FILE* fd, u_int32_t offset, u_int32_t rootFSize);
    Folder(unsigned char* rawData, u_int32_t rootFSize);
    void print();
    vector<unsigned char> getRaw();
};


#endif
