#include <algorithm>
#include <random>
#include <iostream>
#include <climits>
#include <fstream>
#include <chrono>

// #include <bits/stdc++.h>
#include "board.hpp"
#include "engine.hpp"
// #define CUTOFF 4
// #define TIME_LIMIT 2000

// std::map<std::string,int> positions;
Engine *e;

#define vec_print(v) {std::cout<<#v<<" : " ;for(int rnd_i = 0 ; rnd_i < v.size() ; rnd_i ++) {std::cout<<v[rnd_i]<<" " ;} std::cout<<std::endl  ;}
#define vec_print_no_name(v) {for(int rnd_i = 0 ; rnd_i < v.size() ; rnd_i ++) {std::cout<<v[rnd_i]<<" " ;} std::cout<<std::endl  ;}
#define what_is(x) {std::cout<<#x<<" is "<<x<<std::endl ;}
#define check_here(x) {std::cout<<"Here it works "<<x<<std::endl ;}
PlayerColor color, opp_color;
int nodes = 0;
// int total_moves = 0;
int base_eval;
bool repetition = false;
std::unordered_map<std::string, int> BoardPositions;
std::pair<int,U16> max_value(const Board& b, int counter, int alpha, int beta, double time_each2);
std::pair<int,U16> min_value(const Board& b, int counter, int alpha, int beta, double time_each2);
float W0 = 100.0; 
float W1 = 40.0;
float W2 = 10.0; 
float W3 = 3.0;   
float W4 = 4.0;
float W5 = 2.0;
float W6 = 2.0;
auto start = std::chrono::high_resolution_clock::now();
int depth;
int num_iterations_7_3 = 1000;
int num_iterations_8_4 = 750;
int num_iterations_8_2 = 500;
int num_iterations ;
double total_time_7_3 = 120000.0;
double time_7_3 = 4000.0 ;
double total_time_8_4 = 180000.0;
double time_8_4 = 5000.0 ;
double total_time_8_2 = 240000.0;
double time_8_2 = 6000.0;
double std_dev_7_3 = 29000.0;
double std_dev_8_4 = 40000.0;
double std_dev_8_2 = 55000.0;
double std_dev;
double avg_time_left_7_3 = 35000.0;
double avg_time_left_8_4 = 45000.0;
double avg_time_left_8_2 = 50000.0;
double avg_time_left;
double param_7_3 = 0.5;
double param_8_4 = 0.4;
double param_8_2 = 0.3;
double param;
float average_7_3 = 10.0;
float average_8_4 = 10.0;
float average_8_2 = 10.0;
int time_each  ;
int total_moves = 0;
double total_time = 0;
int average_moves ;

float pieceAdvantage(const Board& b){
    // return 0;
    // if (boardType == SEVEN_THREE){
        U8 *pieces = (U8*)(&(b.data));
        U8 *opp_pieces = (U8*)(&(b.data));
        if (color == BLACK) {
            pieces = pieces + 10;
        }
        else {
            opp_pieces = opp_pieces + 10;
        }
        // int score = 0, check_score = 0, piece_adv = 0, piece_threat = 0, pawn_promotion = 0, pawn_promotion2 = 0, pawn_together = 0, rook_boun = 0;
        float piece_adv = 0;
        // piece advantage
        if (pieces[0] != DEAD) piece_adv +=5;
        if (pieces[1] != DEAD) piece_adv +=5;
        if (pieces[3] != DEAD) piece_adv +=3;
        if (pieces[4] != DEAD) piece_adv += 3;
        if (pieces[5] != DEAD) piece_adv += 3;
        if (pieces[6] != DEAD) {
            if (b.data.board_0[pieces[6]] & BISHOP){
                // piece_adv+=3;
                if (pieces[3] != DEAD && ((getx(pieces[3]) + gety(pieces[3]))%2 == 0) && ((getx(pieces[6]) + gety(pieces[6]))%2 != 0)) {piece_adv += 3;}
                else if (pieces[3] != DEAD && ((getx(pieces[3]) + gety(pieces[3]))%2 != 0) && ((getx(pieces[6]) + gety(pieces[6]))%2 == 0)) {piece_adv += 3;}
                else {piece_adv += 2;}
            } 
            else if (b.data.board_0[pieces[6]] & ROOK) piece_adv+=5;
            else piece_adv+=1;
        }
        if (pieces[7] != DEAD) {
            // if (b.data.board_0[pieces[7]] & BISHOP) piece_adv+=3;
            if (b.data.board_0[pieces[7]] & BISHOP){
                // piece_adv+=3;
                if (pieces[3] != DEAD && ((getx(pieces[3]) + gety(pieces[3]))%2 == 0) && ((getx(pieces[7]) + gety(pieces[7]))%2 != 0)) {piece_adv += 3;}
                else if (pieces[3] != DEAD && ((getx(pieces[3]) + gety(pieces[3]))%2 != 0) && ((getx(pieces[7]) + gety(pieces[7]))%2 == 0)) {piece_adv += 3;}
                else {piece_adv += 2;}
            }
            else if (b.data.board_0[pieces[7]] & ROOK) piece_adv+=5;
            else piece_adv+=1;
        }
        if (pieces[8] != DEAD) {
            // if (b.data.board_0[pieces[8]] & BISHOP) piece_adv+=3;
            if (b.data.board_0[pieces[8]] & BISHOP){
                // piece_adv+=3;
                if (pieces[3] != DEAD && ((getx(pieces[3]) + gety(pieces[3]))%2 == 0) && ((getx(pieces[8]) + gety(pieces[8]))%2 != 0)) {piece_adv += 3;}
                else if (pieces[3] != DEAD && ((getx(pieces[3]) + gety(pieces[3]))%2 != 0) && ((getx(pieces[8]) + gety(pieces[8]))%2 == 0)) {piece_adv += 3;}
                else {piece_adv += 2;}
            }
            else if (b.data.board_0[pieces[8]] & ROOK) piece_adv+=5;
            else piece_adv+=1;
        }
        if (pieces[9] != DEAD) {
            // if (b.data.board_0[pieces[9]] & BISHOP) piece_adv+=3;
            if (b.data.board_0[pieces[6]] & BISHOP){
                // piece_adv+=3;
                if (pieces[3] != DEAD && ((getx(pieces[3]) + gety(pieces[3]))%2 == 0) && ((getx(pieces[9]) + gety(pieces[9]))%2 != 0)) {piece_adv += 3;}
                else if (pieces[3] != DEAD && ((getx(pieces[3]) + gety(pieces[3]))%2 != 0) && ((getx(pieces[9]) + gety(pieces[9]))%2 == 0)) {piece_adv += 3;}
                else {piece_adv += 2;}
            }
            else if (b.data.board_0[pieces[9]] & ROOK) piece_adv+=5;
            else piece_adv+=1;
        }
        if (opp_pieces[0] != DEAD) piece_adv -=5;
        if (opp_pieces[1] != DEAD) piece_adv -=5;
        if (opp_pieces[3] != DEAD) piece_adv -=3;
        if (opp_pieces[4] != DEAD) piece_adv -= 3;
        if (opp_pieces[5] != DEAD) piece_adv -= 3;
        if (opp_pieces[6] != DEAD) {
            // if (b.data.board_0[opp_pieces[6]] & BISHOP) piece_adv-=3;
            if (b.data.board_0[opp_pieces[6]] & BISHOP){
                // piece_adv+=3;
                if (opp_pieces[3] != DEAD && ((getx(opp_pieces[3]) + gety(opp_pieces[3]))%2 == 0) && ((getx(opp_pieces[6]) + gety(opp_pieces[6]))%2 != 0)) {piece_adv -= 3;}
                else if (opp_pieces[3] != DEAD && ((getx(opp_pieces[3]) + gety(opp_pieces[3]))%2 != 0) && ((getx(opp_pieces[6]) + gety(opp_pieces[6]))%2 == 0)) {piece_adv -= 3;}
                else {piece_adv -= 2;}
            } 
            else if (b.data.board_0[opp_pieces[6]] & ROOK) piece_adv-=5;
            else piece_adv-=1;
        }
        if (opp_pieces[7] != DEAD) {
            // if (b.data.board_0[opp_pieces[7]] & BISHOP) piece_adv-=3;
            if (b.data.board_0[opp_pieces[7]] & BISHOP){
                // piece_adv+=3;
                if (opp_pieces[3] != DEAD && ((getx(opp_pieces[3]) + gety(opp_pieces[3]))%2 == 0) && ((getx(opp_pieces[7]) + gety(opp_pieces[7]))%2 != 0)) {piece_adv -= 3;}
                else if (opp_pieces[3] != DEAD && ((getx(opp_pieces[3]) + gety(opp_pieces[3]))%2 != 0) && ((getx(opp_pieces[7]) + gety(opp_pieces[7]))%2 == 0)) {piece_adv -= 3;}
                else {piece_adv -= 2;}
            } 
            else if (b.data.board_0[opp_pieces[7]] & ROOK) piece_adv-=5;
            else piece_adv-=1;
        }
        if (opp_pieces[8] != DEAD) {
            // if (b.data.board_0[opp_pieces[8]] & BISHOP) piece_adv-=3;
            if (b.data.board_0[opp_pieces[8]] & BISHOP){
                // piece_adv+=3;
                if (opp_pieces[3] != DEAD && ((getx(opp_pieces[3]) + gety(opp_pieces[3]))%2 == 0) && ((getx(opp_pieces[8]) + gety(opp_pieces[8]))%2 != 0)) {piece_adv -= 3;}
                else if (opp_pieces[3] != DEAD && ((getx(opp_pieces[3]) + gety(opp_pieces[3]))%2 != 0) && ((getx(opp_pieces[8]) + gety(opp_pieces[8]))%2 == 0)) {piece_adv -= 3;}
                else {piece_adv -= 2;}
            } 
            else if (b.data.board_0[opp_pieces[8]] & ROOK) piece_adv-=5;
            else piece_adv-=1;
        }
        if (opp_pieces[9] != DEAD) {
            // if (b.data.board_0[opp_pieces[9]] & BISHOP) piece_adv-=3;
            if (b.data.board_0[opp_pieces[9]] & BISHOP){
                // piece_adv+=3;
                if (opp_pieces[3] != DEAD && ((getx(opp_pieces[3]) + gety(opp_pieces[3]))%2 == 0) && ((getx(opp_pieces[9]) + gety(opp_pieces[9]))%2 != 0)) {piece_adv -= 3;}
                else if (opp_pieces[3] != DEAD && ((getx(opp_pieces[3]) + gety(opp_pieces[3]))%2 != 0) && ((getx(opp_pieces[9]) + gety(opp_pieces[9]))%2 == 0)) {piece_adv -= 3;}
                else {piece_adv -= 2;}
            } 
            else if (b.data.board_0[opp_pieces[9]] & ROOK) piece_adv-=5;
            else piece_adv-=1;
        }

    return piece_adv;
}

