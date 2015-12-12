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

int create_nonblock_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        throw "Can't create socket";
    if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
        throw "Fcntl method error";
    return sock;
}


timeval create_timeout(int seconds) {
    timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
    return timeout;
}

vector<string> current_messages;
string current_message;
const int max_length = 2048;

int process_keyboard() {
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

void send_message(int fd, const string& message) {
    send(fd, message.c_str(), message.length(), 0);
}

void process_socket(int fd) {
    char buf[max_length];
    recv(fd, buf, max_length, 0);
    string incoming_msg = string(buf);
    current_messages.push_back(incoming_msg);
}

void print_messages() {
    system("clear");
    for (auto s : current_messages) {
        cout << s << endl;
    }
}

int main() {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    server = gethostbyname(address);
            cout << "HELLO!!!!";

    if (server == NULL) {
        cerr << "ERROR, no such host\n";
        return -1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    //serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    timeval timeout = create_timeout(120);
    int listener = create_nonblock_socket();
    fcntl(0, F_SETFL, O_NONBLOCK);

    /* Now connect to the server */
    while (true) {
        if (connect(listener, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            cerr << "ERROR connecting\n";
            perror(nullptr);
        } else {
            break;
        }
    }
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(listener, &read_set);
    FD_SET(0, &read_set);
    while (true) {
        if (select(max(0, listener) + 1, &read_set, nullptr, nullptr, &timeout)) {
            if (FD_ISSET(0, &read_set)) {
                if (process_keyboard()) {
                    send_message(listener, current_message);
                    current_message.clear();
                }
            } 
            if (FD_ISSET(listener, &read_set)) {
                    process_socket(listener);
            }

            FD_SET(listener, &read_set);
            FD_SET(0, &read_set);
            print_messages();
        }
    }
}
