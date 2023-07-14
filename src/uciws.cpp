#include <iostream>
#include "uciws.hpp"

UCIWSServer::UCIWSServer(std::string name, std::string ip, uint32_t port, Game& game) {
    game = game;
    this->ip = ip;
    this->port = port;
}

void UCIWSServer::run() {

    // enter into polling loop
    if (!server.init(ip.c_str(), port)) {
        std::cout << "wsserver init failed: " << server.getLastError() << std::endl;
        return;
    }

    running = true;
    ws_thr = std::thread([this]() {
        while (running.load(std::memory_order_relaxed)) {
            server.poll(this);
            std::this_thread::yield();
        }
    });

    std::cout << "Server running..." << std::endl;
    ws_thr.join();
    std::cout << "Server stopped..." << std::endl;
}

bool UCIWSServer::onWSConnect(WSConn& conn, const char* request_uri, const char* host, const char* origin, const char* protocol,
               const char* extensions, char* resp_protocol, uint32_t resp_protocol_size, char* resp_extensions,
               uint32_t resp_extensions_size) {

    this->connection = conn;

    struct sockaddr_in addr;
    conn.getPeername(addr);
    std::cout << "ws connection from: " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;
    std::cout << "request_uri: " << request_uri << std::endl;
    std::cout << "host: " << host << std::endl;
    if (origin) {
      std::cout << "origin: " << origin << std::endl;
    }
    if (protocol) {
      std::cout << "protocol: " << protocol << std::endl;
    }
    if (extensions) {
      std::cout << "extensions: " << extensions << std::endl;
    }

    return true;
}

void UCIWSServer::onWSClose(WSConn& conn, uint16_t status_code, const char* reason) {
    std::cout << "WS closed, error code " << status_code << std::endl;
}

void UCIWSServer::onWSMsg(WSConn& conn, uint8_t opcode, const uint8_t* payload, uint32_t pl_len) {

    std::string message((char*)payload);

    // TODO process message and call methods specified

    std::cout << "Received message " << message << std::endl;
}
