#include <string>
#include <iostream>
#include "board.hpp"
#include "constants.hpp"
#include <cstring>

#define color(p) ((PlayerColor)(p & (WHITE | BLACK)))
#define oppcolor(p) ((PlayerColor)(color(p) ^ (WHITE | BLACK)))
#define occupied(b, p, c) (b[(p)] & (c))
#define inboard(b, x, y) (((x) <= 7) && ((x) >= 0) && ((y) <= 7) && ((y) >= 0) && (b[pos((x),(y))] != 1))

std::unordered_set<U16> transform_moves(const std::unordered_set<U16>& moves, const U8 *transform) {

    std::unordered_set<U16> rot_moves;

    for (U16 move : moves) {
        rot_moves.insert(move_promo(transform[getp0(move)], transform[getp1(move)], getpromo(move)));
    }

    return rot_moves;
}

std::unordered_set<U16> construct_rook_moves(const U8 p0, const U8 *board, const U8 *bmask) {

    PlayerColor color = color(board[p0]);
    PlayerColor oppcolor = oppcolor(board[p0]);
    std::unordered_set<U16> rook_moves;

    // right - move one square 
    if (inboard(bmask, getx(p0)+1, gety(p0)) && 
        !occupied(board, p0+pos(1,0), color)) rook_moves.insert(move(p0, p0+pos(1,0)));

    // bottom - move one square 
    if (inboard(bmask, getx(p0), gety(p0)-1) && 
        !occupied(board, p0-pos(0,1), color)) rook_moves.insert(move(p0, p0-pos(0,1)));

    // top - move multiple if left end (forward), move one if right end
    if (inboard(bmask, getx(p0), gety(p0)+1)) {
        if (bmask[p0+pos(0,1)] == 5 && !occupied(board, p0+pos(0,1), color)) {
            // right end 
            rook_moves.insert(move(p0, p0+pos(0,1)));
        }
        else {
            for (int y=1; inboard(bmask, getx(p0), gety(p0)+y); y++) {
                U8 tgt_pos = p0+pos(0,y);
                if (occupied(board, tgt_pos, color)) break;
                
                rook_moves.insert(move(p0, tgt_pos));
                if (occupied(board, tgt_pos, oppcolor)) break;
            }
        }
    }

    // left
    bool blocked = false;
    for (int x=1; inboard(bmask, getx(p0)-x, gety(p0)); x++) {
        U8 tgt_pos = p0-pos(x,0);
        if (occupied(board, tgt_pos, color)) { blocked = true; break; }
        
        rook_moves.insert(move(p0, tgt_pos));
        if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
    }

    // reflect if on outer ring and not blocked yet
    if (!blocked && gety(p0) == 0) {
        for (int y=1; inboard(bmask, 0, y); y++) {
            U8 tgt_pos = pos(0,y);
            if (occupied(board, tgt_pos, color)) break;
            
            rook_moves.insert(move(p0, tgt_pos));
            if (occupied(board, tgt_pos, oppcolor)) break;
        }
    }

    return rook_moves;
}