float pieceThreat(const Board& b){
    // return 0;
    U8 *pieces = (U8*)(&(b.data));
    U8 *opp_pieces = (U8*)(&(b.data));
    if (color == BLACK) {
        pieces = pieces + 10;
    }
    else {
        opp_pieces = opp_pieces + 10;
    }
    std::unordered_set<U16> moveset, opp_moveset;
    if (color == b.data.player_to_play) {
        moveset = b.get_legal_moves();

        // check_here("printing the moveset inside the piece threat function");
        // for (auto u : moveset){
            // std::cout<<move_to_str(u)<<" ";
        // }
        // std::cout<<std::endl;

        // Board* c = b.copy();
        Board c(b.data);
        c.flip_player_();
        // c.data.player_to_play = (PlayerColor)((b.data.player_to_play)^(WHITE | BLACK));
        opp_moveset = c.get_legal_moves();

        // check_here("printing the opponent moveset inside the piece threat function");
        // for (auto u : opp_moveset){
        //     std::cout<<move_to_str(u)<<" ";
        // }
        // std::cout<<std::endl;

        // delete c;
    }
    else {
        // Board* c = b.copy();
        Board c(b.data);
        c.data.player_to_play = color;
        moveset = c.get_legal_moves();
        // delete c;
        opp_moveset = b.get_legal_moves();
    }

    float piece_threat = 0;

    for (auto m: moveset) {
        U16 p1 = getp1(m);
        if (opp_pieces[0] == p1) {
            // check_here(1);
            piece_threat+=5;
        }
        else if (opp_pieces[1] == p1) {
            // check_here(2);
            piece_threat+=5;
        }
        else if (opp_pieces[3] == p1) {
            // check_here(3);
            piece_threat+=3;
        }
        else if (opp_pieces[4] == p1){
            // check_here(4);
            piece_threat += 3;
        }
        else if (opp_pieces[5] == p1){
            // check_here(5);
            piece_threat += 3;
        }
        else if (opp_pieces[6] == p1) {
            // check_here(6);
            if (b.data.board_0[opp_pieces[6]] & BISHOP) piece_threat+=3;
            else if (b.data.board_0[opp_pieces[6]] & ROOK) piece_threat+=5;
            else piece_threat+=1;
        }
        else if (opp_pieces[7] == p1) {
            // check_here(7);
            if (b.data.board_0[opp_pieces[7]] & BISHOP) piece_threat+=3;
            else if (b.data.board_0[opp_pieces[7]] & ROOK) piece_threat+=5;
            else piece_threat+=1;
        }
        else if (opp_pieces[8] == p1) {
            // check_here(8);
            if (b.data.board_0[opp_pieces[8]] & BISHOP) piece_threat+=3;
            else if (b.data.board_0[opp_pieces[8]] & ROOK) piece_threat+=5;
            else piece_threat+=1;
        }
        else if (opp_pieces[9] == p1) {
            // check_here(9);
            if (b.data.board_0[opp_pieces[9]] & BISHOP) piece_threat+=3;
            else if (b.data.board_0[opp_pieces[9]] & ROOK) piece_threat+=5;
            else piece_threat+=1;
        }
    }
    for (auto m: opp_moveset) {
        U16 p1 = getp1(m);
        if (pieces[0] == p1) {
            // check_here(10);
            piece_threat-=5;
        }
        else if (pieces[1] == p1) {
            // check_here(11);
            piece_threat-=5;
        }
        else if (pieces[3] == p1) {
            // check_here(12);
            piece_threat-=3;
        }
        else if (pieces[4] == p1){
            // check_here(13);
            piece_threat -= 3;
        }
        else if (pieces[5] == p1){
            // check_here(14);
            piece_threat -= 3;
        }
        else if (pieces[6] == p1) {
            // check_here(14);
            if (b.data.board_0[pieces[6]] & BISHOP) piece_threat-=3;
            else if (b.data.board_0[pieces[6]] & ROOK) piece_threat-=5;
            else piece_threat-=1;
        }
        else if (pieces[7] == p1) {
            // check_here(15);
            if (b.data.board_0[pieces[7]] & BISHOP) piece_threat-=3;
            else if (b.data.board_0[pieces[7]] & ROOK) piece_threat-=5;
            else piece_threat-=1;
        }
        else if (pieces[8] == p1) {
            // check_here(16);
            if (b.data.board_0[pieces[8]] & BISHOP) piece_threat-=3;
            else if (b.data.board_0[pieces[8]] & ROOK) piece_threat-=5;
            else piece_threat-=1;
        }
        else if (pieces[9] == p1) {
            // check_here(17);
            if (b.data.board_0[pieces[9]] & BISHOP) piece_threat-=3;
            else if (b.data.board_0[pieces[9]] & ROOK) piece_threat-=5;
            else piece_threat-=1;
        }
    }
    return piece_threat;
}

