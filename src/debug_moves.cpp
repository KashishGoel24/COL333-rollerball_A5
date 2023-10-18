#include <iostream>
#include <vector>

#include "board.hpp"
#include "butils.hpp"

int main(int argc, char** argv) {

    U8 bottom_squares[10] = {1,2,3,4,5,6,10,11,12,13};

    for (int p : bottom_squares) {
        BoardData *d = new BoardData();

        d->set_7x7_transforms();
        d->board_mask = (U8*)board_7_3;
        d->w_king = p;
        d->set_pieces_on_board();

        Board b(*d);

        std::unordered_set<U16> psl_moves = b.get_pseudolegal_moves();
        std::string moves = show_moves(b.data.board_0, psl_moves);

        std::cout << moves << "\n";

        delete d;
    }

    return 0;
}
