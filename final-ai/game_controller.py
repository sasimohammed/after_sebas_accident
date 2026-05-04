"""
game_controller.py – Game Controller
Handles turn switching + win conditions
"""

from board import (
    ATTACKER, DEFENDER, KING,
    DIRS, THRONE_R, THRONE_C,
    in_bounds, is_corner, is_throne,
    find_king, BOARD_SIZE, EMPTY
)


# -------------------------------------------------
# Turn Switching
# -------------------------------------------------
def switch_turn(current_player):
    if current_player == ATTACKER:
        return DEFENDER
    return ATTACKER


# -------------------------------------------------
# King Capture Rules
# -------------------------------------------------
def king_captured(board):
    """
    Checks if king is captured.
    Fetlar rule:
    - 4 sides normally
    - 3 sides if adjacent to throne
    """
    kr, kc = find_king(board)

    if kr is None:
        return True

    if is_corner(kr, kc):
        return False

    surrounding = 0
    needed = 4

    if abs(kr - THRONE_R) + abs(kc - THRONE_C) == 1:
        needed = 3

    for dr, dc in DIRS:
        nr, nc = kr + dr, kc + dc

        if not in_bounds(nr, nc):
            return False

        if board[nr][nc] == ATTACKER:
            surrounding += 1
        elif is_throne(nr, nc) and board[nr][nc] == EMPTY:
            surrounding += 1
        else:
            return False

    return surrounding >= needed


# -------------------------------------------------
# Encirclement Rule
# -------------------------------------------------
def check_encirclement(board):
    """
    Attackers win if defenders are trapped.
    """
    kr, kc = find_king(board)

    if kr is None:
        return True

    defenders = [(kr, kc)]

    for r in range(BOARD_SIZE):
        for c in range(BOARD_SIZE):
            if board[r][c] == DEFENDER:
                defenders.append((r, c))

    visited = set(defenders)
    queue = defenders[:]

    while queue:
        r, c = queue.pop(0)

        if (r in [0, BOARD_SIZE - 1] or c in [0, BOARD_SIZE - 1]) and not is_corner(r, c):
            return False

        for dr, dc in DIRS:
            nr, nc = r + dr, c + dc

            if in_bounds(nr, nc) and (nr, nc) not in visited:
                if board[nr][nc] in (EMPTY, DEFENDER, KING):
                    visited.add((nr, nc))
                    queue.append((nr, nc))

    return True


# -------------------------------------------------
# Winner Detection
# -------------------------------------------------
def check_winner(board):
    """
    Returns:
    ATTACKER -> attackers win
    DEFENDER -> defenders win
    None     -> continue
    """
    kr, kc = find_king(board)

    if kr is None or king_captured(board):
        return ATTACKER

    if is_corner(kr, kc):
        return DEFENDER

    if check_encirclement(board):
        return ATTACKER

    return None
