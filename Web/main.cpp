#include <iostream>
#include <sys/syslog.h>
#include <sys/stat.h>
#include "VebChik.h"

using namespace std;

int main(int argc, char *argv[]) {
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        setsid();
        umask(0);
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        openlog("VebChik", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
        u_short port_number = 8082;
        syslog(LOG_INFO, "Server works on %d port", port_number);
        try {
            VebChik web(port_number);
            web.dispatchCommands();
        } catch (std::string &errorMsg) {
            syslog(LOG_ERR, errorMsg.c_str());
        }
    } else if (pid < 0) {
        syslog(LOG_ERR, "Can't demonize process");
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
    return 0;
}

