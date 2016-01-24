#include "SB.h"

    SB::SB (FILE * fd) {
        if (fd == NULL)
            throw string("File not found, please do smth with your arms");
        fread(&blockSize,      sizeof(blockSize),      1, fd);
        fread(&fatSize,        sizeof(fatSize),        1, fd);
        fread(&rootFSize, sizeof(rootFSize), 1, fd);
    }

    SB::SB(u_int32_t blockSize, u_int32_t fatSize, u_int32_t rootFolderSize) {
        this->blockSize = blockSize;
        this->fatSize = fatSize;
        this->rootFSize = rootFolderSize;
    }
    
    void SB::print() {
        cout << "Block size: \t" << blockSize << endl
        << "Fat table size: \t" << fatSize << endl
        << "Root folder size: \t" << rootFSize << endl;
    }

    vector<unsigned char> SB::getRaw() {
        int size = sizeof(u_int32_t) * 3;
        unsigned char* raw = new unsigned char [size];
        memset(raw, 0, size);
        memcpy(raw, &blockSize, sizeof(blockSize));
        memcpy(raw + sizeof(fatSize), &fatSize, sizeof(u_int32_t));
        memcpy(raw + 2 * sizeof(rootFSize), &rootFSize, sizeof(u_int32_t));
        vector<unsigned char> v(raw, raw + size);
        delete [] raw;
        return v;
    }