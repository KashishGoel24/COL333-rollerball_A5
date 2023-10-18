#include <iostream>
#include <vector>

#include "board.hpp"
#include "butils.hpp"

int main(int argc, char** argv) {

    U8 bottom_squares[15] = {1,2,3,4,5,6,7,10,11,12,13,14,19,20,21};

    for (int p : bottom_squares) {
        BoardData *d = new BoardData();

        d->board_type = EIGHT_TWO;
        d->set_8x8_transforms();
        d->board_mask = (U8*)board_8_2;
        d->w_king = p;
        d->set_pieces_on_board();

        Board b(*d);

        std::unordered_set<U16> psl_moves = b.get_pseudolegal_moves();
        std::string moves = show_moves(&b.data, psl_moves);

        std::cout << moves << "\n";

        delete d;
    }

    return 0;
}
