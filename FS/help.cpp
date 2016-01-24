#include "help.h"

    FILE* help::FRead(string filename) {
        FILE* fd = fopen(filename.c_str(), "r");
        if (fd == NULL)
            throw string("File not found");
        return fd;
    }

    FILE* help::FWrite(string filename) {
        FILE* fd = fopen(filename.c_str(), "w");
        if (fd == NULL)
            throw string("Can't open file for write");
        return fd;
    }

    void help::printRoot(string filename) {
        FILE* fd = FRead(filename);
        SB superBlock(fd);
        Folder rf(fd, superBlock.getSize() + superBlock.fatSize, superBlock.rootFSize);
        rf.print();
    }

    void help::printFat(string filename) {
        FILE* fd = FRead(filename);
        SB sb(fd);
        Table ft(fd, sb.getSize(), sb.fatSize, sb);
        ft.print();
    }
    void help::printTree(string filename) {
        FILE * fd = FRead(filename);
        SB sb(fd);
        Table ft(fd, 0, sb.fatSize, sb);
        Folder rf(fd, 0, sb.rootFSize);
        printTree(fd, rf, ft, "/");
    }

    void help::printTree(FILE* fd, Folder & rf, Table & ft, string path) {
        for(auto fi : rf.files) {
            fi.print(path);
            if(fi.attributes == 1) {
                RAW folder = ft.getFile(fd, fi);
                Folder newRf(folder.data(), folder.size() / File::getSize());
                printTree(fd, newRf, ft, path + fi.name + "/");
            }
        }
    }

    std::vector<std::string> &help::split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }


    std::vector<std::string> help::split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, elems);
        vector<string> filtered;
        for (auto s : elems)
            if (s.size() > 0)
                filtered.push_back(s);
        return filtered;
    }

    Folder help::FindFolder(FILE* fd, Folder & rf, Table & ft, string path) {
        vector<string> splittedPath = split(path, '/');
        string first = splittedPath.front();
        string endPath;
        for (int i = 1; i < splittedPath.size(); i++) {
            endPath += "/" + splittedPath[i];
        }
        for(auto fi : rf.files) {
            while (fi.name.back() == 0) fi.name.pop_back();
            if (fi.name == first) {
                if (fi.attributes == 0 && splittedPath.size() == 1) {
                    return rf;
                }
                else {
                    if (fi.attributes == 1) {
                        RAW folder = ft.getFile(fd, fi);
                        Folder newRf(folder.data(), folder.size() / File::getSize());
                        newRf.start = fi.firstBlock;
                        return FindFolder(fd, newRf, ft, endPath);
                    }
                }
            }
        }
        throw string("File not found");
    }
    File help::findFile(FILE* fd, Folder & rf, Table & ft, string path) {
        vector<string> splittedPath = split(path, '/');
        string first = splittedPath.front();
        string endPath;
        for (int i = 1; i < splittedPath.size(); i++) {
            endPath += "/" + splittedPath[i];
        }
        for(auto fi : rf.files) {
            while (fi.name.back() == 0) fi.name.pop_back();
            if (fi.name == first) {
                RAW folder = ft.getFile(fd, fi);
                Folder newRf(folder.data(), folder.size() / File::getSize());
                if (fi.attributes == 0 && splittedPath.size() == 1)
                    return fi;
                else
                    return findFile(fd, newRf, ft, endPath);
            }
        }
        throw string("File not found");
    }

    RAW help::getFile(string filename, string name) {
        FILE * fd = FRead(filename);
        SB sb(fd);
        Table ft(fd, 0, sb.fatSize, sb);
        Folder rf(fd, 0, sb.rootFSize);
        File fi = findFile(fd, rf, ft, name);
        return ft.getFile(fd, fi);
    }


    void help::Changes(string filename, RAW data) {
        FILE* fd = FWrite(filename);
        fwrite(data.data(), sizeof(unsigned char) * data.size(), 1, fd);
        fclose(fd);
    }

    RAW help::Metadata(Table &newFt, SB &newSb, Folder &newRf) {
        RAW fat = newFt.getRaw();
        newSb.fatSize = fat.size();
        RAW metaData = newSb.getRaw();
        newRf.rootFSize *= File::getSize();
        RAW rootFolder = newRf.getRaw();
        cout << metaData.size() << " " << fat.size() << " " << rootFolder.size() << endl;
        metaData.insert(metaData.end(), fat.begin(), fat.end());
        metaData.insert(metaData.end(), rootFolder.begin(), rootFolder.end());
        return metaData;
    }

    RAW help::getAllData(FILE* fd, int dataOffset) {
        RAW data;
        fseek(fd, 0, SEEK_END);
        long fsize = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        unsigned char *string = (unsigned char *)malloc(fsize + 1);
        fread(string, fsize, 1, fd);
        for (int i = 0; i < fsize; i++)
            data.push_back(string[i]);
        for (int i = 0; i < dataOffset; i++)
            data.erase(data.begin());
        return data;
    }

    void help::removeFile(string filename, string path) {
        FILE * fd = FRead(filename);
        SB sb(fd);
        Table ft(fd, 0, sb.fatSize, sb);
        Folder rf(fd, 0, sb.rootFSize);
        RAW allData = getAllData(fd, ft.dataOffset);
        int posInRoot = getPosInFolder(path, rf);
        if (posInRoot >= 0) {
            rf.files.erase(rf.files.begin() + posInRoot);
        } else {
            Folder folder = FindFolder(fd, rf, ft, path);
            vector<string> name = split(path, '/');
            int posInFolder = getPosInFolder(name.back(), folder);
            int startChunk = folder.files[posInFolder].firstBlock;
            folder.files.erase(folder.files.begin() + posInFolder);

            ft.eraseFrom(startChunk);
            folder.rootFSize = sb.blockSize;
            RAW folderData = folder.getRaw();
            for (int i = 0; i < folderData.size(); i++) {
                allData[folder.start * sb.blockSize + i] = folderData[i];
            }
        }
        fclose(fd);
        RAW metadata = Metadata(ft, sb, rf);
        metadata.insert(metadata.end(), allData.begin(), allData.end());
        Changes(filename, metadata);
    }

    int help::getPosInFolder(const string &path, const Folder &rf) {
        int posInRoot = -1;
        for (int i = 0; i < rf.files.size(); i++) {
            if (rf.files[i].name == path || "/" + rf.files[i].name == path) {
                posInRoot = i;
            }
        }
        return posInRoot;
    }