float check(const Board& b){
    // return 0;
    float check_score = 0.0;
    if ((b.data.player_to_play == WHITE && color == WHITE) || (b.data.player_to_play == BLACK && color == BLACK)){
        if (b.in_check()) {
            if (b.get_legal_moves().size() == 0) {
                // piece_adv-=100 - 2*5*(total_moves/20) - 2*std::min(10,total_moves);
                check_score -= 100;
            }
            else check_score -= 3;
        }
        Board* c = new Board(b);
        c->flip_player_();
        if (c->in_check()) {
            if (c->get_legal_moves().size() == 0) check_score += 100 ;
            else check_score += 3;
        }
    }
    else{
        if (b.in_check()) {
            if (b.get_legal_moves().size() == 0) {
                // piece_adv-=100 ;
                check_score += 100;
            }
            else check_score += 3;
        }
        Board* c = new Board(b);
        c->flip_player_();
        if (c->in_check()) {
            if (c->get_legal_moves().size() == 0) check_score -= 100 ;
            else check_score -= 3;
        }
    }
    return check_score;
}

float pawnPromotion(const Board& b){
    // return 0;
    BoardType boardType = b.data.board_type;
    U8 *pieces = (U8*)(&(b.data));
    U8 *opp_pieces = (U8*)(&(b.data));
    if (color == BLACK) {
        pieces = pieces + 10;}
    else {
        opp_pieces = opp_pieces + 10;}
    float pawn_promotion = 0;
    if (boardType == SEVEN_THREE){
        if (color == WHITE){
            if (pieces[6] != DEAD){
                pawn_promotion -= std::min(abs(5-gety(b.data.w_pawn_1)),abs(6-gety(b.data.w_pawn_1))) + abs(4-getx(b.data.w_pawn_1));
                // what_is(pawn_promotion);
            }
            if (pieces[7] != DEAD){
                pawn_promotion -= std::min(abs(5-gety(b.data.w_pawn_2)),abs(6-gety(b.data.w_pawn_2))) + abs(4-getx(b.data.w_pawn_2));
                // what_is(pawn_promotion);
            }
            if (opp_pieces[6] != DEAD){
                // what_is(std::min(abs(1-gety(b.data.b_pawn_1)),abs(0-gety(b.data.b_pawn_1))));
                // what_is((abs(2-getx(b.data.b_pawn_1))));
                pawn_promotion += std::min(abs(1-gety(b.data.b_pawn_1)),abs(0-gety(b.data.b_pawn_1))) + abs(2-getx(b.data.b_pawn_1));
                // what_is(pawn_promotion);
            }
            if (opp_pieces[7] != DEAD){
                pawn_promotion += std::min(abs(1-gety(b.data.b_pawn_2)),abs(0-gety(b.data.b_pawn_2))) + abs(2-getx(b.data.b_pawn_2));
                // what_is(pawn_promotion);
            }
        }
        else {
            if (opp_pieces[6] != DEAD){
                pawn_promotion += std::min(abs(5-gety(b.data.w_pawn_1)),abs(6-gety(b.data.w_pawn_1))) + abs(4-getx(b.data.w_pawn_1));
                // what_is(pawn_promotion);
            }
            if (opp_pieces[7] != DEAD){
                pawn_promotion += std::min(abs(5-gety(b.data.w_pawn_2)),abs(6-gety(b.data.w_pawn_2))) + abs(4-getx(b.data.w_pawn_2));
                // what_is(pawn_promotion);
            }
            if (pieces[6] != DEAD){
                pawn_promotion -= std::min(abs(1-gety(b.data.b_pawn_1)),abs(0-gety(b.data.b_pawn_1))) + abs(2-getx(b.data.b_pawn_1));
                // what_is(pawn_promotion);
            }
            if (pieces[7] != DEAD){
                pawn_promotion -= std::min(abs(1-gety(b.data.b_pawn_2)),abs(0-gety(b.data.b_pawn_2))) + abs(2-getx(b.data.b_pawn_2));
                // what_is(pawn_promotion);
            }
        }
    }
    else if (boardType == EIGHT_FOUR){
        if (color == WHITE){
            if (pieces[6] != DEAD){
                pawn_promotion -= std::min(abs(7-gety(b.data.w_pawn_1)),abs(6-gety(b.data.w_pawn_1))) + abs(5-getx(b.data.w_pawn_1));
            }
            if (pieces[7] != DEAD){
                pawn_promotion -= std::min(abs(7-gety(b.data.w_pawn_2)),abs(6-gety(b.data.w_pawn_2))) + abs(5-getx(b.data.w_pawn_2));
            }
            if (pieces[8] != DEAD){
                pawn_promotion -= std::min(abs(7-gety(b.data.w_pawn_3)),abs(6-gety(b.data.w_pawn_3))) + abs(5-getx(b.data.w_pawn_3));
            }
            if (pieces[9] != DEAD){
                pawn_promotion -= std::min(abs(7-gety(b.data.w_pawn_4)),abs(6-gety(b.data.w_pawn_4))) + abs(5-getx(b.data.w_pawn_4));
            }
            if (opp_pieces[6] != DEAD){
                pawn_promotion += std::min(abs(1-gety(b.data.b_pawn_1)),abs(0-gety(b.data.b_pawn_1))) + abs(2-getx(b.data.b_pawn_1));
            }
            if (opp_pieces[7] != DEAD){
                pawn_promotion += std::min(abs(1-gety(b.data.b_pawn_2)),abs(0-gety(b.data.b_pawn_2))) + abs(2-getx(b.data.b_pawn_2));
            }
            if (opp_pieces[8] != DEAD){
                pawn_promotion += std::min(abs(1-gety(b.data.b_pawn_3)),abs(0-gety(b.data.b_pawn_3))) + abs(2-getx(b.data.b_pawn_3));
            }
            if (opp_pieces[9] != DEAD){
                pawn_promotion += std::min(abs(1-gety(b.data.b_pawn_4)),abs(0-gety(b.data.b_pawn_4))) + abs(2-getx(b.data.b_pawn_4));
            }
        }
        else {
            if (opp_pieces[6] != DEAD){
                pawn_promotion += std::min(abs(7-gety(b.data.w_pawn_1)),abs(6-gety(b.data.w_pawn_1))) + abs(5-getx(b.data.w_pawn_1));
            }
            if (opp_pieces[7] != DEAD){
                pawn_promotion += std::min(abs(7-gety(b.data.w_pawn_2)),abs(6-gety(b.data.w_pawn_2))) + abs(5-getx(b.data.w_pawn_2));
            }
            if (opp_pieces[8] != DEAD){
                pawn_promotion += std::min(abs(7-gety(b.data.w_pawn_3)),abs(6-gety(b.data.w_pawn_3))) + abs(5-getx(b.data.w_pawn_3));
            }
            if (opp_pieces[9] != DEAD){
                pawn_promotion += std::min(abs(7-gety(b.data.w_pawn_4)),abs(6-gety(b.data.w_pawn_4))) + abs(5-getx(b.data.w_pawn_4));
            }
            if (pieces[6] != DEAD){
                pawn_promotion -= std::min(abs(1-gety(b.data.b_pawn_1)),abs(0-gety(b.data.b_pawn_1))) + abs(2-getx(b.data.b_pawn_1));
            }
            if (pieces[7] != DEAD){
                pawn_promotion -= std::min(abs(1-gety(b.data.b_pawn_2)),abs(0-gety(b.data.b_pawn_2))) + abs(2-getx(b.data.b_pawn_2));
            }
            if (pieces[8] != DEAD){
                pawn_promotion -= std::min(abs(1-gety(b.data.b_pawn_3)),abs(0-gety(b.data.b_pawn_3))) + abs(2-getx(b.data.b_pawn_3));
            }
            if (pieces[9] != DEAD){
                pawn_promotion -= std::min(abs(1-gety(b.data.b_pawn_4)),abs(0-gety(b.data.b_pawn_4))) + abs(2-getx(b.data.b_pawn_4));
            }
        }
    }
    else{
        if (color == WHITE){
            if (pieces[6] != DEAD){
                pawn_promotion -= minm(abs(7-gety(b.data.w_pawn_1)),abs(6-gety(b.data.w_pawn_1)),abs(5-gety(b.data.w_pawn_1))) + abs(5-getx(b.data.w_pawn_1));
            }
            if (pieces[7] != DEAD){
                pawn_promotion -= minm(abs(7-gety(b.data.w_pawn_2)),abs(6-gety(b.data.w_pawn_2)),abs(5-gety(b.data.w_pawn_2))) + abs(5-getx(b.data.w_pawn_2));
            }
            if (pieces[8] != DEAD){
                pawn_promotion -= minm(abs(7-gety(b.data.w_pawn_3)),abs(6-gety(b.data.w_pawn_3)),abs(5-gety(b.data.w_pawn_3))) + abs(5-getx(b.data.w_pawn_3));
            }
            if (pieces[9] != DEAD){
                pawn_promotion -= minm(abs(7-gety(b.data.w_pawn_4)),abs(6-gety(b.data.w_pawn_4)),abs(5-gety(b.data.w_pawn_4))) + abs(5-getx(b.data.w_pawn_4));
            }
            if (opp_pieces[6] != DEAD){
                pawn_promotion += minm(abs(1-gety(b.data.b_pawn_1)),abs(0-gety(b.data.b_pawn_1)),abs(2-gety(b.data.b_pawn_1))) + abs(2-getx(b.data.b_pawn_1));
            }
            if (opp_pieces[7] != DEAD){
                pawn_promotion += minm(abs(1-gety(b.data.b_pawn_2)),abs(0-gety(b.data.b_pawn_2)),abs(2-gety(b.data.b_pawn_2))) + abs(2-getx(b.data.b_pawn_2));
            }
            if (opp_pieces[8] != DEAD){
                pawn_promotion += minm(abs(1-gety(b.data.b_pawn_3)),abs(0-gety(b.data.b_pawn_3)),abs(2-gety(b.data.b_pawn_3))) + abs(2-getx(b.data.b_pawn_3));
            }
            if (opp_pieces[9] != DEAD){
                pawn_promotion += minm(abs(1-gety(b.data.b_pawn_4)),abs(0-gety(b.data.b_pawn_4)),abs(2-gety(b.data.b_pawn_4))) + abs(2-getx(b.data.b_pawn_4));
            }
        }
        else {
            if (opp_pieces[6] != DEAD){
                pawn_promotion += minm(abs(7-gety(b.data.w_pawn_1)),abs(6-gety(b.data.w_pawn_1)),abs(5-gety(b.data.w_pawn_1))) + abs(5-getx(b.data.w_pawn_1));
            }
            if (opp_pieces[7] != DEAD){
                pawn_promotion += minm(abs(7-gety(b.data.w_pawn_2)),abs(6-gety(b.data.w_pawn_2)),abs(5-gety(b.data.w_pawn_2))) + abs(5-getx(b.data.w_pawn_2));
            }
            if (opp_pieces[8] != DEAD){
                pawn_promotion += minm(abs(7-gety(b.data.w_pawn_3)),abs(6-gety(b.data.w_pawn_3)),abs(5-gety(b.data.w_pawn_3))) + abs(5-getx(b.data.w_pawn_3));
            }
            if (opp_pieces[9] != DEAD){
                pawn_promotion += minm(abs(7-gety(b.data.w_pawn_4)),abs(6-gety(b.data.w_pawn_4)),abs(5-gety(b.data.w_pawn_4))) + abs(5-getx(b.data.w_pawn_4));
            }
            if (pieces[6] != DEAD){
                pawn_promotion -= minm(abs(1-gety(b.data.b_pawn_1)),abs(0-gety(b.data.b_pawn_1)),abs(2-gety(b.data.b_pawn_1))) + abs(2-getx(b.data.b_pawn_1));
            }
            if (pieces[7] != DEAD){
                pawn_promotion -= minm(abs(1-gety(b.data.b_pawn_2)),abs(0-gety(b.data.b_pawn_2)),abs(2-gety(b.data.b_pawn_2))) + abs(2-getx(b.data.b_pawn_2));
            }
            if (pieces[8] != DEAD){
                pawn_promotion -= minm(abs(1-gety(b.data.b_pawn_3)),abs(0-gety(b.data.b_pawn_3)),abs(2-gety(b.data.b_pawn_3))) + abs(2-getx(b.data.b_pawn_3));
            }
            if (pieces[9] != DEAD){
                pawn_promotion -= minm(abs(1-gety(b.data.b_pawn_4)),abs(0-gety(b.data.b_pawn_4)),abs(2-gety(b.data.b_pawn_4))) + abs(2-getx(b.data.b_pawn_4));
            }
        }
    }
    // what_is(pawn_promotion);
    return pawn_promotion;
}

