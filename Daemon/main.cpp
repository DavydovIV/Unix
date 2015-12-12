#include <iostream>
#include <signal.h>
#include "Daemon.h"

using namespace std;

auto my_daemon = Daemon();

void signal_handler(int sig) {
    switch (sig) {
        case SIGHUP:
            syslog(LOG_INFO, "Reloading config...");
            my_daemon.reloadConfig();
            break;
        case SIGTERM:
            my_daemon.die();
            break;
    }
}

int main() {
    int pid = fork();

    if (pid == 0) {
        umask(0);
        if (setsid() < 0){
            return 0;
        }

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        signal(SIGHUP, signal_handler);
        signal(SIGTERM, signal_handler);

        openlog("SATANA", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

        my_daemon.start();
        return 0;
    } else {
        return 0;
    }
}