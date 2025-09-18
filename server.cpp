#include "server.h"

#include <arpa/inet.h>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>

Server::Server() {
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    throw std::runtime_error("failed to create socket");
  }
}

Server::~Server() { close(server_fd); }

void Server::bind_and_listen(uint32_t server_port,
                             uint8_t max_pending_connections) {
  port = server_port;
  address.sin_family = AF_INET;         // IPv4
  address.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces
  address.sin_port = htons(port);

  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    close(server_fd);
    throw std::runtime_error("setsockopt failed");
  }

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
    close(server_fd);
    throw std::runtime_error("bind failed");
  }

  if (listen(server_fd, max_pending_connections) == -1) {
    close(server_fd);
    throw std::runtime_error("listen failed");
  }
}

void Server::run() {
  std::cout << "Server listening on port " << port << std::endl;

  while (1) {
    socklen_t addrlen = sizeof(address);
    client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);

    if (client_fd == -1) {
      close(server_fd);
      throw std::runtime_error("accept failed");
    }

    std::string client_ip = inet_ntoa(address.sin_addr);
    int client_port = ntohs(address.sin_port);

    std::cout << "Connection from " << client_ip << ":" << client_port
              << std::endl;

    std::string msg = "Hello from server!\n";
    send(client_fd, msg.c_str(), msg.size(), 0);

    // Optionally receive data
    char buffer[3000] = {0};
    int bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_read > 0) {
      std::cout << "Client says: " << buffer << std::endl;
    }
    close(client_fd);
  }
}