float rookOnBoundaries(const Board& b){
    // return 0;
    BoardType boardType = b.data.board_type;
    U8 *pieces = (U8*)(&(b.data));
    U8 *opp_pieces = (U8*)(&(b.data));
    if (b.data.player_to_play == WHITE) {
        pieces = pieces + 10;
    }
    else {
        opp_pieces = opp_pieces + 10;
    }
    float rook_boun = 0;
    if (boardType == SEVEN_THREE){
        if (color == WHITE){
            if ((getx(b.data.w_rook_1) == 0) || (getx(b.data.w_rook_1) == 6) || (gety(b.data.w_rook_1) == 0) || (gety(b.data.w_rook_1) == 6)){
                rook_boun += (10);
            }
            if ((getx(b.data.w_rook_2) == 0) || (getx(b.data.w_rook_2) == 6) || (gety(b.data.w_rook_2) == 0) || (gety(b.data.w_rook_2) == 6)){
                rook_boun += 10;
            }
            if ( (getx(b.data.b_rook_1) == 0) || (getx(b.data.b_rook_1) == 6) || (gety(b.data.b_rook_1) == 0) || (gety(b.data.b_rook_1) == 6)){
                rook_boun -= (10);
            }
            if ((getx(b.data.b_rook_2) == 0) || (getx(b.data.b_rook_2) == 6) || (gety(b.data.b_rook_2) == 0) || (gety(b.data.b_rook_2) == 6)){
                rook_boun -= 10;
            }
        }
        else{
            if ((getx(b.data.b_rook_1) == 0) || (getx(b.data.b_rook_1) == 6) || (gety(b.data.b_rook_1) == 0) || (gety(b.data.b_rook_1) == 6)){
                rook_boun += (10);
            }
            if ((getx(b.data.b_rook_2) == 0) || (getx(b.data.b_rook_2) == 6) || (gety(b.data.b_rook_2) == 0) || (gety(b.data.b_rook_2) == 6)){
                rook_boun += 10;
            }
            if ((getx(b.data.w_rook_1) == 0) || (getx(b.data.w_rook_1) == 6) || (gety(b.data.w_rook_1) == 0) || (gety(b.data.w_rook_1) == 6)){
                rook_boun -= (10);
            }
            if ((getx(b.data.w_rook_2) == 0) || (getx(b.data.w_rook_2) == 6) || (gety(b.data.w_rook_2) == 0) || (gety(b.data.w_rook_2) == 6)){
                rook_boun -= 10;
            }
        }
    }
    else if (boardType == EIGHT_FOUR){
        if (color == WHITE){
            if ((getx(b.data.w_rook_1) == 0) || (getx(b.data.w_rook_1) == 7) || (gety(b.data.w_rook_1) == 0) || (gety(b.data.w_rook_1) == 7)){
                rook_boun += (10);
            }
            if ((getx(b.data.w_rook_2) == 0) || (getx(b.data.w_rook_2) == 7) || (gety(b.data.w_rook_2) == 0) || (gety(b.data.w_rook_2) == 7)){
                rook_boun += 10;
            }
            if ( (getx(b.data.b_rook_1) == 0) || (getx(b.data.b_rook_1) == 7) || (gety(b.data.b_rook_1) == 0) || (gety(b.data.b_rook_1) == 7)){
                rook_boun -= (10);
            }
            if ((getx(b.data.b_rook_2) == 0) || (getx(b.data.b_rook_2) == 7) || (gety(b.data.b_rook_2) == 0) || (gety(b.data.b_rook_2) == 7)){
                rook_boun -= 10;
            }
        }
        else{
            if ((getx(b.data.b_rook_1) == 0) || (getx(b.data.b_rook_1) == 7) || (gety(b.data.b_rook_1) == 0) || (gety(b.data.b_rook_1) == 7)){
                rook_boun += (10);
            }
            if ((getx(b.data.b_rook_2) == 0) || (getx(b.data.b_rook_2) == 7) || (gety(b.data.b_rook_2) == 0) || (gety(b.data.b_rook_2) == 7)){
                rook_boun += 10;
            }
            if ((getx(b.data.w_rook_1) == 0) || (getx(b.data.w_rook_1) == 7) || (gety(b.data.w_rook_1) == 0) || (gety(b.data.w_rook_1) == 7)){
                rook_boun -= (10);
            }
            if ((getx(b.data.w_rook_2) == 0) || (getx(b.data.w_rook_2) == 7) || (gety(b.data.w_rook_2) == 0) || (gety(b.data.w_rook_2) == 7)){
                rook_boun -= 10;
            }
        }
    }
    else {
        if (color == WHITE){
            if ((getx(b.data.w_rook_1) == 0) || (getx(b.data.w_rook_1) == 7) || (gety(b.data.w_rook_1) == 0) || (gety(b.data.w_rook_1) == 7)){
                rook_boun += (10);
            }
            if ((getx(b.data.w_rook_2) == 0) || (getx(b.data.w_rook_2) == 7) || (gety(b.data.w_rook_2) == 0) || (gety(b.data.w_rook_2) == 7)){
                rook_boun += 10;
            }
            if ( (getx(b.data.b_rook_1) == 0) || (getx(b.data.b_rook_1) == 7) || (gety(b.data.b_rook_1) == 0) || (gety(b.data.b_rook_1) == 7)){
                rook_boun -= (10);
            }
            if ((getx(b.data.b_rook_2) == 0) || (getx(b.data.b_rook_2) == 7) || (gety(b.data.b_rook_2) == 0) || (gety(b.data.b_rook_2) == 7)){
                rook_boun -= 10;
            }
        }
        else{
            if ((getx(b.data.b_rook_1) == 0) || (getx(b.data.b_rook_1) == 7) || (gety(b.data.b_rook_1) == 0) || (gety(b.data.b_rook_1) == 7)){
                rook_boun += (10);
            }
            if ((getx(b.data.b_rook_2) == 0) || (getx(b.data.b_rook_2) == 7) || (gety(b.data.b_rook_2) == 0) || (gety(b.data.b_rook_2) == 7)){
                rook_boun += 10;
            }
            if ((getx(b.data.w_rook_1) == 0) || (getx(b.data.w_rook_1) == 7) || (gety(b.data.w_rook_1) == 0) || (gety(b.data.w_rook_1) == 7)){
                rook_boun -= (10);
            }
            if ((getx(b.data.w_rook_2) == 0) || (getx(b.data.w_rook_2) == 7) || (gety(b.data.w_rook_2) == 0) || (gety(b.data.w_rook_2) == 7)){
                rook_boun -= 10;
            }
        }
    }
    return rook_boun;
}

