#include "Chat.h"

Chat::Chat(u_short port, int timeout_sec) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            throw "Can't create socket";
        fcntl(sock, F_SETFL, O_NONBLOCK);
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        if (bind(sock, (const sockaddr *) & addr, sizeof(addr)) < 0)
            throw "Can't bind socket";
        if (listen(sock, 2) < 0)
            throw "Listen";
        
        init_clients();
        timeval timeout;
        timeout.tv_sec = timeout_sec;
        timeout.tv_usec = 0;
    }

void Chat::mes() {
        while(true) {
            fd_set read_set;
            FD_ZERO(&read_set);
            FD_SET(listener, &read_set);
            for (auto client : clients) {
                FD_SET(client, &read_set);
            }
            int maximum = std::max(listener,
                *std::max_element(clients.begin(), clients.end()));
            if (select(maximum + 1, &read_set, nullptr, nullptr, &timeout) <= 0)
                throw "Error on select";

            if (FD_ISSET(listener, &read_set)){                    
                int sock = accept(listener, nullptr, nullptr);
                if (sock < 0)
                throw "Error on accepting connection";
                fcntl(sock, F_SETFL, O_NONBLOCK);
                clients.insert(sock);
            }
            
            for (auto client : clients){ 
                if (FD_ISSET(client, &read_set)){
                    char buf[max_length];
                    int len = recv(client, buf, max_length, 0);
                    if (len <= 0) {
                    close(client);
                    clients.erase(client);
                    } else if (len > 0)
                    for (auto receiver : clients) 
                        send(receiver, buf, len, 0);
                }
            }
        }
    }