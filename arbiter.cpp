#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include <unistd.h>
#include <string>

using Client = websocketpp::client<websocketpp::config::asio_client>;

using ConnectionHdl = websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

void send_message(Client* client, ConnectionHdl* connection, std::string msg) {
  client->send(*connection, msg, websocketpp::frame::opcode::text);
}

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

void close_connection(Client* client, ConnectionHdl* connection) {
  client->close(*connection, websocketpp::close::status::normal, "done");
}

int ch = 0;
std::string move_c1 = "no_move";

void on_message(Client* client, ConnectionHdl hdl,
                websocketpp::config::asio_client::message_type::ptr msg) {

  auto toks = split(msg->get_payload(), ' ');
  if (toks[0] == "uciok"){
    send_message(client, &hdl, "ready");
  }
  else if (toks[0] == "readyok"){
    send_message(client, &hdl, "ucinewgame");
    ch = 1;
  }
  else if (toks[0] == "bestmove"){
    move_c1 = toks[1];
  }
}

void on_open(Client* client, ConnectionHdl* connection, ConnectionHdl hdl) {
  *connection = hdl;
  std::cout << "Connected\n";
}

void turn_off_logging(Client& client) {
  client.clear_access_channels(websocketpp::log::alevel::all);
  client.clear_error_channels(websocketpp::log::elevel::all);
}

void set_message_handler(Client& client) {
  client.set_message_handler(
      websocketpp::lib::bind(&on_message, &client, ::_1, ::_2));
}

void set_open_handler(Client& client, ConnectionHdl* connection) {
  client.set_open_handler(
      websocketpp::lib::bind(&on_open, &client, connection, ::_1));
}

void set_url(Client& client, std::string url) {
  websocketpp::lib::error_code ec;
  auto connection = client.get_connection(url, ec);
  client.connect(connection);
}

// argv contains url1, url2, fp/sp.
int main(int argc, char* argv[]) {

  int done = 0;
  Client client1, client2;
  ConnectionHdl connection1;
  ConnectionHdl connection2;

  turn_off_logging(client1);
  turn_off_logging(client2);

  client1.init_asio();
  client2.init_asio();

  set_url(client1, "ws://localhost:" + std::string(argv[1]));
  set_url(client2, "ws://localhost:" + std::string(argv[2]));

  set_open_handler(client1, &connection1);
  set_open_handler(client2, &connection2);
  set_message_handler(client1);
  set_message_handler(client2);

  websocketpp::lib::thread t1(&Client::run, &client1);
  websocketpp::lib::thread t2(&Client::run, &client2);

  std::cout << "Connected this far\n";

  while (done < 5) {
    while(!ch){}

    // first player is c1. second player c2.
    send_message(&client1, &connection1, "go");
    usleep(2000000);
    send_message(&client1, &connection1, "stop");
    usleep(200000);
    if (move_c1 == "no_move"){
        std::cout << "The arbiter did not get your move. You were playing as " << argv[3] << std::endl;
        break;
    }
    else{
        send_message(&client2, &connection2, "position startpos moves " + move_c1);
        move_c1 = "no_move";
        send_message(&client2, &connection2, "go");
        usleep(2000000);
        send_message(&client2, &connection2, "stop");
        usleep(200000);
        if (move_c1 == "no_move"){
            std::cout << "The arbiter did not get your move. You were playing as " << argv[3] << std::endl;
            break;
        }
        else{
            send_message(&client1, &connection1, "position startpos moves " + move_c1);
            move_c1 = "no_move";
        }
    }
    done++;
  }
  send_message(&client1, &connection1, "quit");
  send_message(&client2, &connection2, "quit");
  std::cout << "All good. You were playing as " << argv[3] << std::endl;
  t1.join();
  t2.join();
}