float pawnTogether(const Board& b, bool value){
    // return 0;
    // the boolean varibale value is true if this heuristic is being used in the valuue function else it is false
    U8 *pieces = (U8*)(&(b.data));
    // U8 *opp_pieces = (U8*)(&(b.data));
    if (color == BLACK) {
        pieces = pieces + 10;
    }
    // else {
    //     opp_pieces = opp_pieces + 10;
    // }
    float pawn_together = 0;
    float penalty = 10;
    if (b.data.board_type == SEVEN_THREE){
        if (color == WHITE){
            if ((pieces[6] != DEAD) && (pieces[7] != DEAD)){
                pawn_together -= (abs(getx(b.data.w_pawn_1) - getx(b.data.w_pawn_2)) + abs(gety(b.data.w_pawn_1) - gety(b.data.w_pawn_2)));
            }
            if (gety(b.data.w_pawn_1) == 0){
                // std::cout<<"puttinh penalty1\n";
                pawn_together -= penalty;
            }
            if (gety(b.data.w_pawn_2) == 0){
                // std::cout<<"puttinh penalty2\n";
                pawn_together -= penalty;
            }
            if (getx(b.data.w_pawn_1) == 0 && gety(b.data.w_pawn_1) == 0){
                pawn_together -= penalty;
            }
            if (getx(b.data.w_pawn_2) == 0 && gety(b.data.w_pawn_2) == 0){
                pawn_together -= penalty;
            }
        }
        else{
            if ((pieces[6] != DEAD) && (pieces[7] != DEAD) ){
                pawn_together -= abs(getx(b.data.b_pawn_1) - getx(b.data.b_pawn_2)) + abs(gety(b.data.b_pawn_1) - gety(b.data.b_pawn_2));
            }
            if (gety(b.data.b_pawn_1) == 6){
                // std::cout<<"puttinh penalty3\n";
                pawn_together -= penalty;
            }
            if (gety(b.data.b_pawn_2) == 6){
                // std::cout<<"puttinh penalty4\n";
                pawn_together -= penalty;
            }
            if (getx(b.data.b_pawn_1) == 6 && gety(b.data.b_pawn_1) == 6){
                pawn_together -= penalty;
            }
            if (getx(b.data.b_pawn_2) == 6 && gety(b.data.b_pawn_2) == 6){
                pawn_together -= penalty;
            }
        }
    }
    else if (b.data.board_type == EIGHT_FOUR){
        if (color == WHITE){
            if ((pieces[6] != DEAD) && (pieces[7] != DEAD) ){
                // std::cout<<"haan andar aaya to hoon\n";
                pawn_together -= abs(getx(b.data.w_pawn_1) - getx(b.data.w_pawn_2)) + abs(gety(b.data.w_pawn_1) - gety(b.data.w_pawn_2));
            }
            if ( (pieces[8] != DEAD) && (pieces[9] != DEAD)){
                pawn_together -= abs(getx(b.data.w_pawn_3) - getx(b.data.w_pawn_3)) + abs(gety(b.data.w_pawn_3) - gety(b.data.w_pawn_4));
            }
            if (getx(b.data.w_pawn_1) == 0 && gety(b.data.w_pawn_1) == 0){
                pawn_together -= penalty;
            }
            if (getx(b.data.w_pawn_2) == 0 && gety(b.data.w_pawn_2) == 0){
                pawn_together -= penalty;
            }
            if (getx(b.data.w_pawn_3) == 0 && gety(b.data.w_pawn_3) == 0){
                pawn_together -= penalty;
            }
            if (getx(b.data.w_pawn_4) == 0 && gety(b.data.w_pawn_4) == 0){
                pawn_together -= penalty;
            }
        }
        else{
            if ((pieces[6] != DEAD) && (pieces[7] != DEAD)){
                pawn_together -= abs(getx(b.data.b_pawn_1) - getx(b.data.b_pawn_2)) + abs(gety(b.data.b_pawn_1) - gety(b.data.b_pawn_2));
            }
            if ((pieces[8] != DEAD) && (pieces[9] != DEAD)){
                pawn_together -= abs(getx(b.data.b_pawn_3) - getx(b.data.b_pawn_4)) + abs(gety(b.data.b_pawn_3) - gety(b.data.b_pawn_4));
            }
            if (getx(b.data.b_pawn_1) == 7 && gety(b.data.b_pawn_1) == 7){
                pawn_together -= penalty;
            }
            if (getx(b.data.b_pawn_2) == 7 && gety(b.data.b_pawn_2) == 7){
                pawn_together -= penalty;
            }
            if (getx(b.data.b_pawn_3) == 7 && gety(b.data.b_pawn_3) == 7){
                pawn_together -= penalty;
            }
            if (getx(b.data.b_pawn_4) == 7 && gety(b.data.b_pawn_4) == 7){
                pawn_together -= penalty;
            }
        }
    }
    else{
        if (color == WHITE){
            if ((pieces[6] != DEAD) && (pieces[7] != DEAD)  ){
                pawn_together -= abs(getx(b.data.w_pawn_1) - getx(b.data.w_pawn_2)) + abs(gety(b.data.w_pawn_1) - gety(b.data.w_pawn_2));
            }
            if ((pieces[6] != DEAD) && (pieces[8] != DEAD)  ){
                pawn_together -= abs(getx(b.data.w_pawn_1) - getx(b.data.w_pawn_3)) + abs(gety(b.data.w_pawn_1) - gety(b.data.w_pawn_3));
            }
            if ((pieces[8] != DEAD) && (pieces[7] != DEAD)  ){
                pawn_together -= abs(getx(b.data.w_pawn_3) - getx(b.data.w_pawn_2)) + abs(gety(b.data.w_pawn_3) - gety(b.data.w_pawn_2));
            }
            if (getx(b.data.w_pawn_1) == 0 && gety(b.data.w_pawn_1) == 0){
                pawn_together -= penalty;
            }
            if (getx(b.data.w_pawn_2) == 0 && gety(b.data.w_pawn_2) == 0){
                pawn_together -= penalty;
            }
            if (getx(b.data.w_pawn_3) == 0 && gety(b.data.w_pawn_3) == 0){
                pawn_together -= penalty;
            }
            if (getx(b.data.w_pawn_4) == 0 && gety(b.data.w_pawn_4) == 0){
                pawn_together -= penalty;
            }
        }
        else{
            if ((pieces[6] != DEAD) && (pieces[7] != DEAD)){
                pawn_together -= abs(getx(b.data.b_pawn_1) - getx(b.data.b_pawn_2)) + abs(gety(b.data.b_pawn_1) - gety(b.data.b_pawn_2));
            }
            if ((pieces[6] != DEAD) && (pieces[8] != DEAD)){
                pawn_together -= abs(getx(b.data.b_pawn_1) - getx(b.data.b_pawn_3)) + abs(gety(b.data.b_pawn_1) - gety(b.data.b_pawn_3));
            }
            if ((pieces[8] != DEAD) && (pieces[7] != DEAD)){
                pawn_together -= abs(getx(b.data.b_pawn_3) - getx(b.data.b_pawn_2)) + abs(gety(b.data.b_pawn_3) - gety(b.data.b_pawn_2));
            }
            if (getx(b.data.b_pawn_1) == 7 && gety(b.data.b_pawn_1) == 7){
                pawn_together -= penalty;
            }
            if (getx(b.data.b_pawn_2) == 7 && gety(b.data.b_pawn_2) == 7){
                pawn_together -= penalty;
            }
            if (getx(b.data.b_pawn_3) == 7 && gety(b.data.b_pawn_3) == 7){
                pawn_together -= penalty;
            }
            if (getx(b.data.b_pawn_4) == 7 && gety(b.data.b_pawn_4) == 7){
                pawn_together -= penalty;
            }
        }
    }
    
    
    // if (not value){pawn_together = -pawn_together;}

    return pawn_together;
}

