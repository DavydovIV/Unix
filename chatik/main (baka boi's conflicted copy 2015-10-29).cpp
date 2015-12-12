#include <iostream>
#include <sys/syslog.h>
#include "Chatik.h"

using namespace std;

int main() {
    openlog("Chatik", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    try {
        syslog(LOG_INFO, "Chatik started...!");
        Chatik chatik = Chatik(8082, 120);
        chatik.process_messages();
    } catch (const char* error) {
        syslog(LOG_ERR, error);
    }
    syslog(LOG_INFO, "Chatik stopped...!");
}
