#include "Chat.h"

Chat::Chat(u_short port, int timeout_sec) {
        listener = create_nonblock_socket();
        bind_socket(listener, port);
        init_clients();
        timeout = create_timeout(timeout_sec);
    }

void Chat::process_messages() {
        while(true) {
            fd_set read_set;
            init_read_set(&read_set);
            int maximum = std::max(listener,
                *std::max_element(clients.begin(), clients.end()));
            if (select(maximum + 1, &read_set, nullptr, nullptr, &timeout) <= 0)
                throw "Error on select";

            if (FD_ISSET(listener, &read_set)) 
                process_connection();
            
            for (auto client : clients){ 
                if (FD_ISSET(client, &read_set))
                    process_string(client);
            }
        }
    }

void Chat::process_string(const int client) {
        char buf[max_length];
        int len = recv(client, buf, max_length, 0);

        if (len <= 0) {
            close(client);
            clients.erase(client);
        } else if (len > 0)
            for (auto receiver : clients) 
                send(receiver, buf, len, 0);
     }

void Chat::process_connection() {
        int sock = accept(listener, nullptr, nullptr);
        if (sock < 0)
            throw "Error on accepting connection";
        fcntl(sock, F_SETFL, O_NONBLOCK);
        clients.insert(sock);
    }

void Chat::init_read_set(fd_set* read_set) {
        FD_ZERO(read_set);
        FD_SET(listener, read_set);
        for (auto client : clients) {
            FD_SET(client, read_set);
        }
    }

timeval Chat::create_timeout(int seconds) {
        timeval timeout;
        timeout.tv_sec = seconds;
        timeout.tv_usec = 0;
        return timeout;
    }


int Chat::create_nonblock_socket() {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            throw "Can't create socket";
        fcntl(sock, F_SETFL, O_NONBLOCK);
        return sock;
    }

void Chat::bind_socket(int sock, u_short port) {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        if (bind(sock, (const sockaddr *) & addr, sizeof(addr)) < 0)
            throw "Can't bind socket";
        if (listen(sock, 2) < 0)
            throw "Listen";
    }