float kingPawnDist(const Board& b){
    // return 0;
    BoardType boardType = b.data.board_type;
    // U8 *pieces = (U8*)(&(b.data));
    // U8 *opp_pieces = (U8*)(&(b.data));
    // if (b.data.player_to_play == WHITE) {
    //     pieces = pieces + 10;
    // }
    // else {
    //     opp_pieces = opp_pieces + 10;
    // }
    float score = 0.0;
    if (boardType == EIGHT_TWO){
        if (color == WHITE){
            if (b.data.w_pawn_4 != DEAD){
                score -= abs(getx(b.data.w_king) - getx(b.data.w_pawn_4)) + abs(gety(b.data.w_king) - gety(b.data.w_pawn_4));
            }
            if (b.data.b_pawn_4 != DEAD){
                score += abs(getx(b.data.b_king) - getx(b.data.b_pawn_4)) + abs(gety(b.data.b_king) - gety(b.data.b_pawn_4));
            }
        }
        else{
            if (b.data.w_pawn_4 != DEAD){
                score += abs(getx(b.data.w_king) - getx(b.data.w_pawn_4)) + abs(gety(b.data.w_king) - gety(b.data.w_pawn_4));
            }
            if (b.data.b_pawn_4 != DEAD){
                score -= abs(getx(b.data.b_king) - getx(b.data.b_pawn_4)) + abs(gety(b.data.b_king) - gety(b.data.b_pawn_4));
            }
        }
        return score;
    }
    else if (boardType == EIGHT_FOUR) {
        if (color == WHITE){
            if (b.data.w_pawn_4 != DEAD){
                score -= abs(getx(b.data.w_king) - getx(b.data.w_pawn_4)) + abs(gety(b.data.w_king) - gety(b.data.w_pawn_4));
            }
            if (b.data.w_pawn_3 != DEAD){
                score -= abs(getx(b.data.w_king) - getx(b.data.w_pawn_3)) + abs(gety(b.data.w_king) - gety(b.data.w_pawn_3));
            }
            if (b.data.b_pawn_4 != DEAD){
                score += abs(getx(b.data.b_king) - getx(b.data.b_pawn_4)) + abs(gety(b.data.b_king) - gety(b.data.b_pawn_4));
            }
            if (b.data.b_pawn_3 != DEAD){
                score += abs(getx(b.data.b_king) - getx(b.data.b_pawn_3)) + abs(gety(b.data.b_king) - gety(b.data.b_pawn_3));
            }
        }
        else{
            if (b.data.w_pawn_4 != DEAD){
                score += abs(getx(b.data.w_king) - getx(b.data.w_pawn_4)) + abs(gety(b.data.w_king) - gety(b.data.w_pawn_4));
            }
            if (b.data.b_pawn_4 != DEAD){
                score -= abs(getx(b.data.b_king) - getx(b.data.b_pawn_4)) + abs(gety(b.data.b_king) - gety(b.data.b_pawn_4));
            }
            if (b.data.w_pawn_3 != DEAD){
                score += abs(getx(b.data.w_king) - getx(b.data.w_pawn_3)) + abs(gety(b.data.w_king) - gety(b.data.w_pawn_3));
            }
            if (b.data.b_pawn_3 != DEAD){
                score -= abs(getx(b.data.b_king) - getx(b.data.b_pawn_3)) + abs(gety(b.data.b_king) - gety(b.data.b_pawn_3));
            }
        }
        return score;
    }
    else{
        if (color == WHITE){
            // if ((getx(b.data.w_king) == 3 && gety(b.data.w_king) == 1) || (getx(b.data.w_king) == 1 && gety(b.data.w_king) == 3) || (getx(b.data.w_king) == 3 && gety(b.data.w_king) == 5) || (getx(b.data.w_king) == 5 && gety(b.data.w_king) == 3)){
            //     score -= 2;
            // }
            // if (getx(b.data.w_king) == getx(b.data.w_rook_1)){
            //     score -= abs(gety(b.data.w_king) - gety(b.data.w_rook_1));
            // }
            // if (getx(b.data.w_king) == getx(b.data.w_rook_2)){
            //     score -= abs(gety(b.data.w_king) - gety(b.data.w_rook_2));
            // }
            // if (gety(b.data.w_king) == gety(b.data.w_rook_1)){
            //     score -= abs(getx(b.data.w_king) - getx(b.data.w_rook_1));
            // }
            // if (gety(b.data.w_king) == gety(b.data.w_rook_1)){
            //     score -= abs(getx(b.data.w_king) - getx(b.data.w_rook_1));
            // if ((getx(b.data.w_bishop) == 1 && gety(b.data.w_bishop) == 0) || (getx(b.data.w_bishop) == 0 && gety(b.data.w_bishop) == 1)){score -= 20;}
            // }
            if (total_moves > 4 && b.data.w_bishop != DEAD){
                if (getx(b.data.w_pawn_1) == 1 && gety(b.data.w_pawn_1) == 2){score -= 5;}
                // if (getx(b.data.w_pawn_2) == 1 && gety(b.data.w_pawn_2) == 2){score -= 10;}
            }
        }
        else{
        //     if ((getx(b.data.b_king) == 3 && gety(b.data.b_king) == 1) || (getx(b.data.b_king) == 1 && gety(b.data.b_king) == 3) || (getx(b.data.b_king) == 3 && gety(b.data.b_king) == 5) || (getx(b.data.b_king) == 5 && gety(b.data.b_king) == 3)){
        //         score -= 2;
        //     }
        //     if (getx(b.data.b_king) == getx(b.data.b_rook_1)){
        //         score -= abs(gety(b.data.b_king) - gety(b.data.b_rook_1));
        //     }
        //     if (getx(b.data.b_king) == getx(b.data.b_rook_2)){
        //         score -= abs(gety(b.data.b_king) - gety(b.data.b_rook_2));
        //     }
        //     if (gety(b.data.b_king) == gety(b.data.b_rook_1)){
        //         score -= abs(getx(b.data.b_king) - getx(b.data.b_rook_1));
        //     }
        //     if (gety(b.data.b_king) == gety(b.data.b_rook_1)){
        //         score -= abs(getx(b.data.b_king) - getx(b.data.b_rook_1));
        //     }

            // if ((getx(b.data.b_bishop) == 5 && gety(b.data.b_bishop) == 6) || (getx(b.data.b_bishop) == 6 && gety(b.data.b_bishop) == 5)){check_here("inside bishop penalty"); score -= 20;}
            if (total_moves > 4 && b.data.b_bishop != DEAD){
                if (getx(b.data.b_pawn_1) == 5 && gety(b.data.b_pawn_1) == 4){score -= 5;}
                // if (getx(b.data.w_pawn_2) == 1 && gety(b.data.w_pawn_2) == 2){score -= 10;}
            }
        }
        return score;
    }
}

