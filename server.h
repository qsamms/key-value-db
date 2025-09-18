#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <cstdint>


class Server {
    private:
        uint32_t port;
        int server_fd;
        int client_fd;
        struct sockaddr_in address{};

    public: 
        Server();
        ~Server();

        void bind_and_listen(uint32_t port, uint8_t max_pending_connections);
        void run();
};

#endif