std::unordered_set<U16> construct_bishop_moves(const U8 p0, const U8 *board, const U8 *bmask) {

    PlayerColor color = color(board[p0]);
    PlayerColor oppcolor = oppcolor(board[p0]);
    std::unordered_set<U16> bishop_moves;

    // top right - move one square 
    if (inboard(bmask, getx(p0)+1, gety(p0)+1) && 
        !occupied(board, p0+pos(1,1), color)) bishop_moves.insert(move(p0, p0+pos(1,1)));

    // bottom right - move one square 
    if (inboard(bmask, getx(p0)+1, gety(p0)-1) && 
        !occupied(board, p0+pos(1,0)-pos(0,1), color)) 
        bishop_moves.insert(move(p0, p0+pos(1,0)-pos(0,1)));

    // top left - move till reflection, then reflect
    bool blocked = false;
    U8 tgt_pos = DEAD;
    for (int s=1; inboard(bmask, getx(p0)-s, gety(p0)+s); s++) {
        U8 tgt_pos = p0-pos(s,0)+pos(0,s);
        if (occupied(board, tgt_pos, color)) { blocked = true; break; }
        
        bishop_moves.insert(move(p0, tgt_pos));
        if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
    }

    // reflect on left / top edge
    if (!blocked) {
        U8 p1 = tgt_pos;
        if (getx(tgt_pos) == 0) {
            // left edge 
            for (int s=1; inboard(bmask, getx(p1)+s, gety(p1)+s); s++) {
                U8 tgt_pos = p1+pos(s,s);
                if (occupied(board, tgt_pos, color)) { blocked = true; break; }
                
                bishop_moves.insert(move(p0, tgt_pos));
                if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
            }
        }
        else {
            // top edge
            for (int s=1; inboard(bmask, getx(p1)-s, gety(p1)-s); s++) {
                U8 tgt_pos = p1-pos(s,s);
                if (occupied(board, tgt_pos, color)) { blocked = true; break; }
                
                bishop_moves.insert(move(p0, tgt_pos));
                if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
            }
        }
    }

    // bottom left - move till reflection, then reflect
    blocked = false;
    tgt_pos = DEAD;
    for (int s=1; inboard(bmask, getx(p0)-s, gety(p0)-s); s++) {
        U8 tgt_pos = p0-pos(s,s);
        if (occupied(board, tgt_pos, color)) { blocked = true; break; }
        
        bishop_moves.insert(move(p0, tgt_pos));
        if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
    }

    // reflect on bottom edge
    if (!blocked) {
        U8 p1 = tgt_pos;
        for (int s=1; inboard(bmask, getx(p1)-s, gety(p1)+s); s++) {
            U8 tgt_pos = p1-pos(s,0)+pos(0,s);
            if (occupied(board, tgt_pos, color)) { blocked = true; break; }
            
            bishop_moves.insert(move(p0, tgt_pos));
            if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
        }
    }

    return bishop_moves;
}

// TODO refactor + generalize
std::unordered_set<U16> construct_bottom_rook_moves_with_board(const U8 p0, const U8* board) {

    int left_rook_reflect[7] = {0, 8, 16, 24, 32, 40, 48};
    PlayerColor color = color(board[p0]);
    std::unordered_set<U16> rook_moves;
    bool refl_blocked = false;

    if (p0 < 8 || p0 == 13) {
        if (!(board[p0+pos(0,1)] & color)) rook_moves.insert(move(p0, p0+pos(0,1))); // top
        if (p0 == 1) { // top continued on the edge
            for (int y = 1; y<=6; y++) {
                U8 p1 = pos(1, y);
                if (board[p1]) {
                    if (board[p1] & color) break;         // our piece
                    else rook_moves.insert(move(p0, p1)); // their piece - capture
                    break;
                }
                else rook_moves.insert(move(p0, p1));
            }
        }
    }
    if (p0 >= 8) {
        if (!(board[p0-pos(0,1)] & color)) rook_moves.insert(move(p0, p0-pos(0,1))); // bottom
    }

    if (p0 != 6) {
        if (!(board[p0+pos(1,0)] & color)) rook_moves.insert(move(p0, p0+pos(1,0))); // right
    }

    for (int x=getx(p0)-1; x>=0; x--) {
        U8 p1 = pos(x, gety(p0));
        if (board[p1]) {
            refl_blocked = true;
            if (board[p1] & color) break;         // our piece
            else rook_moves.insert(move(p0, p1)); // their piece - capture
            break;
        }
        else {
            rook_moves.insert(move(p0, p1));
        }
    }

    if (refl_blocked) return rook_moves;
    
    if (p0 < 8) {
        for (int p1 : left_rook_reflect) {
            if (board[p1]) {
                if (board[p1] & color) break;         // our piece
                else rook_moves.insert(move(p0, p1)); // their piece - capture
                break;
            }
            else {
                rook_moves.insert(move(p0, p1));
            }
        }
    }

    return rook_moves;
}