int eval(const Board& b, bool no_moves) {
    int v;
    v = W0  *   pieceAdvantage(b) +
        W1  *   pieceThreat(b) +
        W2  *   check(b) +
        W3  *   pawnPromotion(b) +
        W4  *   rookOnBoundaries(b) +
        W5  *   pawnTogether(b, false) +
        W6  *   kingPawnDist(b);
    return v;
    // return 2.0/(1.0+exp(-v/100.0)) - 1.0;
}

std::pair<int,U16> max_value(const Board& b,int counter, int alpha, int beta, double time_each2) {
    // check_here("inside the max_value funcion");
    // what_is(counter);
    // std::cout<<board_to_str(&(b.data))<<std::endl;
    // float time_each2 = float(time_each)*exp(-(pow((total_moves - average_moves)/15.0,2)));
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() >= time_each2) {/*check_here("returning timeout thigny in max "); */return std::make_pair(0,(U16)-1);}
    nodes++;
    if (counter == 0) {
        return std::make_pair(eval(b,false),(U16)0);
    }
    int v = INT_MIN;
    auto moveset = b.get_legal_moves();
    if (moveset.size() == 0) {
        return std::make_pair(eval(b,true),(U16)0);
    }
    else {
        U16 bestmove = *moveset.begin();

        // for (auto u : moveset){
        //     std::cout<<move_to_str(u)<<" ";
        // }
        // std::cout<<std::endl;

        for (auto m : moveset) {

            Board* c = new Board(b);
            c->do_move_(m);

            if (c->in_check() && c->get_legal_moves().size() == 0) {
                // std::cout<<"checkmate\n";
                return std::make_pair(eval(*c,true),m);
            }

            std::pair<int,U16> x = min_value(*c, counter-1,alpha,beta, time_each2);
            

            int value = x.first;

            // if(x.second==(U16)-1) {what_is(x.second); what_is(counter); check_here("returning 0 from max finally");  return std::make_pair(0,0);}
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() >= time_each2) return std::make_pair(0,(U16)-1); 
            // std::cout<<counter<<" max: "<<move_to_str(m)<<": "<<value<<std::endl;         
            // std::cout<<"alpha:"<<alpha<<" "<<x.first<<"\n";
            alpha = std::max(alpha, value);
            // std::cout<<"alpha:"<<alpha<<"\n";
            if (value >= v) {
                v = value;
                bestmove = m;
            }
            if (alpha > beta) {
                // std::cout<<"max pruned:"<<alpha<<" "<<beta<<"\n";
                delete c;
                return std::make_pair(v,bestmove);
            }
            
            delete c;
        }
        // what_is(counter);
        // what_is(move_to_str(bestmove));
        // what_is(v);
        return std::make_pair(v,bestmove);
    } 

}

