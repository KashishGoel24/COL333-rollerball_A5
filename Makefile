CC=g++
CFLAGS=-Wall -std=c++17 -O3 -funroll-loops -DASIO_STANDALONE

INCLUDES=-Iinclude #-I/opt/homebrew/opt/openssl@1.1/include/
#LIBPATH=-L/opt/homebrew/opt/openssl@1.1/lib/
#LIBS=-lcrypto -lssl

all: server client

server: src/server.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $< -o bin/server

client: src/client.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $< -o bin/client

clean:
	rm bin/*
