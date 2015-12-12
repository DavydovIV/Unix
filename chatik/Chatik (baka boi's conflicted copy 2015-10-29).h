//
// Created by baka on 25.10.15.
//

#ifndef CHATIK_CHATIK_H
#define CHATIK_CHATIK_H


#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <set>
#include <bits/stl_algo.h>
#include <unistd.h>
#include <iostream>

class Chatik {
    int listener;
    timeval timeout;
    std::set<int> clients;
    const int max_length = 2048;

public:
    Chatik(u_short port, int timeout_sec) {
        listener = create_nonblock_socket();
        bind_socket(listener, port);
        init_clients();
        timeout = create_timeout(timeout_sec);
    }

    ~Chatik() {
        close(listener);
    }

    void process_messages() {
        while(true) {
            fd_set read_set;
            init_read_set(&read_set);
            int maximum = std::max(listener,
                *std::max_element(clients.begin(), clients.end()));
            if (select(maximum + 1, &read_set, nullptr, nullptr, &timeout) <= 0) {
                throw "Error on select";
            }

            if (FD_ISSET(listener, &read_set)) {
                process_connection();
            }
            for (auto client : clients) {
                if (FD_ISSET(client, &read_set))
                    process_string(client);
            }

        }
    }

    void process_string(const int client) {
        char buf[max_length];
        int len = recv(client, buf, max_length, 0);

        if (len <= 0) {
            close(client);
            clients.erase(client);
        } else if (len > 0) {
            for (auto receiver : clients) {
                std::cout << buf << std::endl;
                send(receiver, buf, len, 0);
            }
        }
    }

private:
    void process_connection() {
        int sock = accept(listener, nullptr, nullptr);
        if (sock < 0)
            throw "Error on accepting connection";
        fcntl(sock, F_SETFL, O_NONBLOCK);
        clients.insert(sock);
    }

    void init_read_set(fd_set *read_set) const {
        FD_ZERO(read_set);
        FD_SET(listener, read_set);
        for (auto client : clients) {
            FD_SET(client, read_set);
        }
    }

    timeval create_timeout(int seconds) {
        timeval timeout;
        timeout.tv_sec = seconds;
        timeout.tv_usec = 0;
        return timeout;
    }


    int create_nonblock_socket() {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            throw "Can't create socket";
        fcntl(sock, F_SETFL, O_NONBLOCK);
        return sock;
    }

    void bind_socket(int sock, u_short port) {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        if (bind(sock, (const sockaddr *) & addr, sizeof(addr)) < 0)
            throw "Can't bind socket";
        if (listen(sock, 2) < 0)
            throw "Listen";
    }

    void init_clients() {
        clients.clear();
    }
};


#endif //CHATIK_CHATIK_H
