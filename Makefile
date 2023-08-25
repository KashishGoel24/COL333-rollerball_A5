CC=g++
CFLAGS=-Wall -std=c++17 -O3 -funroll-loops -DASIO_STANDALONE

INCLUDES=-Iinclude #-I/opt/homebrew/opt/openssl@1.1/include/


# Uncomment If throws an error, and not using python3
PYTHON_INCLUDE_PATH=$(shell python -c "import sysconfig; print(sysconfig.get_path('include'))")
LIBRARYPATH=$(shell python -c "import sysconfig; import os;print(os.path.split(os.path.split(sysconfig.get_path('platlib'))[0])[0])")
PYTHON_VERSION=$(shell python -c "import sys; print('python' + sys.version[:3])")
export LIBRARY_PATH=$(LIBRARYPATH):$LIBRARY_PATH


rollerball:
	$(CC) $(CFLAGS) $(INCLUDES) src/server.cpp src/board.cpp src/engine.cpp src/rollerball.cpp src/uciws.cpp -o bin/rollerball
rollerball_py:
	$(CC) $(CFLAGS) $(INCLUDES) -I/path/to/pybind11/include `python3 -m pybind11 --includes` src/board.cpp src/engine.cpp src/bindings.cpp -shared -o BoardModule`python3-config --extension-suffix` -fPIC
	$(CC) $(CFLAGS) $(INCLUDES) src/server.cpp src/board.cpp src/engine_py.cpp src/rollerball.cpp src/uciws.cpp -o bin/rollerball -I$(PYTHON_INCLUDE_PATH) -l$(PYTHON_VERSION) -fPIC
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
