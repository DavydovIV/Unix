#include <iostream>
#include <sys/syslog.h>
#include <sys/stat.h>
#include "Web.h"

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
        openlog("Web ", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
        u_short port_number = 8090;
        syslog(LOG_INFO, "Daemon is start with Port: %d ", port_number);
        try {
            Web web(port_number);
            web.dispatchCommands();
        } catch (std::string &errorMsg) {
            syslog(LOG_ERR, errorMsg.c_str());
        }
    } else if (pid < 0) {
        syslog(LOG_ERR, "Sorry :( I can't demonize process");
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
    return 0;
}

