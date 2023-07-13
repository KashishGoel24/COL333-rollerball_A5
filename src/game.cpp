#include "game.hpp"

U8 cw_90[64] = {
    48, 40, 32, 24, 16, 8,  0,  7,
    49, 41, 33, 25, 17, 9,  1,  15,
    50, 42, 18, 19, 20, 10, 2,  23,
    51, 43, 26, 27, 28, 11, 3,  31,
    52, 44, 34, 35, 36, 12, 4,  39,
    53, 45, 37, 29, 21, 13, 5,  47,
    54, 46, 38, 30, 22, 14, 6,  55,
    56, 57, 58, 59, 60, 61, 62, 63
};

#define cw_90_pos(p) cw_90[p] // consistency
#define cw_90_move(m) move(cw_90[getp0(m)], cw_90[getp1(m)])

std::unordered_set<U16> cw_90_moves(const std::unordered_set<U16>& moves) {

    std::unordered_set<U16> rot_moves;

    for (U16 move : moves) {
        rot_moves.insert(cw_90_move(move));
    }

    return rot_moves;
}

std::unordered_set<U16> construct_bottom_rook_moves(U8 p0) {

    U8 left_rook_reflect[7] = {0, 8, 16, 24, 32, 40, 48};
    std::unordered_set<U16> rook_moves;

    if (p0 < 8) {
        for (int p1 : left_rook_reflect) rook_moves.insert(move(p0, p1)); // reflect
        rook_moves.insert(move(p0, p0+pos(0,1))); // top
    }
    else {
        rook_moves.insert(move(p0, p0-pos(0,1))); // bottom
        if (p0 == 13) rook_moves.insert(move(p0, p0+pos(0,1))); // top
    }
    if (p0 != 6) {
        rook_moves.insert(move(p0, p0+pos(1,0))); // right
    }
    for (U8 x=getx(p0)-1; x>=0; x--) rook_moves.insert(move(p0, pos(x,gety(p0)))); // left

    return rook_moves;
}

std::unordered_set<U16> construct_bottom_pawn_moves(U8 p0) {
    
    std::unordered_set<U16> pawn_moves;
    pawn_moves.insert(move(p0, pos(getx(p0)-1,0)));
    pawn_moves.insert(move(p0, pos(getx(p0)-1,1)));
    if (p0 == 10) pawn_moves.insert(move(p0, 17));

    return pawn_moves;
}

std::unordered_set<U16> construct_bottom_bishop_moves(U8 p0) {
    
    std::unordered_set<U16> bishop_moves;

    // sanest way I see to make this is by bruteforce

    bishop_moves.insert(move(p0, pos(getx(p0)-1,0)));
    bishop_moves.insert(move(p0, pos(getx(p0)-1,1)));
    if (p0 == 10) bishop_moves.insert(move(p0, 17));

    return bishop_moves;
}

std::unordered_set<U16> construct_bottom_king_moves(U8 p0) {
    
    std::unordered_set<U16> king_moves;
    king_moves.insert(move(p0, pos(getx(p0)-1,0)));
    king_moves.insert(move(p0, pos(getx(p0)-1,1)));
    king_moves.insert(move(p0, pos(getx(p0)+1,0)));
    king_moves.insert(move(p0, pos(getx(p0)+1,1)));
    king_moves.insert(move(p0, pos(getx(p0),gety(p0)^1)));

    return king_moves;
}

