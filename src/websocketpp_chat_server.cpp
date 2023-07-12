#include <csignal>
#include <iostream>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>

using Server = websocketpp::server<websocketpp::config::asio>;
using ConnectionHdl = websocketpp::connection_hdl;
using SslContext = websocketpp::lib::asio::ssl::context;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

bool equal_connection_hdl(ConnectionHdl& a, ConnectionHdl& b) {
  return a.lock() == b.lock();
}

void remove_connection(std::vector<ConnectionHdl>* connections,
                       ConnectionHdl& hdl) {
  auto equal_connection_hdl_predicate =
      std::bind(&equal_connection_hdl, hdl, ::_1);
  connections->erase(
      std::remove_if(std::begin(*connections), std::end(*connections),
                     equal_connection_hdl_predicate),
      std::end(*connections));
}

void on_message(Server* server, std::vector<ConnectionHdl>* connections,
                ConnectionHdl hdl,
                websocketpp::config::asio::message_type::ptr msg) {
  std::cout << "on_message: " << msg->get_payload() << std::endl;
  for (auto& connection : *connections) {
    if (connection.lock() != hdl.lock()) {
      server->send(connection, msg->get_payload(),
                   websocketpp::frame::opcode::text);
    }
  }
}

void on_open(std::vector<ConnectionHdl>* connections, ConnectionHdl hdl) {
  std::cout << "on_open" << std::endl;
  connections->push_back(hdl);
  std::cout << "connections: " << connections->size() << std::endl;
}

void on_close(Server* server, std::vector<ConnectionHdl>* connections,
              ConnectionHdl hdl) {
  std::cout << "on_close" << std::endl;
  remove_connection(connections, hdl);
  std::cout << "connections: " << connections->size() << std::endl;
}

void turn_off_logging(Server& server) {
  server.clear_access_channels(websocketpp::log::alevel::all);
  server.clear_error_channels(websocketpp::log::elevel::all);
}

void set_message_handler(Server& server,
                         std::vector<ConnectionHdl>& connections) {
  server.set_message_handler(
      websocketpp::lib::bind(&on_message, &server, &connections, ::_1, ::_2));
}

void set_open_handler(Server& server, std::vector<ConnectionHdl>& connections) {
  server.set_open_handler(websocketpp::lib::bind(&on_open, &connections, ::_1));
}

void set_close_handler(Server& server,
                       std::vector<ConnectionHdl>& connections) {
  server.set_close_handler(
      websocketpp::lib::bind(&on_close, &server, &connections, ::_1));
}

int main() {
  Server server;
  std::vector<ConnectionHdl> connections;

  turn_off_logging(server);

  server.init_asio();

  set_message_handler(server, connections);
  set_open_handler(server, connections);
  set_close_handler(server, connections);

  server.listen(30001);
  server.start_accept();
  server.run();
}
