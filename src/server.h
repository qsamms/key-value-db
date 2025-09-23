#pragma once

#include <arpa/inet.h>

#include <cstdint>

class Server {
   private:
    uint32_t port;
    int server_fd;
    struct sockaddr_in address {};

   public:
    Server();
    ~Server();

    void bind_and_listen(uint32_t port, uint8_t max_pending_connections);
    void run();
};