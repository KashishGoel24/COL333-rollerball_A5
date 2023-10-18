CC=g++
CFLAGS=-Wall -std=c++17 -O3 -funroll-loops -DASIO_STANDALONE

INCLUDES=-Iinclude

rollerball:
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDES) src/server.cpp src/board.cpp src/engine.cpp src/rollerball.cpp src/uciws.cpp -lpthread -o bin/rollerball

package:
	mkdir -p build
	rm -rf build/*
	mkdir build/rollerball build/rollerball/src
	cp -r include build/rollerball/include
	cp src/*.hpp build/rollerball/src/
	cp src/board.cpp src/bindings.cpp src/engine.cpp src/engine_py.cpp src/rollerball.cpp src/server.cpp src/uciws.cpp build/rollerball/src/
	cp -r scripts build/rollerball/scripts
	cp engine.py setup.py build/rollerball/
	cp Makefile build/rollerball/
	cd web && npm run build
	cp -r web/dist build/rollerball/web
	cd build && zip -r rollerball.zip rollerball

dbg_frontend: src/debug_frontend.cpp 
	$(CC) $(CFLAGS) $(INCLUDES) src/server.cpp src/debug_frontend.cpp -o bin/debug_frontend

dbg_uciws: src/debug_uciws.cpp 
	$(CC) $(CFLAGS) $(INCLUDES) src/server.cpp src/uciws.cpp src/debug_uciws.cpp -o bin/debug_uciws

dbg_board: src/debug_board.cpp 
	$(CC) $(CFLAGS) $(INCLUDES) src/board.cpp src/debug_board.cpp -o bin/debug_board

dbg_moves: src/debug_moves.cpp 
	$(CC) $(CFLAGS) $(INCLUDES) src/bdata.cpp src/butils.cpp src/board.cpp src/debug_moves.cpp -o bin/dbg_moves

clean:
	rm bin/*
