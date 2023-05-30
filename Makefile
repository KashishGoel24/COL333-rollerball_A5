CC=g++
CFLAGS=-Wall -std=c++17 -O3 -funroll-loops -DASIO_STANDALONE

INCLUDES=-Iinclude

all: src/main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $< -o bin/main

clean:
	rm bin/*
