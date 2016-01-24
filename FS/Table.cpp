#include "Table.h"

    Table::Table(FILE* fd, u_int32_t offset, u_int32_t FTSize, SB& sb) {
        if (fd == NULL)
            throw string("File not found");
        if (offset != 0)
            fseek(fd, offset, SEEK_SET);
        for (int  i = 0; i < FTSize / sizeof(u_int32_t); i += 2) {
            u_int32_t first;
            u_int32_t second;
            fread(&first, sizeof(u_int32_t), 1, fd);
            fread(&second, sizeof(u_int32_t), 1, fd);
            FT[first] = second;
        }
        this->chunkSize = sb.blockSize;
        this->dataOffset = sb.getSize()
                           + sb.fatSize
                           + sb.rootFSize * File::getSize();
    }

    Table::Table(map<u_int32_t, u_int32_t> fileTable, u_int32_t chunkSize, u_int32_t dataOffset) {
        this->FT = fileTable;
        this->chunkSize = chunkSize;
        this->dataOffset = dataOffset;
    }

    vector<unsigned char> Table::getFile(vector<unsigned char> data, File fi) {
        int currentChunk = fi.firstBlock;
        vector<unsigned char> bytes;
        while(currentChunk != EOC) {
            int start = currentChunk * chunkSize;
            unsigned char c;
            for (int i = 0; i < chunkSize; i++) {
                bytes.push_back(data[start + i]);
            }
            currentChunk = FT[currentChunk];
        }
        return bytes;
    }

    void Table::eraseFrom(int startChunk) {
        int currentChunk = startChunk;
        while(currentChunk != EOC) {
            int removeFrom = currentChunk;
            currentChunk = FT[currentChunk];
            FT[removeFrom] = 0;
        }
        FT[currentChunk] = 0;
    }

    vector<unsigned char> Table::getFile(FILE* fd, File fi) {
        if (fd == NULL)
            throw string("File not found");
        if (fi.firstBlock == 0)
            throw string("Empty block!");
        int currentChunk = fi.firstBlock;
        vector<unsigned char> bytes;
        while(currentChunk != EOC) {
            fseek(fd, dataOffset + currentChunk * chunkSize, SEEK_SET);
            unsigned char c;
            for (int i = 0; i < chunkSize; i++) {
                fread(&c, sizeof(c), 1, fd);
                bytes.push_back(c);
            }
            currentChunk = FT[currentChunk];
        }
        if (fi.attributes == 0) {
            bytes = strip(bytes);
        } else {
            bytes = strip(bytes, File::getSize());
        }
        return bytes;
    }



    vector<unsigned char> Table::strip(const vector<unsigned char>& bytes) {
        vector<unsigned char> stripped = bytes;
        while(stripped.back() == 0)
            stripped.pop_back();
        return stripped;
    }

    bool Table::IsEmptyLB(const vector<unsigned char>& bytes, u_int32_t blockSize) {
        for(int i = bytes.size() - 1; i >= bytes.size() - blockSize; i--)
            if(bytes[i] != 0)
                return false;
        return true;
    }

    vector<unsigned char> Table::strip(const vector<unsigned char>& bytes, u_int32_t blockSize) {
        vector<unsigned char> stripped = bytes;
        bool Del = true;
        while(Del) {
            if (IsEmptyLB(stripped , blockSize))
                for(int i = 0; i < blockSize; i++)
                    stripped.pop_back();
            else
                Del = false;
        }
        while(stripped.size() % blockSize != 0)
            stripped.pop_back();
        return stripped;
    }

    Table Table::scale(int scale, SB& sb) {
        map<u_int32_t, u_int32_t> NewTable;
        for (auto& p : FT) {
            for (int i = 0; i < scale - 1; i++) {
                NewTable[p.first * scale + i] = p.first * scale + i + 1;
            }
            if (FT[p.first] != EOC && FT[p.first] != ERROR)
                NewTable[p.first * scale + scale - 1] = FT[p.first] * scale;
            else
                NewTable[p.first * scale + scale - 1] = FT[p.first];

        }
        u_int32_t newDataOffset = SB::getSize() +
            sb.fatSize * scale +
            sb.rootFSize * File::getSize();
        return Table(NewTable, this->chunkSize / scale, newDataOffset);
    }

    void Table::print() {
        cout << "File Allocation Table" << endl;
        cout << "ChunkSize = " << chunkSize << endl;
        cout << "DataOffSet = " << dataOffset << endl;
        for (auto& p : FT) {
            string state;
            if (p.second == EOC)
                state = "EOC";
            else if (p.second == ERROR)
                state = "ERROR";
            if (state.size() == 0)
                cout << p.first << "\t" << p.second << endl;
            else
                cout << p.first << "\t" << state << endl;
        }
        cout << endl;
    }

    vector<unsigned char> Table::getRaw() {
        int size = FT.size() * 2 * sizeof(u_int32_t);
        unsigned char* raw = new unsigned char [size];
        memset(raw, 0, size);
        int offset = 0;
        for (auto& p : FT) {
            memcpy(raw + offset, &p.first, sizeof(p.first));
            memcpy(raw + offset + sizeof(p.first), &p.second, sizeof(p.second));
            offset += sizeof(p.first) + sizeof(p.second);
        }
        vector<unsigned char> v(raw, raw + size);
        delete [] raw;
        return v;
    }