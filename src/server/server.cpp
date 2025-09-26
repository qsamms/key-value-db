#include "server.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include <connection/connection.h>

using RuntimeError = std::runtime_error;

Server::Server(uint32_t server_port, uint32_t max_pending_connections,
               uint32_t max_concurrent_connections) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        throw RuntimeError("failed to create socket");
    }

    port = server_port;
    address.sin_family = AF_INET;          // IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Bind to all interfaces
    address.sin_port = htons(port);

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        close(server_fd);
        throw RuntimeError("setsockopt failed");
    }

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        close(server_fd);
        throw RuntimeError("bind failed");
    }

    if (listen(server_fd, max_pending_connections) == -1) {
        close(server_fd);
        throw RuntimeError("listen failed");
    }
}

Server::~Server() {
    close(server_fd);
}

void Server::run() {
    std::cout << "Server listening on port " << port << std::endl;

    while (1) {
        socklen_t addrlen = sizeof(address);
        int client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_fd == -1) {
            std::cout << "Failed to accept connection" << std::endl;
        }

        bool conn_ok;
        {
            std::lock_guard<std::mutex> lck(connection_info.mutex);
            conn_ok = connection_info.open_connections < connection_info.max_connections;
        }

        if (conn_ok) {
            std::string client_ip = inet_ntoa(address.sin_addr);
            int client_port = ntohs(address.sin_port);
            std::cout << "Connection from " << client_ip << ":" << client_port << std::endl;
            {
                std::lock_guard<std::mutex> lck(connection_info.mutex);
                connection_info.open_connections++;
            }
            std::thread t([client_fd, this] { Connection c(client_fd, &connection_info); });
            t.detach();
        } else {
            std::cout << "Connection limit reached (" << connection_info.max_connections
                      << ") rejecting connection" << std::endl;
            close(client_fd);
        }
    }
}
