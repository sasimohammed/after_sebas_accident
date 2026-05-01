import copy #used in state cloning 

BOARD_SIZE = 11

EMPTY = 0
ATTACKER = 1
DEFENDER = 2
KING = 3


def initialize_board():
    board = [[EMPTY for _ in range(BOARD_SIZE)] for _ in range(BOARD_SIZE)]

    mid = 5 

    # set King
    board[mid][mid] = KING

    # Defenders around king
    defenders = [
        (5,4),(5,6),(4,5),(6,5),
        (5,3),(5,7),(3,5),(7,5),
        (4,4),(4,6),(6,4),(6,6)
    ]
    for r,c in defenders:
        board[r][c] = DEFENDER

    # Attackers
    attackers = [
        # TOP
        (0,3),(0,4),(0,5),(0,6),(0,7),
        (1,5),
        # BOTTOM 
        (10,3),(10,4),(10,5),(10,6),(10,7),
        (9,5),
        # LEFT 
        (3,0),(4,0),(5,0),(6,0),(7,0),
        (5,1),
        # RIGHT
        (3,10),(4,10),(5,10),(6,10),(7,10),
        (5,9)
    ]
    for r,c in attackers:
        board[r][c] = ATTACKER

    return board


def print_board(board):
    symbols = {
        EMPTY: '.',
        ATTACKER: 'A',
        DEFENDER: 'D',
        KING: 'K'
    }

    for row in board:
        print(' '.join(symbols[cell] for cell in row))

class GameState:
    def __init__(self, board, turn):
        self.board = board
        self.turn = turn  # ATTACKER or DEFENDER

# state cloning for AI
def clone_state(state):
    new_board = copy.deepcopy(state.board)
    return GameState(new_board, state.turn)

def switch_turn(turn):
    return ATTACKER if turn == DEFENDER else DEFENDER

# test
board = initialize_board()
print_board(board)
