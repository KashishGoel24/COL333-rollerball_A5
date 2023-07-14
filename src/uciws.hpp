#pragma once

#include <csignal>
#include <string>
#include <thread>
#include <websocket.h>

#include "game.hpp"

struct UCIConnData {
};

class UCIWSServer {

    public:
    using WSServer = websocket::WSServer<UCIWSServer, UCIConnData>;
    using WSConn = WSServer::Connection;

    WSServer server;
    WSConn connection;
    // Game game; - TODO

    std::thread ws_thr;
    std::atomic<bool> running;

    std::string ip;
    uint32_t port;

    UCIWSServer(std::string name, std::string ip, uint32_t port);

    void start();
    void end();
    void on_uci();
    void on_isready();
    void on_ucinewgame();
    void on_position();
    void on_go();
    void on_stop();

    bool onWSConnect(WSConn& conn, const char* request_uri, const char* host, const char* origin, const char* protocol,
                   const char* extensions, char* resp_protocol, uint32_t resp_protocol_size, char* resp_extensions,
                   uint32_t resp_extensions_size);
    
    void onWSClose(WSConn& conn, uint16_t status_code, const char* reason);
    void onWSMsg(WSConn& conn, uint8_t opcode, const uint8_t* payload, uint32_t pl_len);

};
