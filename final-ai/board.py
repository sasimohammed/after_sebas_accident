"""
board.py – Person 1: Board & Game State
Hnefatafl – Fetlar Rules (11x11)

Responsibilities:
  - Board representation (11x11)
  - King, defenders, attackers setup
  - Initial state + printing board
  - State cloning for AI
"""

# ── Constants ─────────────────────────────────────────────────────────────────
BOARD_SIZE = 11
EMPTY, ATTACKER, DEFENDER, KING = 0, 1, 2, 3

CORNERS = frozenset([(0, 0), (0, 10), (10, 0), (10, 10)])
THRONE_R, THRONE_C = 5, 5
DIRS = ((0, 1), (0, -1), (1, 0), (-1, 0))

# Colors
C_LIGHT = "#F5ECD7"
C_DARK = "#D4B896"
C_THRONE = "#C4A474"
C_CORNER = "#8BC38B"
C_ATK = "#C94040"
C_DEF = "#3B7DD8"
C_KING_F = "#E8C040"
C_KING_O = "#B8900A"
C_SEL = "#ADFF8A"
C_DOT = "#228B22"
C_BG = "#F0EAD6"
C_PANEL = "#EDE3CE"
C_TEXT = "#2C2416"
C_GRID = "#C0B090"
MIN_CS, MAX_CS = 38, 80


def init_board():
    """Initialize the Fetlar 11x11 starting position"""
    b = [[EMPTY] * BOARD_SIZE for _ in range(BOARD_SIZE)]

    # Place king on throne
    b[THRONE_R][THRONE_C] = KING

    # Place defenders (12 pieces including king)
    defender_positions = [
        (5, 4), (5, 6), (4, 5), (6, 5),  # around throne
        (5, 3), (5, 7), (3, 5), (7, 5),  # extended
        (4, 4), (4, 6), (6, 4), (6, 6)   # corners of inner square
    ]
    for r, c in defender_positions:
        b[r][c] = DEFENDER

    # Place attackers (24 pieces)
    attacker_positions = [
        # Top edge
        (0, 3), (0, 4), (0, 5), (0, 6), (0, 7),
        # Bottom edge
        (10, 3), (10, 4), (10, 5), (10, 6), (10, 7),
        # Left edge
        (3, 0), (4, 0), (5, 0), (6, 0), (7, 0),
        # Right edge
        (3, 10), (4, 10), (5, 10), (6, 10), (7, 10),
        # Corners of inner square
        (1, 5), (9, 5), (5, 1), (5, 9)
    ]
    for r, c in attacker_positions:
        b[r][c] = ATTACKER

    return b


def clone(board):
    """Create a deep copy of the board"""
    return [row[:] for row in board]


def in_bounds(r, c):
    """Check if coordinates are within board"""
    return 0 <= r < BOARD_SIZE and 0 <= c < BOARD_SIZE


def is_throne(r, c):
    """Check if square is the throne"""
    return r == THRONE_R and c == THRONE_C


def is_corner(r, c):
    """Check if square is a corner"""
    return (r, c) in CORNERS


def is_restricted(r, c):
    """Check if square is restricted (throne or corner)"""
    return is_throne(r, c) or is_corner(r, c)


def find_king(board):
    """Find king position, return (row, col) or (None, None)"""
    for r in range(BOARD_SIZE):
        for c in range(BOARD_SIZE):
            if board[r][c] == KING:
                return r, c
    return None, None


def count_pieces(board):
    """Count attackers and defenders (excluding king from defenders count)"""
    atk = 0
    defe = 0
    for r in range(BOARD_SIZE):
        for c in range(BOARD_SIZE):
            p = board[r][c]
            if p == ATTACKER:
                atk += 1
            elif p == DEFENDER:
                defe += 1
    return atk, defe