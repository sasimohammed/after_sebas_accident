"""
moves.py – Person 2: Move System
Hnefatafl – Fetlar Rules (11x11)

Responsibilities:
  - Generate legal moves (rook-like)
  - Validate moves
  - Handle throne and corners rules
  - No jumping over pieces
"""

from board import (
    EMPTY, ATTACKER, DEFENDER, KING,
    DIRS, in_bounds, is_throne, is_corner, is_restricted
)


def is_hostile_square(board, r, c, attacker):
    """
    Check if square is hostile for capture purposes.
    attacker = the piece being captured (ATTACKER or DEFENDER/KING)
    """
    if not in_bounds(r, c):
        return False

    # Corners are always hostile
    if is_corner(r, c):
        return True

    # Throne is hostile to attackers, but only to defenders when empty
    if is_throne(r, c):
        if attacker == ATTACKER:
            return True
        else:  # defender/king
            return board[r][c] == EMPTY

    # Check if there's an enemy piece
    cell = board[r][c]
    if attacker == ATTACKER:
        return cell in (DEFENDER, KING)
    else:
        return cell == ATTACKER


def piece_moves(board, r, c):
    """
    Get all valid moves for piece at (r,c).
    Returns list of (from_r, from_c, to_r, to_c)
    """
    piece = board[r][c]
    if piece == EMPTY:
        return []

    moves = []
    for dr, dc in DIRS:
        nr, nc = r + dr, c + dc

        while in_bounds(nr, nc):
            cell = board[nr][nc]

            # Can't move through pieces
            if cell != EMPTY:
                break

            # King can move onto corners
            if is_corner(nr, nc):
                if piece == KING:
                    moves.append((r, c, nr, nc))
                break

            # Only king can occupy throne
            if is_throne(nr, nc) and piece != KING:
                nr += dr
                nc += dc
                continue

            # Valid move
            moves.append((r, c, nr, nc))
            nr += dr
            nc += dc

    return moves


def all_moves(board, player):
    """Get all moves for a player"""
    moves = []
    for r in range(len(board)):
        for c in range(len(board[0])):
            piece = board[r][c]
            if player == ATTACKER and piece == ATTACKER:
                moves.extend(piece_moves(board, r, c))
            elif player == DEFENDER and piece in (DEFENDER, KING):
                moves.extend(piece_moves(board, r, c))
    return moves


def do_captures(board, r, c):
    """
    Perform all captures triggered by a piece moving to (r,c).
    """
    mover = board[r][c]
    captured = []

    # Check horizontal captures
    for step in [-1, 1]:
        c1 = c + step
        if not in_bounds(r, c1):
            continue

        piece1 = board[r][c1]
        if piece1 == EMPTY or piece1 == KING:
            continue

        if mover == ATTACKER and piece1 not in (DEFENDER, KING):
            continue
        if mover in (DEFENDER, KING) and piece1 != ATTACKER:
            continue

        c2 = c + 2 * step
        if is_hostile_square(board, r, c2, piece1):
            captured.append((r, c1))

    # Check vertical captures
    for step in [-1, 1]:
        r1 = r + step
        if not in_bounds(r1, c):
            continue

        piece1 = board[r1][c]
        if piece1 == EMPTY or piece1 == KING:
            continue

        if mover == ATTACKER and piece1 not in (DEFENDER, KING):
            continue
        if mover in (DEFENDER, KING) and piece1 != ATTACKER:
            continue

        r2 = r + 2 * step
        if is_hostile_square(board, r2, c, piece1):
            captured.append((r1, c))

    # Remove captured pieces
    for cr, cc in captured:
        board[cr][cc] = EMPTY


def apply_move(board, move):
    """Apply a move to the board and handle captures"""
    sr, sc, tr, tc = move
    piece = board[sr][sc]

    # Move the piece
    board[tr][tc] = piece
    board[sr][sc] = EMPTY

    # Handle captures from the new position
    do_captures(board, tr, tc)