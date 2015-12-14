#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <set>
#include <bits/stl_algo.h>
#include <unistd.h>

class Chat {
    int listener;
    timeval timeout;
    std::set<int> clients;
    const int max_length = 2048;

public:
    Chat(u_short port, int timeout_sec);

    ~Chat() {
        close(listener);
    }

    void mes();

private:
    
    void init_clients() {
        clients.clear();
    }   
};
