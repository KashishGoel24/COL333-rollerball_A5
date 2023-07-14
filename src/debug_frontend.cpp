#include "server.hpp"

#include <iostream>
#include <thread>
#include <asio/io_service.hpp>

int main(int argc, char* argv[])
{
    
    int port_num = std::stoi(argv[1]);

    //Create the event loop for the main thread, and the WebSocket server
    asio::io_service mainEventLoop;
    WebsocketServer server;
    
    //Register our network callbacks, ensuring the logic is run on the main thread's event loop
    server.connect([&mainEventLoop, &server](ClientConnection conn)
    {
        mainEventLoop.post([conn, &server]()
        {
            std::clog << "Connection opened." << std::endl;
            std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
            
        });
    });
    server.disconnect([&mainEventLoop, &server](ClientConnection conn)
    {
        mainEventLoop.post([conn, &server]()
        {
            std::clog << "Connection closed." << std::endl;
            std::clog << "There are now " << server.numConnections() << " open connections." << std::endl;
        });
    });
    server.message([&mainEventLoop](ClientConnection conn, const string& message)
    {
        mainEventLoop.post([conn, message]()
        {
            std::clog << "message handler on the main thread" << std::endl;
            std::clog << "Message payload: " << message << std::endl;
        });
    });
    
    //Start the networking thread
    std::thread serverThread([&server, &port_num]() {
        server.run(port_num);
    });
    
    //Start a keyboard input thread that reads from stdin
    std::thread inputThread([&server, &mainEventLoop]()
    {
        string input;
        while (1)
        {
            //Read user input from stdin
            std::getline(std::cin, input);
            
            //Broadcast the input to all connected clients (is sent on the network thread)
            server.broadcastMessage(input);
            
            //Debug output on the main thread
            mainEventLoop.post([]() {
                std::clog << "User input debug output on the main thread" << std::endl;
            });
        }
    });
    
    //Start the event loop for the main thread
    asio::io_service::work work(mainEventLoop);
    mainEventLoop.run();
    
    return 0;
}
