"""
game_controller.py – Person 3: Game Controller
Hnefatafl – Fetlar Rules (11x11)

Responsibilities:
  - Turn switching (attacker first)
  - Human vs AI flow
  - Apply moves and update state
  - Check win/lose conditions
"""

from board import (
    ATTACKER, DEFENDER, KING,
    DIRS, THRONE_R, THRONE_C,
    in_bounds, is_corner, is_throne, find_king, count_pieces
)
from moves import all_moves, apply_move, piece_moves


def king_captured(board):
    """
    Check if king is captured according to Fetlar rules.
    King captured when surrounded on all 4 sides,
    except when next to throne (only needs 3).
    """
    kr, kc = find_king(board)
    if kr is None:
        return True

    # King can't be captured on corner (game would have ended)
    if is_corner(kr, kc):
        return False

    surrounding = 0
    needed = 4

    # If king is next to throne, only need 3 attackers
    if abs(kr - THRONE_R) + abs(kc - THRONE_C) == 1:
        needed = 3

    for dr, dc in DIRS:
        ar, ac = kr + dr, kc + dc

        if not in_bounds(ar, ac):
            return False  # King on edge not captured

        cell = board[ar][ac]

        if cell == ATTACKER:
            surrounding += 1
        elif is_throne(ar, ac) and not is_throne(kr, kc):
            if cell == 0:  # EMPTY
                surrounding += 1
        else:
            return False

    return surrounding >= needed


def check_encirclement(board):
    """
    Check if attackers have encircled all defenders (rule 7b).
    """
    kr, kc = find_king(board)
    if kr is None:
        return True

    from board import BOARD_SIZE, EMPTY, DEFENDER
    defenders = [(kr, kc)]
    for r in range(BOARD_SIZE):
        for c in range(BOARD_SIZE):
            if board[r][c] == DEFENDER:
                defenders.append((r, c))

    if not defenders:
        return True

    visited = set()
    queue = defenders[:]

    for start in queue:
        visited.add(start)

    while queue:
        r, c = queue.pop(0)

        if (r == 0 or r == BOARD_SIZE - 1 or c == 0 or c == BOARD_SIZE - 1) and not is_corner(r, c):
            return False

        for dr, dc in DIRS:
            nr, nc = r + dr, c + dc
            if in_bounds(nr, nc) and (nr, nc) not in visited:
                cell = board[nr][nc]
                if cell == EMPTY or cell == DEFENDER or cell == KING:
                    visited.add((nr, nc))
                    queue.append((nr, nc))

    return True


def check_winner(board):
    """Check if game has ended. Returns winner constant or None."""
    kr, kc = find_king(board)

    if kr is None or king_captured(board):
        return ATTACKER

    if is_corner(kr, kc):
        return DEFENDER

    if check_encirclement(board):
        return ATTACKER

    return None