std::unordered_set<U16> construct_bottom_bishop_moves_with_board(const U8 p0, const U8* board) {

    PlayerColor color = color(board[p0]);
    std::unordered_set<U16> bishop_moves;

    // top right - move back
    if (p0 < 6 || p0 >= 12) {
        if (!(board[p0+pos(0,1)+pos(1,0)] & color)) bishop_moves.insert(move(p0, p0+pos(0,1)+pos(1,0)));
    }
    // bottom right - move back
    if (p0 > 6) {
        if (!(board[p0-pos(0,1)+pos(1,0)] & color)) bishop_moves.insert(move(p0, p0-pos(0,1)+pos(1,0)));
    }

    std::vector<U8> p1s;
    std::vector<U8> p1s_2;

    // top left - forward / reflections
    if (p0 == 1) {
        p1s.push_back(pos(0,1));
        p1s.push_back(pos(1,2));
    }
    else if (p0 == 2) {
        p1s.push_back(pos(1,1));
        p1s.push_back(pos(0,2));
        p1s.push_back(pos(1,3));
    }
    else if (p0 == 3) {
        p1s.push_back(pos(2,1));
        p1s.push_back(pos(1,2));
        p1s.push_back(pos(0,3));
        p1s.push_back(pos(1,4));
        p1s.push_back(pos(2,5));
        p1s.push_back(pos(3,6));
    }
    else if (p0 == 4 || p0 == 5) {
        p1s.push_back(p0+pos(0,1)-pos(1,0));
        p1s.push_back(p0-pos(2,0));
    }
    else if (p0 == 6) {
        p1s.push_back(pos(5,1));
    }
    else if (p0 == 10) {
        p1s_2.push_back(pos(1,0));
        p1s_2.push_back(pos(0,1));

        p1s.push_back(pos(1,2));
        p1s.push_back(pos(0,3));
        p1s.push_back(pos(1,4));
        p1s.push_back(pos(2,5));
        p1s.push_back(pos(3,6));
    }
    else if (p0 == 11) {
        p1s.push_back(pos(2,0));
        p1s.push_back(pos(1,1));
        p1s.push_back(pos(0,2));
    }
    else if (p0 == 12) {
        p1s.push_back(pos(3,0));
        p1s.push_back(pos(2,1));
        p1s.push_back(pos(1,2));
        p1s.push_back(pos(0,3));
    }
    else if (p0 == 13) {
        p1s.push_back(pos(4,0));
        p1s.push_back(pos(3,1));
    }

    for (auto p1 : p1s) {
        if (board[p1]) {
            if (board[p1] & color) break;           // our piece
            else bishop_moves.insert(move(p0, p1)); // their piece - capture
            break;
        }
        else {
            bishop_moves.insert(move(p0, p1));
        }
    }

    for (auto p1 : p1s_2) {
        if (board[p1]) {
            if (board[p1] & color) break;           // our piece
            else bishop_moves.insert(move(p0, p1)); // their piece - capture
            break;
        }
        else {
            bishop_moves.insert(move(p0, p1));
        }
    }

    return bishop_moves;
}

bool can_promote(U8 pos, U8 *promo, int n_promo, bool promote) {
    for (int i=0; i<n_promo; i++) {
        if (promo[i] == pos) return true;
    }
    return false;
}

std::unordered_set<U16> construct_pawn_moves(const U8 p0, const U8 *board, const U8 *bmask,
        U8 *promo, int n_promo, bool promote) {
    
    PlayerColor color = color(board[p0]);
    std::unordered_set<U16> pawn_moves;

    for (int y = gety(p0)-1; y <= gety(p0) + 1; y++) {
        if (!inboard(bmask, getx(p0)-1, y)) continue;
        U8 p1 = pos(getx(p0)-1, y);
        if (!occupied(board, p1, color)) {
            if (can_promote(p1, promo, n_promo, promote)) {
                pawn_moves.insert(move_promo(p0, p1, PAWN_ROOK));
                pawn_moves.insert(move_promo(p0, p1, PAWN_BISHOP));
            }
            else {
                pawn_moves.insert(move(p0, p1));
            }
        }
    }

    return pawn_moves;
}

