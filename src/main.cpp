#include <iostream>

#include "server.h"

int main(int argc, char* argv[]) {
    Server& s = Server::get_instance();
    s.run();
    return 0;
}