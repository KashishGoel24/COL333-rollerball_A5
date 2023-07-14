#pragma once

#include "board.hpp"

class Engine {

    U16 best_move;

    void find_best_move(const Board& b);
};
