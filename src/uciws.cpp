#include <iostream>
#include "uciws.hpp"

#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <iterator>

template <typename Out>
void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

UCIWSServer::UCIWSServer(std::string name, std::string ip, uint32_t port) {
    this->ip = ip;
    this->port = port;
}

void UCIWSServer::handle_message(ClientConnection conn, const std::string& message) {

    auto toks = split(message, ' ');

    if (toks[0] == "uci") {
        on_uci();
    }
    else if (toks[0] == "isready") {
        on_isready();
    }
    else if (toks[0] == "ucinewgame") {
        on_ucinewgame();
    }
    else if (toks[0] == "position") {
        on_position(toks);
    }
    else if (toks[0] == "go") {
        on_go(toks);
    }
    else if (toks[0] == "stop") {
        on_stop();
    }
    else if (toks[0] == "quit") {
        on_quit();
    }
    else {
        std::cout << "Unsupported message\n";
    }
}

void UCIWSServer::start() {

    //Register our network callbacks, ensuring the logic is run on the main thread's event loop
    server.connect([this](ClientConnection conn)
    {
        this->main_evt_loop.post([conn, this]()
        {
            std::clog << "Connection opened." << std::endl;
            std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
            
            //Send a hello message to the client
            server.sendMessage(conn, "hello");
        });
    });

    server.disconnect([this](ClientConnection conn)
    {
        main_evt_loop.post([conn, this]()
        {
            std::clog << "Connection closed." << std::endl;
            std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
        });
    });

    server.message([this](ClientConnection conn, const string& message) {
        this->handle_message(conn, message);
    });
    
    //Start the networking thread
    this->server_thread = std::thread([this]() {
        server.run(port);
    });
    
    //Start the event loop for the main thread
    asio::io_service::work work(main_evt_loop);
    main_evt_loop.run();
}

void UCIWSServer::on_uci() {
    std::cout << "In method on_uci\n";
}

void UCIWSServer::on_isready() {
    std::cout << "In method on_isready\n";
}

void UCIWSServer::on_ucinewgame() {
    std::cout << "In method on_ucinewgame\n";
}

void UCIWSServer::on_position(std::vector<std::string>& toks) {
    std::cout << "In method on_position\n";
}

void UCIWSServer::on_go(std::vector<std::string>& toks) {
    std::cout << "In method on_go\n";
}

void UCIWSServer::on_stop() {
    std::cout << "In method on_stop\n";
}

void UCIWSServer::on_quit() {
    std::cout << "In method on_quit\n";
}