std::unordered_set<U16> construct_king_moves(const U8 p0, const U8 *board, const U8 *bmask) {

    PlayerColor color = color(board[p0]);
    std::unordered_set<U16> king_moves;

    int x_incrs[8] = {1, 1,  1, 0,  0, -1, -1, -1};
    int y_incrs[8] = {1, 0, -1, 1, -1,  1,  0, -1};

    for (int i=0; i<8; i++) {
        if (!inboard(bmask, getx(p0)+x_incrs[i], gety(p0)+y_incrs[i])) continue;
        U8 p1 = pos(getx(p0)+x_incrs[i], gety(p0)+y_incrs[i]);
        if (!occupied(board, p1, color)) king_moves.insert(move(p0, p1));
    }

    return king_moves;
}

char piece_to_char(U8 piece) {
    char ch = '.';
    if      (piece & PAWN)   ch = 'p';
    else if (piece & ROOK)   ch = 'r';
    else if (piece & BISHOP) ch = 'b';
    else if (piece & KING)   ch = 'k';

    if (piece & WHITE) ch = ch - ('a'-'A');

    return ch;
}

std::string board_to_str(const U8 *board) {

    std::string board_str = ".......\n.......\n..   ..\n..   ..\n..   ..\n.......\n.......\n";

    for (int i=0; i<56; i++) {
        U8 piece = board[i];
        if (board_str[i] == '\n' || board_str[i] == ' ') continue;
        board_str[(48-(i/8)*8) + i%8] = piece_to_char(piece);
    }

    return board_str;
}

std::string player_to_play_to_str(const Board& b) {
    if (b.data.player_to_play == WHITE) {
        return "WHITE";
    }
    else if (b.data.player_to_play == BLACK) {
        return "BLACK";
    }
    else {
        return "UNKNOWN";
    }
}

std::string all_boards_to_str(const Board& b) {

    std::string board_str(256, ' ');
    std::string board_mask = ".......\n.......\n..   ..\n..   ..\n..   ..\n.......\n.......\n";

    const U8 (*boards)[64] = &(b.data.board_0);

    for (int b=0; b<4; b++) {
        for (int i=0; i<56; i++) {
            if (board_mask[i] == '\n' || board_mask[i] == ' ') continue;
            board_str[(224-(i/8)*32) + b*8 + i%8] = piece_to_char(boards[b][i]);
        }
    }

    for (int i=31; i<256; i+=32) {
        board_str[i] = '\n';
    }

    return board_str.substr(32);
}

std::string move_to_str(U16 move) {

    std::string s = "a1a1";
    s[0] += getx(getp0(move));
    s[1] += gety(getp0(move));
    s[2] += getx(getp1(move));
    s[3] += gety(getp1(move));
    if (getpromo(move) & PAWN_BISHOP) {
        s += "b";
    }
    else if (getpromo(move) & PAWN_ROOK) {
        s += "r";
    }

    return s;
}

U16 str_to_move(std::string move) {
    
    U8 x0 = move[0] - 'a';
    U8 y0 = move[1] - '1';
    U8 x1 = move[2] - 'a';
    U8 y1 = move[3] - '1';
    U8 promo = 0;
    if (move.size() > 4) {
        if (move[4] == 'r') promo = PAWN_ROOK;
        else promo = PAWN_BISHOP;
    }

    return move_promo(pos(x0,y0), pos(x1,y1), promo);
}

