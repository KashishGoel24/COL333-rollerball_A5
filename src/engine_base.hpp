#pragma once

#include "board.hpp"
#include <atomic>

class AbstractEngine {

    public:
    std::atomic<U16> best_move;
    std::atomic<bool> search;

    void find_best_move(const Board& b);
};
