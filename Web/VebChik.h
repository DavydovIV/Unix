//
// Created by baka on 12.10.15.
//

#ifndef VEBCHIK_VEBHICK_H
#define VEBCHIK_VEBHICK_H

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/syslog.h>
#include <sstream>

using namespace std;
class VebChik {
    string rootPath = "/var/www/";
    string index = "index.html";
    string error404 = "notFount.html";
    u_short port = 0;
    int socketDescriptor = 0;
    struct sockaddr_in serverSocket;

    void createSocket() {
        int boolean = 1;

        int cli_size;

        if ((socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            throw string("Can't create socket");
        }

        serverSocket.sin_family = AF_INET;
        serverSocket.sin_port = port;
        serverSocket.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(socketDescriptor, (struct sockaddr *) &serverSocket, sizeof(serverSocket)) == -1) {
            perror(nullptr);
            throw string("Can't bind socket");
        }

        if (listen(socketDescriptor, 5) == -1) {
            throw string("Can't start listen socket");
        }
    }

    void notFound(int connected) {
        string errorPage = "HTTP/1.0 404 Page Not Found\r\n";
        errorPage += "Server: VebChik\r\n";
        errorPage += "Content-Type: text/html\r\n\r\n";
        errorPage += "<HTML><HEAD><TITLE>404 Page not found!\r\n";
        errorPage += "</TITLE></HEAD>\r\n";
        errorPage += "<BODY><P>404 Page not found!\r\n";
        errorPage += "</BODY></HTML>\r\n";
        send(connected, errorPage.c_str(), strlen(errorPage.c_str()), 0);
    }

public:
    VebChik(u_short port) {
        this->port = htons(port);
        createSocket();
    }

    void dispatchCommands() {
        bool interrupted = false;
        struct sockaddr_in clientSocket;
        int connected;
        socklen_t clientSize = sizeof(struct sockaddr_in);
        while (true) {
            connected = accept(socketDescriptor, (struct sockaddr *) &clientSocket, &clientSize);
            if (connected < 0) {
                throw string("Error on connect");
            }

            int pid = fork();

            if (pid < 0) {
                throw string("Error on fork");
            } else if (pid == 0) {
                close(socketDescriptor);
                processRequest(connected);
                exit(EXIT_SUCCESS);
            } else {
                close(connected);
            }
        }
    }

private:
    void processRequest(int connected) {
        char req[1024];
        recv(connected, &req, sizeof(req), 0);
        string request(req);

        istringstream iss(request);
        string method;
        iss >> method;
        if (method == "GET") {
            processGET(connected, iss);
        }
    }

    void processGET(int connected, istringstream& iss) {
        string path, absPath = rootPath;
        iss >> path;
        if (path == "/") {
            absPath += index;
        } else {
            absPath += path;
        }
        replyFile(connected, absPath);
    }

    void replyFile(int connected, const string &absPath) {
        FILE *file = fopen(absPath.c_str(), "r");
        if (file == NULL) {
            notFound(connected);
        } else {
            char content[1024], rep[1024];
            strcpy(rep, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n");
            send(connected, &rep, strlen(rep), 0);
            while (fgets(content, sizeof(content), file) != NULL) {
                send(connected, &content, strlen(content), 0);
            }
        }
    }
};


#endif //VEBCHIK_VEBHICK_H
