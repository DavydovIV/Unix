#include <iostream>
#include <string>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/syslog.h>
#include <sstream>

using namespace std;
class Web {
    string rootPath = "/usr/share/doc/libsane/umax/";
    string index = "sane-umax-doc.html";
    string error404 = "notFount.html";
    u_short port = 0;
    int socketDescriptor = 0;
    struct sockaddr_in serverSocket;

    void createSocket();

    void notFound(int connected);

public:
    Web(u_short port) {
        this->port = htons(port);
        createSocket();
    }

    void dispatchCommands();

private:
    void processRequest(int connected);

    void processGET(int connected, istringstream& iss);

    void replyFile(int connected, const string &absPath);
};
