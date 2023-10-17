#pragma once

#include "engine_base.hpp"
#include <atomic>

class Engine : public AbstractEngine {

    public:

    void find_best_move(const Board& b);
};
