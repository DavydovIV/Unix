#ifndef LAB1_CONFIGREADER_H
#define LAB1_CONFIGREADER_H

#include <string>
#include <fstream>

class ConfigReader {
private:
    std::string inputPath;
    unsigned int time;

public:
    static std::string defaultFilePath;
    ConfigReader() {};
    void read(const std::string& path = defaultFilePath) {
        std::ifstream file(path);
        if (file.is_open()) {
            std::getline(file, inputPath);
            file >> time;
        } else {
            std::string msg = "Can't read config file " + path;
            throw msg.c_str();
        }
    }

    const std::string &getInputPath() {
        return inputPath;
    }

    unsigned int getTime() {
        return time;
    }
};



#endif //LAB1_CONFIGREADER_H
