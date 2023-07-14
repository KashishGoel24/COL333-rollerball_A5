#pragma once

#include <csignal>
#include <string>
#include <thread>
#include <asio/io_service.hpp>

#include "server.hpp"

struct UCIConnData {
};

class UCIWSServer {

    public:

    asio::io_service main_evt_loop;
    WebsocketServer server;
    
    std::thread server_thread;
    std::atomic<bool> running;

    std::string ip;
    uint32_t port;

    UCIWSServer(std::string name, std::string ip, uint32_t port);

    void start();
    void stop();

    void handle_message(ClientConnection conn, const std::string& message);

    void on_uci();
    void on_isready();
    void on_ucinewgame();
    void on_position(std::vector<std::string>& toks);
    void on_go(std::vector<std::string>& toks);
    void on_stop();
    void on_quit();
};
