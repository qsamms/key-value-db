#include <iostream>

#include "server.h"

#define SERVER_PORT 9999
#define MAX_PENDING_CONNECTIONS 10

int main(int argc, char* argv[]) {
    Server s;
    s.bind_and_listen(SERVER_PORT, MAX_PENDING_CONNECTIONS);
    s.run();
    return 0;
}