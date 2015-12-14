#include <iostream>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

const char* address = "localhost";
ushort port = 8111;

vector<string> current_messages;
string current_message;
const int max_length = 2048;

int inter() {
    char c;
    if (read(0, &c, sizeof(c)) > 0) {
        if (c != 8) {
            current_message += c;
        } else {
            current_message.pop_back();
        }
        if (c == '\n') {
            return 1;
        }
    }
    return 0;
}

int main() {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    server = gethostbyname(address);
            cout << "yo!";

    if (server == NULL) {
        cerr << "ERROR, no such host\n";
        return -1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    
    timeval timeout;
    timeout.tv_sec = 120;
    timeout.tv_usec = 0;
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        throw "Can't create socket";
    if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
        throw "Fcntl method error";
    return sock;
    
    fcntl(0, F_SETFL, O_NONBLOCK);

    while (true) {
        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            cerr << "ERROR connecting\n";
            perror(nullptr);
        } else {
            break;
        }
    }
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(sock, &read_set);
    FD_SET(0, &read_set);
    while (true) {
        if (select(max(0, sock) + 1, &read_set, nullptr, nullptr, &timeout)) {
            if (FD_ISSET(0, &read_set)) {
                if (inter()) {
                    send(sock, current_message.c_str(), current_message.length(), 0);
                    current_message.clear();
                }
            } 
            if (FD_ISSET(sock, &read_set)) {
                char buf[max_length];
                recv(sock, buf, max_length, 0);
                string incoming_msg = string(buf);
                current_messages.push_back(incoming_msg);
            }

            FD_SET(sock, &read_set);
            FD_SET(0, &read_set);
            system("clear");
            for (auto s : current_messages) {
                cout << s << endl;
            }
        }
    }
}