std::unordered_set<U16> Board::_get_pseudolegal_moves_for_piece(U8 piece_pos) const {

    std::unordered_set<U16> moves;
    U8 piece_id = this->data.board[0][piece_pos];
    int board_idx = data.board_mask[piece_pos] - 2;
    const U8 *transform_arr = this->data.transform_array[board_idx];
    const U8 *inv_transform_arr = this->data.inverse_transform_array[board_idx];
    const U8 *board = this->data.board[board_idx];

    if (piece_id & PAWN) {
        moves = construct_pawn_moves(transform_arr[piece_pos], board, this->data.board_mask,
                (U8*)this->data.pawn_promo_squares, this->data.n_pawn_promo_squares,
                (board_idx==2 && (piece_id & WHITE)) || (board_idx==0 && (piece_id & BLACK)));
    }
    else if (piece_id & ROOK) {
        moves = construct_rook_moves(transform_arr[piece_pos], board, this->data.board_mask);
    }
    else if (piece_id & BISHOP) {
        moves = construct_bishop_moves(transform_arr[piece_pos], board, this->data.board_mask);
    }
    else if (piece_id & KING) {
        moves = construct_king_moves(transform_arr[piece_pos], board, this->data.board_mask);
    }

    moves = transform_moves(moves, inv_transform_arr);

    return moves;
}

void rotate_board(U8 *src, U8 *tgt, const U8 *transform) {

    for (int i=0; i<64; i++) {
        tgt[transform[i]] = src[i];
    }
}

Board::Board(): data{SEVEN_THREE} {}

BoardData::BoardData(BoardType btype) {

    if (btype == SEVEN_THREE) {
        this->b_rook_ws = pos(2,5);
        this->b_rook_bs = pos(2,6);
        this->b_king    = pos(3,5);
        this->b_bishop  = pos(3,6);
        this->b_pawn_ws = pos(4,5);
        this->b_pawn_bs = pos(4,6);

        this->w_rook_ws = pos(4,1);
        this->w_rook_bs = pos(4,0);
        this->w_king    = pos(3,1);
        this->w_bishop  = pos(3,0);
        this->w_pawn_ws = pos(2,1);
        this->w_pawn_bs = pos(2,0);

        this->board_mask = (U8*)board_7_3;
        this->transform_array[0] = (U8*)id_7x7;
        this->transform_array[1] = (U8*)cw_90_7x7;
        this->transform_array[2] = (U8*)cw_180_7x7;
        this->transform_array[3] = (U8*)acw_90_7x7;
        this->inverse_transform_array[0] = (U8*)id_7x7;
        this->inverse_transform_array[1] = (U8*)acw_90_7x7;
        this->inverse_transform_array[2] = (U8*)cw_180_7x7;
        this->inverse_transform_array[3] = (U8*)cw_90_7x7;
    }
    else {
        this->transform_array[0] = (U8*)id_8x8;
        this->transform_array[1] = (U8*)cw_90_8x8;
        this->transform_array[2] = (U8*)cw_180_8x8;
        this->transform_array[3] = (U8*)acw_90_8x8;
        this->inverse_transform_array[0] = (U8*)id_8x8;
        this->inverse_transform_array[1] = (U8*)acw_90_8x8;
        this->inverse_transform_array[2] = (U8*)cw_180_8x8;
        this->inverse_transform_array[3] = (U8*)cw_90_8x8;

        if (btype == EIGHT_FOUR) {
            // TODO change layout
            this->b_rook_ws = pos(2,5);
            this->b_rook_bs = pos(2,6);
            this->b_king    = pos(3,5);
            this->b_bishop  = pos(3,6);
            this->b_pawn_ws = pos(4,5);
            this->b_pawn_bs = pos(4,6);

            this->w_rook_ws = pos(4,1);
            this->w_rook_bs = pos(4,0);
            this->w_king    = pos(3,1);
            this->w_bishop  = pos(3,0);
            this->w_pawn_ws = pos(2,1);
            this->w_pawn_bs = pos(2,0);

            this->board_mask = (U8*)board_7_3;
        }
        else {
            // TODO change layout
            this->b_rook_ws = pos(2,5);
            this->b_rook_bs = pos(2,6);
            this->b_king    = pos(3,5);
            this->b_bishop  = pos(3,6);
            this->b_pawn_ws = pos(4,5);
            this->b_pawn_bs = pos(4,6);

            this->w_rook_ws = pos(4,1);
            this->w_rook_bs = pos(4,0);
            this->w_king    = pos(3,1);
            this->w_bishop  = pos(3,0);
            this->w_pawn_ws = pos(2,1);
            this->w_pawn_bs = pos(2,0);

            this->board_mask = (U8*)board_7_3;
        }
    }

    this->board_0[this->b_rook_ws]  = BLACK | ROOK;
    this->board_0[this->b_rook_bs]  = BLACK | ROOK;
    this->board_0[this->b_king   ]  = BLACK | KING;
    this->board_0[this->b_bishop ]  = BLACK | BISHOP;
    this->board_0[this->b_pawn_ws]  = BLACK | PAWN;
    this->board_0[this->b_pawn_bs]  = BLACK | PAWN;

    this->board_0[this->w_rook_ws]  = WHITE | ROOK;
    this->board_0[this->w_rook_bs]  = WHITE | ROOK;
    this->board_0[this->w_king   ]  = WHITE | KING;
    this->board_0[this->w_bishop ]  = WHITE | BISHOP;
    this->board_0[this->w_pawn_ws]  = WHITE | PAWN;
    this->board_0[this->w_pawn_bs]  = WHITE | PAWN;

    rotate_board(this->board_0, this->board_90, this->transform_array[1]);
    rotate_board(this->board_0, this->board_180, this->transform_array[2]);
    rotate_board(this->board_0, this->board_270, this->transform_array[3]);
}


