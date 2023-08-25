#include <iostream>
#include <vector>

#include "board.hpp"


int main(int argc, char** argv) {

    Board *b = new Board();

    while (true) {
        std::cout << board_to_str(b->data.board_0) << std::endl;
        
        auto move_set = b->get_legal_moves();
        std::vector<U16> moves(move_set.begin(), move_set.end());

        for (int i=0; i<moves.size(); i++) {
            std::cout << i << ") " << move_to_str(moves[i]) << "\n";
        }
        std::cout << std::endl;

        std::cout << "> ";
        int option;
        std::cin >> option;
        b->do_move(moves[option]);
    }

    return 0;
}
