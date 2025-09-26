#pragma once

#include <arpa/inet.h>

#include <cstdint>
#include <mutex>

#define SERVER_PORT 9999
#define MAX_PENDING_CONNECTIONS 10
#define MAX_CONCURRENT_CONNECTIONS 10000

struct ConnectionInfo {
    std::mutex mutex;
    uint16_t open_connections = 0;
    const uint32_t max_connections = MAX_CONCURRENT_CONNECTIONS;
};

class Server {
   private:
    ConnectionInfo connection_info;
    uint32_t port;
    int server_fd;
    struct sockaddr_in address {};

    Server(uint32_t port, uint32_t max_pending_connections, uint32_t max_concurrent_connections);
    ~Server();

   public:
    Server(const Server& server) = delete;
    Server& operator=(const Server& server) = delete;
    Server(Server&& server) = delete;
    Server& operator=(Server&& server) = delete;

    static Server& get_instance() {
        static Server server(SERVER_PORT, MAX_PENDING_CONNECTIONS, MAX_CONCURRENT_CONNECTIONS);
        return server;
    }

    void run();
};