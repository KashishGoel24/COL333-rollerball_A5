print('Start of Python Code')
from board import Board
import random
print('Successfull imoprts in pythonn code')

def find_best_move(board):
    print('Inside find_best_move in py')
    print(board.get_legal_moves())
    print('Planning to return from python code')
    moves = list(board.get_legal_moves())
    idx = random.randint(0, len(moves) - 1)
    return moves[idx]
