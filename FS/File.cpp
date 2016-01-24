#include "File.h"

    File::File(FILE* fd) {
        if (fd == NULL)
            throw std::string("File not found");
        char buf[12];
        fread(buf, sizeof(char), 12, fd);
        this->name = string(buf);
        fread(&firstBlock, sizeof(firstBlock), 1, fd);
        fread(&attributes, sizeof(attributes), 1, fd);
    }

    File::File(unsigned char* rawData) {
        if (rawData == NULL)
            throw string("Null ptr");
        int offset = 12;
        if (*rawData != 0) {
            int len = strlen((const char*)rawData);
            this->name = string(rawData, rawData + len);
        }
        memcpy(&this->firstBlock, rawData + offset, sizeof(firstBlock));
        offset += sizeof(firstBlock);
        memcpy(&this->attributes, rawData + offset, sizeof(attributes));
    }