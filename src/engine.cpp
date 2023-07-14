#include "board.hpp"
#include "engine.hpp"

void Engine::find_best_move(const Board& b) {

    // naively pick the best move
    this->best_move = *(b.get_valid_moves().begin());
}
