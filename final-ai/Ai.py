"""
ai.py – Person 5: Alpha-Beta AI
Hnefatafl – Fetlar Rules (11x11)

Responsibilities:
  - Implement alpha-beta pruning
  - Choose best move
  - Depth levels (Easy=2, Medium=3, Hard=4)
  - Uses move generator + evaluation function
"""

import time
from board import ATTACKER, DEFENDER, KING, clone, is_corner
from moves import all_moves, apply_move
from evaluation import evaluate, order_moves
from game_controller import check_winner


def alpha_beta(board, depth, alpha, beta, maximizing, deadline):
    """Alpha-beta pruning search."""
    if time.time() > deadline:
        return evaluate(board)

    winner = check_winner(board)
    if winner == ATTACKER:
        return -100000
    if winner == DEFENDER:
        return 100000

    if depth == 0:
        return evaluate(board)

    if maximizing:  # Defender's turn (AI)
        moves = all_moves(board, DEFENDER)
        if not moves:
            return -100000

        moves = order_moves(board, moves, True)

        for mv in moves:
            nb = clone(board)
            apply_move(nb, mv)
            value = alpha_beta(nb, depth - 1, alpha, beta, False, deadline)
            if value > alpha:
                alpha = value
            if alpha >= beta:
                break
        return alpha

    else:  # Attacker's turn
        moves = all_moves(board, ATTACKER)
        if not moves:
            return 100000

        moves = order_moves(board, moves, False)

        for mv in moves:
            nb = clone(board)
            apply_move(nb, mv)
            value = alpha_beta(nb, depth - 1, alpha, beta, True, deadline)
            if value < beta:
                beta = value
            if beta <= alpha:
                break
        return beta


def get_ai_move(board, depth, time_limit=2.0):
    """
    Get best move for AI (defenders) using iterative deepening alpha-beta.
    depth: Easy=2, Medium=3, Hard=4
    """
    moves = all_moves(board, DEFENDER)
    if not moves:
        return None

    # Check for immediate winning move first
    for mv in moves:
        sr, sc, tr, tc = mv
        if board[sr][sc] == KING and is_corner(tr, tc):
            return mv

    moves = order_moves(board, moves, True)

    deadline = time.time() + time_limit
    best_move = moves[0]
    best_value = -float('inf')

    # Iterative deepening
    for current_depth in range(1, depth + 1):
        for mv in moves:
            if time.time() > deadline:
                break
            nb = clone(board)
            apply_move(nb, mv)
            value = alpha_beta(nb, current_depth - 1, -float('inf'), float('inf'), False, deadline)
            if value > best_value:
                best_value = value
                best_move = mv
        if time.time() > deadline:
            break

    return best_move
