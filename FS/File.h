#ifndef FILE_H
#define FILE_H

#include <sys/types.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <string.h>

using namespace std;

struct File {
    string name;
    u_int32_t firstBlock;
    u_int32_t attributes;
    File(FILE* fd);
    File(unsigned char* rawData);

    bool isExist() {
        return !name.empty();
    }

    void print(string prevPath = "") {
        if (name.size() != 0)
            cout << (attributes == 0 ? "File" : "Folder") << ": " << prevPath + name << " start: " << firstBlock << endl;
    }

    static u_int32_t getSize() {
        return 12 + sizeof(firstBlock) + sizeof(attributes);
    }
};


#endif
