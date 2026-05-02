from boardSetup import *


def setup_game():
    side_choice = 0
    while side_choice not in [1, 2]:
        print("Choose your side:")
        print("1- Attacker")
        print("2- Defender")
        side_choice = int(input("Your choice: "))

    if side_choice == 1:
        human_side = ATTACKER
        ai_side = DEFENDER
    else:
        human_side = DEFENDER
        ai_side = ATTACKER

    difficulty = 0
    while difficulty not in [1, 2, 3]:
        print("Choose difficulty:")
        print("1- Easy")
        print("2- Medium")
        print("3- Hard")
        difficulty = int(input("Your choice: "))

    if difficulty == 1:
        depth = 1
    elif difficulty == 2:
        depth = 3
    else:
        depth = 5

    return human_side, ai_side, depth


def get_move():
    print("What is the position of the piece you want to move? (Row,Column)\n")
    source_r, source_c = input("Your Position: ").strip().split(",")
    print("What is the position of the place you want to move to? (Row,Column)\n")
    target_r, target_c = input("Your Position: ").strip().split(
        ",")
    return int(source_r), int(source_c), int(target_r), int(target_c)


def apply_move(board, move):
    source_r, source_c, target_r, target_c = move
    board[target_r][target_c] = board[source_r][source_c]
    board[source_r][source_c] = EMPTY
    # check_captures(board, target_r, target_c) a function for Handle throne and corners rules
    # (Capturing Opponents) katkat 🩷
    return board


def check_winner(board):
    corners = [(0, 0), (0, 10), (10, 0), (10, 10)]
    for r, c in corners:
        if board[r][c] == KING:
            return DEFENDER

    king_found = False
    for i in range(11):
        for j in range(11):
            if board[i][j] == KING:
                r, c = i, j
                king_found = True
                break
        if king_found:
            break
    if not king_found:
        return ATTACKER

    if 0 < r < 10 and 0 < c < 10:
        if board[r][c - 1] == ATTACKER and board[r][c + 1] == ATTACKER and board[r - 1][c] == ATTACKER and board[r + 1][c] == ATTACKER:
            return ATTACKER

    elif r == 0:
        if board[r][c - 1] == ATTACKER and board[r][c + 1] == ATTACKER and board[r + 1][c] == ATTACKER:
            return ATTACKER

    elif r == 10:
        if board[r][c - 1] == ATTACKER and board[r][c + 1] == ATTACKER and board[r - 1][c] == ATTACKER:
            return ATTACKER

    elif c == 0:
        if board[r - 1][c] == ATTACKER and board[r + 1][c] == ATTACKER and board[r][c + 1] == ATTACKER:
            return ATTACKER

    elif c == 10:
        if board[r - 1][c] == ATTACKER and board[r + 1][c] == ATTACKER and board[r][c - 1] == ATTACKER:
            return ATTACKER

    return None


def play_game():
    human_side, ai_side, depth = setup_game()
    board = initialize_board()
    current_player = ATTACKER
    winner = None
    while not winner:
        print_board(board)
        if current_player == human_side:
            move = get_move()
        else:
            move = get_move()   # temporary until AI connected (placeholder)
            #            move =get ai move()  Alpha-Beta AI soso 🩷
            #            source_r, source_c, target_r, target_c = move

            # if validate_move(board, move):  (a function to make sure it is a valid move) katkat 🩷
        apply_move(board, move)
        winner = check_winner(board)
        if winner is None:
            current_player = switch_turn(current_player)
    if winner == ATTACKER:
        print("Attackers win!")
    else:
        print("Defenders win!")


# test
play_game()