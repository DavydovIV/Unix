#include "Daemon.h"


bool Daemon::init() {
        try {
            configReader.read();
        } catch (const char* msg) {
            this->isRunning = false;
            syslog(LOG_ERR, msg);
            return false;
        }
        allFilesWithTimestamp = fileSearch(configReader.getInputPath());
        return true;
    }

void Daemon::recursiveFileSearch(DirStatus & acc, std::string dirName) {
        DIR *dir;
        struct dirent* entry;
        dir = opendir(dirName.c_str());
        if (dir == NULL) {
            syslog(LOG_ERR, ("No directory for read! " + dirName).c_str());
            isRunning = false;
            return;
        }
        while ((entry = readdir(dir)) != NULL) {
            if (std::string(".") == entry->d_name || std::string("..") == entry->d_name)
                continue;

            auto fileName = dirName + "/" + entry->d_name;
            struct stat sb;
            stat(fileName.c_str(), &sb);
            acc[fileName] = sb.st_ctim;

            if (entry->d_type == DT_DIR)
                recursiveFileSearch(acc, fileName);

        }
        closedir(dir);
    }

DirStatus Daemon::fileSearch(std::string dirName) {
        auto files = DirStatus();
        recursiveFileSearch(files, configReader.getInputPath());
        return files;
    }

void Daemon::compare(DirStatus& before, DirStatus& after) {
        for(auto pair : before) {
            if (after.count(pair.first) == 0) {
                syslog(LOG_INFO, (pair.first + " was removed").c_str());
            } else if (after[pair.first].tv_sec != pair.second.tv_sec
                       || after[pair.first].tv_nsec != pair.second.tv_nsec) {
                syslog(LOG_INFO, (pair.first + " was changed").c_str());
            }
        }

        for(auto pair : after) {
            if (before.count(pair.first) == 0) {
                syslog(LOG_INFO, (pair.first + " was added").c_str());
            }
        }
    }

    void Daemon::process() {
        syslog(LOG_INFO, "Daemon is starting...");
        while (isRunning) {
            if (needReload) {
                reload();
                if (!(isRunning = init())) {
                    syslog(LOG_ERR, "Initialization error!");
                    isRunning = false;
                }
                needReload = false;
            }
            auto newDirStatus = fileSearch(configReader.getInputPath());
            compare(allFilesWithTimestamp, newDirStatus);
            allFilesWithTimestamp = newDirStatus;
            usleep(configReader.getTime() * 1000);
        }
        syslog(LOG_INFO, "Daemon is close...");
    }