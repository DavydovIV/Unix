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
#include <thread>
#include <mutex>

using namespace std;

const char* address = "localhost";
ushort port = 8082;

int create_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        throw "Can't create socket";
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
int listener;
mutex listener_lock;


int process_keyboard() {
    current_message.clear();
    std::getline(std::cin, current_message);
    return 1;
}

void send_message(int fd, const string& message) {
    if (send(fd, message.c_str(), message.length(), 0) <= 0)
        perror(nullptr);
}

int process_socket(int fd) {
    char buf[max_length];
    int r;
    if((r = recv(fd, buf, max_length, 0)) > 0) {
        string incoming_msg = string(buf);
        current_messages.push_back(incoming_msg);
        return r;
    }
    return 0;
}

void print_messages() {
    system("clear");
    for (auto s : current_messages) {
        cout << s << endl;
    }
}

void consumer_function() {
    while (true) {
        listener_lock.lock();
        if (process_socket(listener))
            print_messages();
        listener_lock.unlock();
        usleep(50000);
    }
}

int main() {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    server = gethostbyname(address);

    if (server == NULL) {
        cerr << "ERROR, no such host\n";
        return -1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    timeval timeout = create_timeout(120);
    listener = create_socket();

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
    thread consumer (consumer_function);
    while (true) {
        if (process_keyboard()) {
            send_message(listener, current_message);
            current_message.clear();
        }
    }
    consumer.join();
}
