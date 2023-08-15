CC=g++
CFLAGS=-Wall -std=c++17 -O3 -funroll-loops -DASIO_STANDALONE

INCLUDES=-Iinclude #-I/opt/homebrew/opt/openssl@1.1/include/
#LIBPATH=-L/opt/homebrew/opt/openssl@1.1/lib/
#LIBS=-lcrypto -lssl

rollerball:
	$(CC) $(CFLAGS) $(INCLUDES) src/server.cpp src/board.cpp src/engine.cpp src/rollerball.cpp src/uciws.cpp -o bin/rollerball

lib:
	$(CC) $(CFLAGS) $(INCLUDES) -shared src/board.cpp -o bin/librollerball.so

dbg_frontend: src/debug_frontend.cpp 
	$(CC) $(CFLAGS) $(INCLUDES) src/server.cpp src/debug_frontend.cpp -o bin/debug_frontend

dbg_uciws: src/debug_uciws.cpp 
	$(CC) $(CFLAGS) $(INCLUDES) src/server.cpp src/uciws.cpp src/debug_uciws.cpp -o bin/debug_uciws

dbg_board: src/debug_board.cpp 
	$(CC) $(CFLAGS) $(INCLUDES) src/board.cpp src/debug_board.cpp -o bin/debug_board

all: server client

server: src/server.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $< -o bin/server

client: src/client.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $< -o bin/client

clean:
	rm bin/*