std::pair<int,U16> min_value(const Board& b, int counter, int alpha, int beta, double time_each2) {
    // check_here("inside the min function ");
    // what_is(counter);
    // std::cout<<board_to_str(&(b.data))<<std::endl;
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() >= time_each2) {/*check_here("returning timeout thigny in min ");*/ return std::make_pair(0,(U16)-1);}
    nodes++;
    if (counter == 0) {
        return std::make_pair(eval(b,false),(U16)0);
    }
    int v = INT_MAX;
    auto moveset = b.get_legal_moves();
    if (moveset.size() == 0) {
        return std::make_pair(eval(b,true),(U16)0);
    }
    else {

        // for (auto u : moveset){
        //     std::cout<<move_to_str(u)<<" ";
        // }
        // std::cout<<std::endl;

        U16 bestmove = *moveset.begin();
        for (auto m : moveset) {

            Board* c = new Board(b);
            c->do_move_(m);

            std::pair<int,U16> x = max_value(*c, counter-1, alpha, beta, time_each2);
            // std::cout<<counter<<" min: "<<x.first<<" "<<move_to_str(m)<<"\n";
            int value = x.first;   
            // if(x.second==(U16)-1) {what_is(x.second); what_is(counter); check_here("returning 0 from min finally"); return std::make_pair(0,0);}
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() >= time_each2) return std::make_pair(0,(U16)-1); 
            // std::cout<<"beta:"<<beta<<" "<<x.first<<"\n";
            beta = std::min(beta,value);
            // std::cout<<"beta:"<<beta<<"\n";c
            if (value <= v) {
                v = value;
                bestmove = m;
            }
            if (beta < alpha) {
                // std::cout<<"min pruned:"<<alpha<<" "<<beta<<"\n";
                delete c;
                // std::cout<<"pruning" << v <<"\n";
                return std::make_pair(v,bestmove);
            }
           
            delete c;
        }
        // std::cout<<"without pruning" << v <<"\n";
        // what_is(counter);
        // what_is(move_to_str(bestmove));
        // what_is(v);
        return std::make_pair(v,bestmove);
    } 

}

void Engine::find_best_move(const Board& b) {

    // std::ifstream inputFile("/Users/kashishgoel/Downloads/COL333_A5-main/src/WEIGHTS");
    if (total_moves == 0){
        if (b.data.board_type == SEVEN_THREE){
            W0 = 100.0; 
            W1 = 40.0;
            W2 = 10.0; 
            W3 = 3.0;   
            W4 = 4.0;
            W5 = 1.0;
            W6 = 3.0;
            // std::ifstream inputFile("/Users/kashishgoel/Downloads/COL333_A5-main/src/WEIGHTS_7_3");
            num_iterations = num_iterations_7_3;
            average_moves = average_7_3;
            time_each = time_7_3;
            total_time = total_time_7_3;
            std_dev = std_dev_7_3;
            avg_time_left = avg_time_left_7_3;
            param = param_7_3;
            
        }
        else if (b.data.board_type == EIGHT_FOUR){
            W0 = 100.0; 
            W1 = 40.0;
            W2 = 10.0; 
            W3 = 3.0;   
            W4 = 4.0;
            W5 = 2.0;
            W6 = 2.0;
            // std::ifstream inputFile("/Users/kashishgoel/Downloads/COL333_A5-main/src/WEIGHTS_8_4");
            num_iterations = num_iterations_8_4;
            average_moves = average_8_4;
            time_each = time_8_4;
            total_time = total_time_8_4;
            std_dev = std_dev_8_4;
            avg_time_left = avg_time_left_8_4;
            param = param_8_4;
        }
        else{
            // std::ifstream inputFile("/Users/kashishgoel/Downloads/COL333_A5-main/src/WEIGHTS_8_3");
            W0 = 100.0; 
            W1 = 40.0;
            W2 = 10.0; 
            W3 = 3.0;   
            W4 = 4.0;
            W5 = 2.0;
            W6 = 5.0;
            num_iterations = num_iterations_8_2;
            average_moves = average_8_2;
            time_each = time_8_2;
            total_time = total_time_8_2;
            std_dev = std_dev_8_2;
            avg_time_left = avg_time_left_8_2;
            param = param_8_2;
        }

        if (b.data.player_to_play == WHITE){
            if (b.data.board_type == SEVEN_THREE){
                this->best_move = str_to_move("c1b2");
                total_moves  += 1;
                return;
            }
            else if (b.data.board_type == EIGHT_FOUR){
                this->best_move = str_to_move("c1b2");
                total_moves  += 1;
                return;
            }
            else{
                this->best_move = str_to_move("d2c4");
                total_moves  += 1;
                return;
            }
        }
    }
    
    e = this;
    total_moves++;
    color = b.data.player_to_play;
    opp_color = (PlayerColor)(color ^ (WHITE | BLACK));
    // int total_depth;

    // if (b.data.board_type == SEVEN_THREE) total_depth = 5;
    // else if (b.data.board_type == EIGHT_FOUR) total_depth = 5;
    // else total_depth = 4;

    depth = 1;
    // float time_each2 = float(time_each)*exp(-(pow(((total_time - static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(this->time_left).count()))-(total_time/100.0))/1.0,2)));
    // float time_each2 = float(time_each)*exp(-(pow((total_moves - average_moves)/15.0,2)));
    // std::cout<<(total_moves - average_moves)/15.0<<" ";
    // std::cout<<pow((total_moves - average_moves)/15.0,2)<<" ";
    // std::cout<<time_each2<<"\n";
    float time_each2 = time_each*exp(-param*pow((((total_time-static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(this->time_left).count()))-avg_time_left)/std_dev),2));
    // std::cout<<static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(this->time_left).count())<<" ";
    // std::cout<<total_time-static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(this->time_left).count())<<" ";
    // std::cout<<(total_time-static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(this->time_left).count()))-avg_time_left<<" ";
    // std::cout<<(((total_time-static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(this->time_left).count()))-avg_time_left)/std_dev)<<" ";
    // std::cout<<pow((((total_time-static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(this->time_left).count()))-avg_time_left)/std_dev),2)<<" ";
    // std::cout<<param*pow((((total_time-static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(this->time_left).count()))-avg_time_left)/std_dev),2)<<" ";
    // std::cout<<exp(-param*pow((((total_time-static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(this->time_left).count()))-avg_time_left)/std_dev),2))<<" ";
    // std::cout<<time_each2<<"\n";
    start = std::chrono::high_resolution_clock::now();
    // int iter = 1;
    do {
        // check_here("in the do while loop");
        // std::cout<<depth<<"\n";
        std::pair<int,U16> m = max_value(b,depth,INT_MIN, INT_MAX, time_each2);
        if (color == WHITE && total_moves == 1){
            this->best_move = str_to_move("c1b2");
        }
        else{

            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < time_each2) 
                this->best_move = m.second;
        
        }
        // iter -= 1;
        depth++;
        // what_is(depth);
    }
    // while(iter > 0);
    while ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < time_each2));
}
