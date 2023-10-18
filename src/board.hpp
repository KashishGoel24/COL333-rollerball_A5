#pragma once

#include <vector>
#include <unordered_set>
#include <stack>
#include "constants.hpp"

enum PlayerColor {
    WHITE=(1<<6),
    BLACK=(1<<5)
};

enum PieceType {
    EMPTY  = 0,
    PAWN   = (1<<1),
    ROOK   = (1<<2),
    KING   = (1<<3),
    BISHOP = (1<<4)

};

enum Promotion {
    PAWN_BISHOP = (1<<6),
    PAWN_ROOK   = (1<<7)
};

enum BoardType {
    SEVEN_THREE = 1,
    EIGHT_FOUR = 2,
    EIGHT_TWO = 3 
};

struct BoardData {

    // DO NOT add any fields above this
    U8 b_rook_ws  = pos(2,5);
    U8 b_rook_bs  = pos(2,6);
    U8 b_king     = pos(3,5);
    U8 b_bishop   = pos(3,6);
    U8 b_pawn_ws  = pos(4,5);
    U8 b_pawn_bs  = pos(4,6);

    U8 w_rook_ws  = pos(4,1);
    U8 w_rook_bs  = pos(4,0);
    U8 w_king     = pos(3,1);
    U8 w_bishop   = pos(3,0);
    U8 w_pawn_ws  = pos(2,1);
    U8 w_pawn_bs  = pos(2,0);

    U8 *black_pieces[6] = {&b_rook_ws, &b_rook_bs, &b_king, &b_bishop, &b_pawn_ws, &b_pawn_bs};
    U8 *white_pieces[6] = {&w_rook_ws, &w_rook_bs, &w_king, &w_bishop, &w_pawn_ws, &w_pawn_bs};
    
    U8 board_0[64];
    U8 board_90[64];
    U8 board_180[64];
    U8 board_270[64];

    BoardType board_type = SEVEN_THREE;
    U8 *board_mask;
    PlayerColor player_to_play = WHITE;
    U8 last_killed_piece = 0;
    int last_killed_piece_idx = -1;

    U8 *board[4] = {board_0, board_90, board_180, board_270};
    U8 *transform_array[4];
    U8 *inverse_transform_array[4];

    U8 pawn_promo_squares[10];
    int n_pawn_promo_squares;

    BoardData(BoardType board_type);

};

struct Board {

    BoardData data;

    Board();

    std::unordered_set<U16> get_legal_moves() const;
    bool in_check() const;
    Board* copy() const;
    void do_move(U16 move);

    private:
    std::unordered_set<U16> _get_pseudolegal_moves() const;
    std::unordered_set<U16> _get_pseudolegal_moves_for_piece(U8 piece_pos) const;
    void _flip_player();
    void _do_move(U16 move);
    bool _under_threat(U8 piece_pos) const;
    void _undo_last_move(U16 move);
    std::unordered_set<U16> _get_pseudolegal_moves_for_side(U8 color) const;
};

std::string move_to_str(U16 move);
U16 str_to_move(std::string move);
std::string board_to_str(const U8 *b);
std::string all_boards_to_str(const Board& b);
char piece_to_char(U8 piece);
