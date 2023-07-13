#include <iostream>
#include <vector>

#include "game.hpp"

std::string board_to_str(uint64_t board) {

    std::string board_str = "       \n0000000\n0000000\n00   00\n00   00\n00   00\n0000000\n0000000\n";

    for (uint64_t i=0; i<64; i++) {
        if ((1ULL << i) & board) {
            board_str[63-i] = '1';
        }
    }

    return board_str;
}

int main(int argc, char** argv) {

    std::cout << board_to_str(BOARD_MASK) << std::endl;
    std::cout << board_to_str(BOTTOM_REGION) << std::endl;
    std::cout << board_to_str(TOP_REGION) << std::endl;
    std::cout << board_to_str(LEFT_REGION) << std::endl;
    std::cout << board_to_str(RIGHT_REGION) << std::endl;
    std::cout << board_to_str(INNER_RING) << std::endl;
    std::cout << board_to_str(OUTER_RING) << std::endl;

    // barrel shift test
    std::cout << std::hex << uint32_t(uint32_t(uint8_t(0xff) << 5)) << std::dec << "\n";

    MoveDB d = MoveDB();

    for (int i=0; i<56; i++) {
        if (!d.rook[i]) continue;
        std::cout << i << ":\n";
        auto bstr = board_to_str(d.rook[i]);
        bstr[63-i] = 'x';
        std::cout << bstr << "\n";
    }

    return 0;
}
