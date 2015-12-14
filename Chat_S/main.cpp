#include <iostream>
#include <sys/syslog.h>
#include <sys/stat.h>
#include "Chat.h"

using namespace std;

int main() {
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        setsid();
        umask(0);
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        openlog("Chat", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
        try {
            syslog(LOG_INFO, "Initialization... \n Check User Rights\n Preparation for launch\n Launching nuclear missiles !!!");
            Chat chat = Chat(8111, 120);
            chat.mes();
        } catch (const char* error) {
            syslog(LOG_ERR, error);
        }
        syslog(LOG_INFO, "Ops... Aborted!");
    } else if (pid < 0) {
        syslog(LOG_ERR, "Can't demonize process");
    }
    return 0;
}
