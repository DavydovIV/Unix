#include <string>

#include <vector>
#include <map>
#include <sys/syslog.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ConfigReader.h"
typedef std::map<std::string, timespec> DirStatus;

class Daemon {
    bool isRunning = false;
    bool needReload = false;
    ConfigReader configReader = ConfigReader();
    DirStatus allFilesWithTimestamp;

public:
    Daemon() {};

    void start() {
        if (!(isRunning = init())) {
            syslog(LOG_ERR, "Initialization error!");
            isRunning = false;
        }
        process();
    };

    void die() {
        isRunning = false;
    }

    void reloadConfig() {
        needReload = true;
    }



private:

    bool init();

    void reload() {
        configReader = ConfigReader();
    };

    void recursiveFileSearch(DirStatus & acc, std::string dirName);

    DirStatus fileSearch(std::string dirName);

    void compare(DirStatus& before, DirStatus& afte);

    void process();
};
