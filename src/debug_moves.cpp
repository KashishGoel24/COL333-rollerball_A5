#include <iostream>
#include <vector>

#include "board.hpp"
#include "butils.hpp"
#include "constants.hpp"

void print_white_piece_positions(const BoardData& b) {

    std::string piece_map[] = {
          "w_rook_1",
          "w_rook_2",
          "w_king",
          "w_bishop",
          "w_knight_1",
          "w_knight_2",
          "w_pawn_1",
          "w_pawn_2",
          "w_pawn_3",
          "w_pawn_4"
    };


    U8 *pieces = (U8*)(&b);
    for (int i=0; i<b.n_pieces; i++) {
        U8 pos = pieces[i];
        if (pieces[i] == DEAD) {
            continue;
        }
        std::cout << piece_map[i] << " (" << piece_to_char(b.board_0[pos]) << ") = " << move_to_str(move(pos,pos)).substr(0,2) << std::endl;
    }
}

int main(int argc, char** argv) {

    // U8* board_mask = (U8*)board_8_; 

    BoardData d;

    // Board b(EIGHT_TWO);

    d.board_type = SEVEN_THREE;
    d.set_7x7_transforms();
    d.board_mask = (U8*)board_7_3;
    d.b_pawn_1 = pos(3,1);
    d.pawn_promo_squares[0] = pos(2,1);
    d.pawn_promo_squares[1] = pos(2,0);
    d.n_pawn_promo_squares = 2;

    d.w_pawn_1 = DEAD;
    d.w_pawn_2 = DEAD; // pos(1,2);
    d.w_king   = pos(3,1);
    d.w_bishop = DEAD; // pos(3,0);
    d.w_rook_1 = pos(3,0);
    d.w_rook_2 = DEAD; // pos(4,0);

    d.b_pawn_1 = DEAD;
    d.b_pawn_2 = DEAD; // pos(5,5);
    d.b_king   = pos(1,2);
    d.b_bishop = DEAD; // pos(3,6);
    d.b_rook_1 = DEAD;
    d.b_rook_2 = DEAD; // pos(3,0);

    d.player_to_play = BLACK;
    d.set_pieces_on_board();

    Board b(d);
    std::cout << board_to_str(&b.data) << "\n";
    // print_white_piece_positions(b.data);

    std::unordered_set<U16> psl_moves;
    std::string moves;

    psl_moves = b.get_legal_moves();
    for (auto move : psl_moves) {
        std::cout << move_to_str(move) << " ";
    }
    std::cout << std::endl;

    // b.do_move_without_flip_(move_promo(pos(3,5),pos(4,5),PAWN_BISHOP));
    // std::cout << board_to_str(&b.data) << "\n";
    // std::cout << all_boards_to_str(b) << "\n";
    // print_white_piece_positions(b.data);

    // b.undo_last_move_without_flip_(move_promo(pos(3,5),pos(4,5),PAWN_BISHOP));
    // std::cout << board_to_str(&b.data) << "\n";
    // std::cout << all_boards_to_str(b) << "\n";
    // print_white_piece_positions(b.data);

    // psl_moves = b.get_legal_moves();
    // for (auto move : psl_moves) {
    //     std::cout << move_to_str(move) << " ";
    // }
    // std::cout << std::endl;
    // moves = show_moves(&b.data, psl_moves);

    // std::cout << moves << "\n";

    return 0;
}

void print_single_piece_moves() {

    U8* board_mask = (U8*)board_7_3; 

    for (int i=0; i<64; i++) {

        if (board_mask[i] == 1) continue;
        BoardData *d = new BoardData();

        d->board_type = SEVEN_THREE;
        d->set_7x7_transforms();
        d->board_mask = (U8*)board_7_3;
        d->b_pawn_1 = (U8)i;
        d->player_to_play = BLACK;
        d->set_pieces_on_board();

        Board b(*d);

        std::unordered_set<U16> psl_moves = b.get_legal_moves();
        for (auto move : psl_moves) {
            std::cout << move_to_str(move) << " ";
        }
        std::cout << std::endl;
        std::string moves = show_moves(&b.data, psl_moves);

        std::cout << moves << "\n";

        delete d;
    }
}