// Optimization: generate inverse king moves
// For now, just generate moves of the opposite color and check if any of them
// attack the king square
bool Board::_under_threat(U8 piece_pos) const {

    auto pseudolegal_moves = this->_get_pseudolegal_moves_for_side(this->data.player_to_play ^ (WHITE | BLACK));

    for (auto move : pseudolegal_moves) {
        // std::cout << move_to_str(move) << " ";
        if (getp1(move) == piece_pos) {
            // std::cout << "<- causes check\n";
            return true;
        }
    }
    // std::cout << std::endl;

    return false;
}

bool Board::in_check() const {

    auto king_pos = this->data.w_king;
    // can make this branchless for kicks but won't add much performance
    if (this->data.player_to_play == BLACK) {
        king_pos = this->data.b_king;
    }

    return _under_threat(king_pos);
}

std::unordered_set<U16> Board::_get_pseudolegal_moves() const {
    return _get_pseudolegal_moves_for_side(this->data.player_to_play);
}

std::unordered_set<U16> Board::_get_pseudolegal_moves_for_side(U8 color) const {

    // std::cout << "Getting Pseudolegal moves for " << (char)((color>>5) + 'a') << "\n";
    std::unordered_set<U16> pseudolegal_moves;

    U8 *pieces = (U8*)(&(this->data));

    if (color == WHITE) {
        pieces = pieces + 6;
    }

    for (int i=0; i<6; i++) {
        //std::cout << "checking " << piece_to_char(this->data.board_0[pieces[i]]) << "\n";
        if (pieces[i] == DEAD) continue;
        //std::cout << "Getting Moves for " << piece_to_char(this->data.board_0[pieces[i]]) << "\n";
        auto piece_moves = this->_get_pseudolegal_moves_for_piece(pieces[i]);
        pseudolegal_moves.insert(piece_moves.begin(), piece_moves.end());
    }

    return pseudolegal_moves;

}

Board* Board::copy() const {

    Board *b = new Board();
    memcpy(&(b->data), &(this->data), sizeof(BoardData));

    return b;
}

