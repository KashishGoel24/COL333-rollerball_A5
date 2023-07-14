#include <iostream>
#include <vector>

#include "uciws.hpp"

UCIWSServer server("rollerball", "localhost", 8181);

void my_handler(int s) {
  server.end();
}

int main(int argc, char** argv) {

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    server.start();

    return 0;
}
