"""
evaluation.py – Person 4: Utility Function
Hnefatafl – Fetlar Rules (11x11)

Responsibilities:
  - Evaluate board state
  - King safety (distance to corners)
  - Piece advantage
  - Threat assessment
"""

from board import (
    ATTACKER, DEFENDER, KING,
    CORNERS, DIRS,
    in_bounds, is_corner, find_king, count_pieces
)
from moves import piece_moves, all_moves, apply_move
from board import clone


def evaluate(board):
    """
    Evaluate board from defender's perspective.
    Positive = good for AI/defenders. Negative = good for attackers.
    """
    kr, kc = find_king(board)

    # Terminal states
    if kr is None:
        return -100000
    if is_corner(kr, kc):
        return 100000

    score = 0
    atk, defe = count_pieces(board)

    # King distance to nearest corner (closer = better for defenders)
    min_dist = min(abs(kr - cr) + abs(kc - cc) for cr, cc in CORNERS)
    score += (20 - min_dist) * 15

    # King escape lanes – reward open paths to corners
    for cr, cc in CORNERS:
        if kr == cr:  # Same row
            step = 1 if cc > kc else -1
            path_clear = True
            blockers = 0
            for nc in range(kc + step, cc, step):
                if board[kr][nc] == ATTACKER:
                    blockers += 1
                    path_clear = False
                elif board[kr][nc] in (DEFENDER, KING):
                    path_clear = False
            if path_clear:
                score += 80
            elif blockers > 0:
                score -= blockers * 15
        elif kc == cc:  # Same column
            step = 1 if cr > kr else -1
            path_clear = True
            blockers = 0
            for nr in range(kr + step, cr, step):
                if board[nr][kc] == ATTACKER:
                    blockers += 1
                    path_clear = False
                elif board[nr][kc] in (DEFENDER, KING):
                    path_clear = False
            if path_clear:
                score += 80
            elif blockers > 0:
                score -= blockers * 15

    # King safety – adjacent attackers are bad, defenders are good
    for dr, dc in DIRS:
        ar, ac = kr + dr, kc + dc
        if in_bounds(ar, ac):
            if board[ar][ac] == ATTACKER:
                score -= 30
            elif board[ar][ac] == DEFENDER:
                score += 15

    # King mobility – more moves is better
    king_moves = piece_moves(board, kr, kc)
    score += len(king_moves) * 5

    # Check for immediate winning move
    for _, _, tr, tc in king_moves:
        if is_corner(tr, tc):
            score += 500

    # Material balance
    score += defe * 10
    score -= atk * 6

    # Defenders protecting king
    for dr, dc in DIRS:
        nr, nc = kr + dr, kc + dc
        if in_bounds(nr, nc) and board[nr][nc] == DEFENDER:
            score += 20

    return score


def order_moves(board, moves, is_defender):
    """Order moves for better alpha-beta pruning efficiency."""
    scored_moves = []

    for mv in moves:
        sr, sc, tr, tc = mv
        piece = board[sr][sc]
        priority = 0

        # King moves to corner is highest priority
        if piece == KING and is_corner(tr, tc):
            priority = 100000
        elif is_defender and piece == DEFENDER:
            test_board = clone(board)
            apply_move(test_board, mv)
            atk_after, _ = count_pieces(test_board)
            atk_before, _ = count_pieces(board)
            if atk_after < atk_before:
                priority = 10000 + (atk_before - atk_after) * 1000
        elif not is_defender and piece == ATTACKER:
            test_board = clone(board)
            apply_move(test_board, mv)
            _, def_after = count_pieces(test_board)
            _, def_before = count_pieces(board)
            if def_after < def_before:
                priority = 10000 + (def_before - def_after) * 1000

        scored_moves.append((priority, mv))

    scored_moves.sort(reverse=True, key=lambda x: x[0])
    return [mv for _, mv in scored_moves]