// legal move generation:
// if the king is under check: (Possible optimization, as this should be faster)
//     Look for moves that protect the king from check
// else:
//     Get all pseudolegal moves
//     for each pseudolegal move for our color:
//         if doing this move will leave the king in threat from opponent's pieces
//             don't add the move to legal moves
//         else
//             add to legal moves
//
// Only implement the else case for now
std::unordered_set<U16> Board::get_legal_moves() const {

    Board* c = this->copy();
    auto pseudolegal_moves = c->_get_pseudolegal_moves();
    std::unordered_set<U16> legal_moves;

    for (auto move : pseudolegal_moves) {
        c->_do_move(move);

        if (!c->in_check()) {
            legal_moves.insert(move);
        }

        c->_undo_last_move(move);
    }

    delete c;

    return legal_moves;
}

void Board::do_move(U16 move) {
    _do_move(move);
    _flip_player();
}

void Board::_flip_player() {
    this->data.player_to_play = (PlayerColor)(this->data.player_to_play ^ (WHITE | BLACK));
}

void Board::_do_move(U16 move) {

    U8 p0 = getp0(move);
    U8 p1 = getp1(move);
    U8 promo = getpromo(move);

    U8 piecetype = this->data.board_0[p0];
    this->data.last_killed_piece = 0;
    this->data.last_killed_piece_idx = -1;

    // scan and get piece from coord
    U8 *pieces = (U8*)this;
    for (int i=0; i<12; i++) {
        if (pieces[i] == p1) {
            pieces[i] = DEAD;
            this->data.last_killed_piece = this->data.board_0[p1];
            this->data.last_killed_piece_idx = i;
        }
        if (pieces[i] == p0) {
            pieces[i] = p1;
        }
    }

    if (promo == PAWN_ROOK) {
        piecetype = (piecetype & (WHITE | BLACK)) | ROOK;
    }
    else if (promo == PAWN_BISHOP) {
        piecetype = (piecetype & (WHITE | BLACK)) | BISHOP;
    }

    this->data.board_0[p1]           = piecetype;
    this->data.board_90[cw_90_7x7[p1]]   = piecetype;
    this->data.board_180[cw_180_7x7[p1]] = piecetype;
    this->data.board_270[acw_90_7x7[p1]] = piecetype;

    this->data.board_0[p0]           = 0;
    this->data.board_90[cw_90_7x7[p0]]   = 0;
    this->data.board_180[cw_180_7x7[p0]] = 0;
    this->data.board_270[acw_90_7x7[p0]] = 0;

    // std::cout << "Did last move\n";
    // std::cout << all_boards_to_str(*this);
}

void Board::_undo_last_move(U16 move) {

    U8 p0 = getp0(move);
    U8 p1 = getp1(move);
    U8 promo = getpromo(move);

    U8 piecetype = this->data.board_0[p1];
    U8 deadpiece = this->data.last_killed_piece;
    this->data.last_killed_piece = 0;

    // scan and get piece from coord
    U8 *pieces = (U8*)(&(this->data));
    for (int i=0; i<12; i++) {
        if (pieces[i] == p1) {
            pieces[i] = p0;
            break;
        }
    }
    if (this->data.last_killed_piece_idx >= 0) {
        pieces[this->data.last_killed_piece_idx] = p1;
        this->data.last_killed_piece_idx = -1;
    }

    if (promo == PAWN_ROOK) {
        piecetype = ((piecetype & (WHITE | BLACK)) ^ ROOK) | PAWN;
    }
    else if (promo == PAWN_BISHOP) {
        piecetype = ((piecetype & (WHITE | BLACK)) ^ BISHOP) | PAWN;
    }

    this->data.board_0[p0]           = piecetype;
    this->data.board_90[cw_90_7x7[p0]]   = piecetype;
    this->data.board_180[cw_180_7x7[p0]] = piecetype;
    this->data.board_270[acw_90_7x7[p0]] = piecetype;

    this->data.board_0[p1]           = deadpiece;
    this->data.board_90[cw_90_7x7[p1]]   = deadpiece;
    this->data.board_180[cw_180_7x7[p1]] = deadpiece;
    this->data.board_270[acw_90_7x7[p1]] = deadpiece;

    // std::cout << "Undid last move\n";
    // std::cout << all_boards_to_str(*this);
}
