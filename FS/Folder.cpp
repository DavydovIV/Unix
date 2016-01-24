#include "Folder.h"

Folder::Folder(FILE* fd, u_int32_t offset, u_int32_t rootFSize) {
        if (fd == NULL)
            throw string("File not found");

        if (offset != 0)
            fseek(fd, offset, SEEK_SET);

        vector<unsigned char> bytes;
        unsigned char buf[1024];
        int size = fread(buf, sizeof(char), rootFSize * File::getSize(), fd);
        for(int i = 0; i < size; i++)
            bytes.push_back(buf[i]);
        unsigned char* rawData = bytes.data();
        for (int i = 0; i < rootFSize; i++) {
            File fi(rawData + File::getSize() * i);
            if (fi.isExist())
                files.push_back(fi);
        }
        this->rootFSize = rootFSize;
    }

Folder::Folder(unsigned char* rawData, u_int32_t rootFSize) {
        if (rawData == NULL)
            throw string("Null ptr");
        if (rawData[0] != 0)
            for (int i = 0; i < rootFSize; i++) {
                File fi(rawData + File::getSize() * i);
                if (fi.isExist())
                    files.push_back(fi);
            }
        this->rootFSize = rootFSize;
    }

void Folder::print() {
        sort(files.begin(), files.end(), [] (const File & l, const File & r) {
            return l.attributes < r.attributes;
        });
        for (auto& f: files)
            f.print();
    }

vector<unsigned char> Folder::getRaw() {
        unsigned char* raw = new unsigned char [rootFSize];
        memset(raw, 0, rootFSize);
        int rawOffset = 0;
        int fileOffset = 0;
        for (auto& file: files) {
            char name[12];
            memset(name, 0, 12);
            memcpy(name, file.name.c_str(), file.name.size() + 1);
            memcpy(raw + rawOffset, name, 12);
            memcpy(raw + rawOffset + 12, &file.firstBlock, sizeof(u_int32_t));
            memcpy(raw + rawOffset + 12 + sizeof(u_int32_t), &file.attributes, sizeof(u_int32_t));
            rawOffset += File::getSize();
        }
        vector<unsigned char> v(raw, raw + rootFSize);
        delete [] raw;
        return v;
    }