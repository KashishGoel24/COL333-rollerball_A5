#pragma once

#include <atomic>
// #include <bits/stdc++.h>
#include "engine_base.hpp"
// #include "neuralnet.hpp"
#include "butils.hpp"

#define minm(x,y,z) std::min(x,std::min(y,z))

// std::vector<float> weights({100.0,40.0,10.0,3.0,10.0,2.0});
// std::vector<std::vector<float>> input_data;
// std::vector<float> output_data;

// float learning_rate = 0.1;
class Node {
    
    public:
        Node(Node* parent, U16 action_taken);
        Node(Board* b);
        Node* select(int iterations);
        void expand();
        void back_propagate(float wins);
        float num_wins = 0;
        int num_visits = 0;
        float p = 1;
        Board* state;
        std::vector<Node*> children;
        Node* parent = NULL;
        U16 action_taken;
    private:
        void generate_children() ;

        void get_probability_distribution();
        
        float UCB(int iterations) ;
};

class MCTS {
    public:
        MCTS(Node* root);
        Node* root;
        std::map<U16,float> search(unsigned t);
};

class Engine : public AbstractEngine {

    // add extra items here. 
    // Note that your engine will always be instantiated with the default 
    // constructor.
    
    public:
    void find_best_move(const Board& b) override;

};
