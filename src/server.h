#pragma once

#include <arpa/inet.h>

#include <cstdint>

#define SERVER_PORT 9999
#define MAX_PENDING_CONNECTIONS 10

class Server {
   private:
    uint32_t port;
    int server_fd;
    struct sockaddr_in address {};

    Server(uint32_t port, uint32_t max_pending_connections);
    ~Server();

   public:
    Server(const Server& server) = delete;
    Server& operator=(const Server& server) = delete;
    Server(Server&& server) = delete;
    Server& operator=(Server&& server) = delete;

    static Server& get_instance() {
        static Server server(SERVER_PORT, MAX_PENDING_CONNECTIONS);
        return server;
    }

    void run();
};