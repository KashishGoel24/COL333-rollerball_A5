#include <algorithm>
#include <random>

#include "board.hpp"
#include "engine.hpp"

void Engine::find_best_move(const Board& b) {

    // naively pick the first move
    auto moveset = b.get_valid_moves();
    if (moveset.size() == 0) {
        this->best_move = 0;
    }
    else {
        std::vector<U16> moves;
        std::sample(
            moveset.begin(),
            moveset.end(),
            std::back_inserter(moves),
            1,
            std::mt19937{std::random_device{}()}
        );
        this->best_move = moves[0];
    }
}
