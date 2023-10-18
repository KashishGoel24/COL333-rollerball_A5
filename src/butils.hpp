#pragma once

#include <vector>
#include <unordered_set>
#include <stack>
#include "constants.hpp"
#include "board.hpp"

std::string move_to_str(U16 move);
U16 str_to_move(std::string move);
std::string board_to_str(const U8 *b);
std::string show_moves(const U8 *b, std::unordered_set<U16>& moves);
std::string all_boards_to_str(const Board& b);
char piece_to_char(U8 piece);
