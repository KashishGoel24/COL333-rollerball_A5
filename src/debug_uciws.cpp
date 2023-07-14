#include "uciws.hpp"

int main(int argc, char* argv[]) {

    UCIWSServer server("rollerball", "localhost", 8181);

    server.start();
    
    return 0;
}

