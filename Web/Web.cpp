#include "Web.h"

void Web::createSocket() {
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

void Web::dispatchCommands() {
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

void Web::notFound(int connected) {
        string errorPage = "HTTP/1.0 404 Page Not Found\r\n";
        errorPage += "Server: Web\r\n";
        errorPage += "Content-Type: text/html\r\n\r\n";
        errorPage += "<HTML><HEAD><TITLE>404 Page not found!\r\n";
        errorPage += "</TITLE></HEAD>\r\n";
        errorPage += "<BODY><P>404 Page not found!\r\n";
        errorPage += "</BODY></HTML>\r\n";
        send(connected, errorPage.c_str(), strlen(errorPage.c_str()), 0);
    }

void Web::processRequest(int connected) {
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

void Web::processGET(int connected, istringstream& iss) {
        string path, absPath = rootPath;
        iss >> path;
        if (path == "/") {
            absPath += index;
        } else {
            absPath += path;
        }
        replyFile(connected, absPath);
    }

void Web::replyFile(int connected, const string &absPath) {
    FILE *file = fopen(absPath.c_str(), "rb");
    unsigned long fileLen = 0;
    if (file == NULL) {
        notFound(connected);
    } else {
        char rep[1024];
        strcpy(rep, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n");
        send(connected, &rep, strlen(rep), 0);

        fseek(file, 0, SEEK_END);
        fileLen = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *buffer=(char *)malloc(fileLen+1);

        fread(buffer, fileLen, 1, file);
        send(connected, buffer, fileLen + 1, 0);
    }
}