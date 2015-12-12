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

    void process_messages();

    void process_string(const int client);

private:
    void process_connection();

    void init_read_set(fd_set *read_set);

    timeval create_timeout(int seconds);


    int create_nonblock_socket();

    void bind_socket(int sock, u_short port);

    void init_clients() {
        clients.clear();
    }
};