MoveDB::MoveDB() {

    U8 bottom[10] = {1, 2, 3, 4, 5, 6, 10, 11, 12, 13};

    for (int p0 : bottom) {

        auto rook_moves = construct_bottom_rook_moves(p0);
        auto pawn_moves = construct_bottom_pawn_moves(p0);
        auto bishop_moves = construct_bottom_bishop_moves(p0);
        auto king_moves = construct_bottom_king_moves(p0);

        this->rook_moves[p0] = rook_moves;
        this->rook_moves[cw_90_pos(p0)] = cw_90_moves(rook_moves);
        this->rook_moves[cw_90_pos(cw_90_pos(p0))] = cw_90_moves(cw_90_moves(rook_moves));
        this->rook_moves[cw_90_pos(cw_90_pos(cw_90_pos(p0)))] = cw_90_moves(cw_90_moves(cw_90_moves(rook_moves)));

        this->pawn_moves[p0] = pawn_moves;
        this->pawn_moves[cw_90_pos(p0)] = cw_90_moves(pawn_moves);
        this->pawn_moves[cw_90_pos(cw_90_pos(p0))] = cw_90_moves(cw_90_moves(pawn_moves));
        this->pawn_moves[cw_90_pos(cw_90_pos(cw_90_pos(p0)))] = cw_90_moves(cw_90_moves(cw_90_moves(pawn_moves)));

        this->bishop_moves[p0] = bishop_moves;
        this->bishop_moves[cw_90_pos(p0)] = cw_90_moves(bishop_moves);
        this->bishop_moves[cw_90_pos(cw_90_pos(p0))] = cw_90_moves(cw_90_moves(bishop_moves));
        this->bishop_moves[cw_90_pos(cw_90_pos(cw_90_pos(p0)))] = cw_90_moves(cw_90_moves(cw_90_moves(bishop_moves)));

        this->king_moves[p0] = king_moves;
        this->king_moves[cw_90_pos(p0)] = cw_90_moves(king_moves);
        this->king_moves[cw_90_pos(cw_90_pos(p0))] = cw_90_moves(cw_90_moves(king_moves));
        this->king_moves[cw_90_pos(cw_90_pos(cw_90_pos(p0)))] = cw_90_moves(cw_90_moves(cw_90_moves(king_moves)));
    }
}

std::unordered_set<U16> Board::get_moves_for_piece(U8 piece_pos) {

    std::unordered_set<U16> moves;
    U8 piece_id = this->board[piece_pos];

    if (piece_id & PAWN) {
        moves = this->moves->pawn_moves[piece_pos];
        for (auto it = moves.begin(); it != moves.end(); ) {
            // same-color pieces - erase move
            if (this->board[getp1(*it)] && ~(this->board[getp1(*it)] ^ piece_id) & BLACK) moves.erase(it);
            else it++;
        }
    }
    else if (piece_id & ROOK) {
        moves = this->moves->rook_moves[piece_pos];
        std::unordered_set<U16> dirty_moves;

        // TODO occupancy checks
        for (auto it = moves.begin(); it != moves.end(); ) {
            // same-color pieces - erase move
            if (this->board[getp1(*it)] && ~(this->board[getp1(*it)] ^ piece_id) & BLACK) moves.erase(it);
            else it++;
        }
    }
    else if (piece_id & BISHOP) {
        moves = this->moves->bishop_moves[piece_pos];
        std::unordered_set<U16> dirty_moves;

        // TODO occupancy checks
        for (auto it = moves.begin(); it != moves.end(); ) {
            // same-color pieces - erase move
            if (this->board[getp1(*it)] && ~(this->board[getp1(*it)] ^ piece_id) & BLACK) moves.erase(it);
            else it++;
        }
    }
    else if (piece_id & KING) {
        moves = this->moves->king_moves[piece_pos];
        std::unordered_set<U16> dirty_moves;

        // TODO occupancy checks
        for (auto it = moves.begin(); it != moves.end(); ) {
            // same-color pieces - erase move
            if (this->board[getp1(*it)] && ~(this->board[getp1(*it)] ^ piece_id) & BLACK) moves.erase(it);
            else it++;
        }
    }

    return moves;
}

bool in_threat(U8 piece) {

    // check all possible directions from which check may arise, 
    // then see if there's an enemy piece providing check in that direction
    //
    return false;

}

std::unordered_set<U16> Board::get_valid_moves() {

    std::unordered_set<U16> moves;

    U8 *pieces = nullptr;

    // TODO king in check logic
    if (this->player_to_play == WHITE) {
        U8 *pieces = ((U8*)this) + 6;
    }
    else {
        U8 *pieces = (U8*)this;
    }

    for (int i=0; i<6; i++) {
        auto piece_moves = this->get_moves_for_piece(pieces[i]);
        moves.insert(piece_moves.begin(), piece_moves.end());
    }

    return moves;

}

void Board::do_move(U16 move, PieceType promotion) {

    U8 p0 = getp0(move);
    U8 p1 = getp1(move);

    U8 piecetype = this->board[p0];

    // scan and get piece from coord
    U8 *pieces = (U8*)this;
    for (int i=0; i<12; i++) {
        if (pieces[i] == p1) {
            pieces[i] = pos(7,7); // dead
        }
        if (pieces[i] == p0) {
            pieces[i] = p1;
        }
    }

    this->board[p1] = piecetype;
    // Pawn promotion
    if (piecetype == PAWN && (
                ((piecetype&BLACK) && gety(p1)==0) ||
                (((~piecetype)&BLACK) && gety(p1)==6)
        )) {
        this->board[p1] = (piecetype&BLACK) | promotion;
    }
}
