#ifndef HELP_H
#define HELP_H

#include <iostream>
#include <string>
#include <sstream>
#include "SB.h"
#include "Folder.h"
#include "Table.h"

using namespace std;

typedef vector<unsigned char> RAW;

class help {
public:
    static FILE* FRead(string filename);
    static FILE* FWrite(string filename);
    static void printRoot(string filename);
    static void printFat(string filename);
    static void printTree(string filename);
    static void printTree(FILE* fd, Folder & rf, Table & ft, string path);
    static std::vector<std::string> &split(const std::string &s, char del, std::vector<std::string> &elems);
    static std::vector<std::string> split(const std::string &s, char del);
    static Folder FindFolder(FILE* fd, Folder & rf, Table & ft, string path);
    static File findFile(FILE* fd, Folder & rf, Table & ft, string path);
    static RAW getFile(string filename, string name);
    static void Changes(string filename, RAW data);
    static RAW Metadata(Table &newFt, SB &newSb, Folder &newRf);
    static RAW getAllData(FILE* fd, int dataOffset);
    static void removeFile(string filename, string path);
    int static getPosInFolder(const string &path, const Folder &rf);

};


#endif