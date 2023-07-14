#pragma once

#include <vector>
#include <unordered_set>

typedef uint8_t U8;
typedef uint16_t U16;

#define pos(x,y) ((y<<3)|x)
#define gety(p)  (p>>3)
#define getx(p)  (p&0x7)

#define move(p0, p1) ((p0<<8)|p1)
#define getp0(m) (m>>8)
#define getp1(m) (m&0xff)

struct MoveDB {

    std::unordered_set<U16> pawn_moves[64];
    std::unordered_set<U16> rook_moves[64];
    std::unordered_set<U16> bishop_moves[64];
    std::unordered_set<U16> king_moves[64];

    MoveDB();
};

struct Move {
    U8 start;
    U8 end;

    char* to_string();
};

enum PlayerColor {
    WHITE=0,
    BLACK=(1<<5)
};

enum PieceType {
    EMPTY  = 0,
    PAWN   = (1<<1),
    ROOK   = (1<<2),
    KING   = (1<<3),
    BISHOP = (1<<4)

};

// bitboard-based representation
struct Board {

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

    U8 board[64];

    PlayerColor player_to_play = WHITE;

    MoveDB *moves = nullptr;

    Board(MoveDB *moves);

    std::unordered_set<U16> get_valid_moves();
    std::unordered_set<U16> get_moves_for_piece(U8 piece_pos);
    void do_move(U16 move, PieceType promotion = BISHOP);
};

class Game {

    private:


    public:
    Move best_move;

    Game(PlayerColor c);

    std::vector<Move> get_valid_moves();

    void search_best_move();

};
