"""
main.py – GUI Application Entry Point
Hnefatafl – Fetlar Rules (11x11)

Human  = Attackers (Red)  — moves first
AI     = Defenders + King (Blue/Gold)

Win conditions:
  Attackers win → capture the King (surround on 4 sides; 3 if next to throne)
  Defenders win → King reaches any corner square
"""

import tkinter as tk
from tkinter import messagebox

from board import (
    ATTACKER, DEFENDER, KING, EMPTY,
    BOARD_SIZE, init_board, count_pieces,
    in_bounds, is_corner, is_throne,
    C_LIGHT, C_DARK, C_THRONE, C_CORNER,
    C_ATK, C_DEF, C_KING_F, C_KING_O,
    C_SEL, C_DOT, C_BG, C_PANEL, C_TEXT, C_GRID,
    MIN_CS, MAX_CS
)
from moves import piece_moves, all_moves, apply_move
from game_controller import check_winner, switch_turn
from Ai import get_ai_move


class HnefataflGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Hnefatafl - Fetlar Rules 11x11")
        self.root.configure(bg=C_BG)

        self.board = None
        self.turn = ATTACKER  # Attackers (human) go first
        self.selected = None
        self.valid_moves = []
        self.game_over = False
        self.cell_size = 60
        self.ai_depth = 3
        self.move_history = []

        self.setup_ui()
        self.new_game()

    def setup_ui(self):
        main_frame = tk.Frame(self.root, bg=C_BG)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Top panel
        top_panel = tk.Frame(main_frame, bg=C_PANEL, relief=tk.RAISED, bd=1)
        top_panel.pack(fill=tk.X, pady=(0, 10))

        title = tk.Label(top_panel, text="Hnefatafl - Fetlar Rules",
                         font=("Helvetica", 16, "bold"), bg=C_PANEL, fg=C_TEXT)
        title.pack(side=tk.LEFT, padx=20, pady=10)

        self.status_var = tk.StringVar()
        self.status_var.set("Your turn - Attackers (Red)")
        status_label = tk.Label(top_panel, textvariable=self.status_var,
                                font=("Helvetica", 12), bg=C_PANEL, fg=C_TEXT)
        status_label.pack(side=tk.LEFT, padx=20, pady=10)

        # Difficulty selector
        diff_frame = tk.Frame(top_panel, bg=C_PANEL)
        diff_frame.pack(side=tk.RIGHT, padx=20, pady=10)
        tk.Label(diff_frame, text="Difficulty:", font=("Helvetica", 10),
                 bg=C_PANEL, fg=C_TEXT).pack(side=tk.LEFT)
        self.difficulty = tk.StringVar(value="Medium")
        difficulties = [("Easy", 2), ("Medium", 3), ("Hard", 4)]
        for text, depth in difficulties:
            rb = tk.Radiobutton(diff_frame, text=text, variable=self.difficulty,
                                value=text, command=lambda d=depth: self.set_difficulty(d),
                                bg=C_PANEL, activebackground=C_PANEL)
            rb.pack(side=tk.LEFT, padx=5)

        # Canvas
        self.canvas = tk.Canvas(main_frame, bg=C_LIGHT, highlightthickness=1,
                                highlightbackground=C_GRID)
        self.canvas.pack(fill=tk.BOTH, expand=True)
        self.canvas.bind("<Configure>", self.on_resize)
        self.canvas.bind("<Button-1>", self.on_click)

        # Bottom panel
        bottom_panel = tk.Frame(main_frame, bg=C_PANEL, relief=tk.RAISED, bd=1)
        bottom_panel.pack(fill=tk.X, pady=(10, 0))

        self.atk_count = tk.StringVar(value="24")
        self.def_count = tk.StringVar(value="12")

        counts_frame = tk.Frame(bottom_panel, bg=C_PANEL)
        counts_frame.pack(side=tk.LEFT, padx=20, pady=10)

        atk_frame = tk.Frame(counts_frame, bg=C_PANEL)
        atk_frame.pack(side=tk.LEFT, padx=10)
        tk.Label(atk_frame, text="Attackers", font=("Helvetica", 10, "bold"),
                 bg=C_PANEL, fg=C_ATK).pack()
        tk.Label(atk_frame, textvariable=self.atk_count, font=("Helvetica", 14, "bold"),
                 bg=C_PANEL, fg=C_ATK).pack()

        def_frame = tk.Frame(counts_frame, bg=C_PANEL)
        def_frame.pack(side=tk.LEFT, padx=10)
        tk.Label(def_frame, text="Defenders", font=("Helvetica", 10, "bold"),
                 bg=C_PANEL, fg=C_DEF).pack()
        tk.Label(def_frame, textvariable=self.def_count, font=("Helvetica", 14, "bold"),
                 bg=C_PANEL, fg=C_DEF).pack()

        new_game_btn = tk.Button(bottom_panel, text="New Game", command=self.new_game,
                                 font=("Helvetica", 11), bg=C_DARK, fg=C_TEXT,
                                 activebackground=C_LIGHT, padx=20, pady=5)
        new_game_btn.pack(side=tk.RIGHT, padx=20, pady=10)

        rules_text = "Goal: Attackers capture King (surround on 4 sides) | Defenders: King reaches corner"
        rules_label = tk.Label(bottom_panel, text=rules_text, font=("Helvetica", 9),
                               bg=C_PANEL, fg=C_TEXT)
        rules_label.pack(side=tk.BOTTOM, pady=(0, 5))

    def set_difficulty(self, depth):
        self.ai_depth = depth

    def new_game(self):
        self.board = init_board()
        self.turn = ATTACKER
        self.selected = None
        self.valid_moves = []
        self.game_over = False
        self.move_history = []
        self.update_counts()
        self.status_var.set("Your turn - Attackers (Red) - Click a red piece to move")
        self.draw_board()

    def update_counts(self):
        atk, defe = count_pieces(self.board)
        self.atk_count.set(str(atk))
        self.def_count.set(str(defe + 1))  # +1 for king

    def on_resize(self, event):
        size = min(event.width, event.height) // BOARD_SIZE
        self.cell_size = max(MIN_CS, min(MAX_CS, size))
        self.draw_board()

    def draw_board(self):
        self.canvas.delete("all")
        width = self.canvas.winfo_width()
        height = self.canvas.winfo_height()

        if width < 10 or height < 10:
            return

        cell_size = self.cell_size
        offset_x = max(0, (width - BOARD_SIZE * cell_size) // 2)
        offset_y = max(0, (height - BOARD_SIZE * cell_size) // 2)

        # Draw squares
        for r in range(BOARD_SIZE):
            for c in range(BOARD_SIZE):
                x1 = offset_x + c * cell_size
                y1 = offset_y + r * cell_size
                x2 = x1 + cell_size
                y2 = y1 + cell_size

                if is_corner(r, c):
                    color = C_CORNER
                elif is_throne(r, c):
                    color = C_THRONE
                elif (r + c) % 2 == 0:
                    color = C_LIGHT
                else:
                    color = C_DARK

                self.canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline=C_GRID, width=1)

        # Highlight selected piece
        if self.selected:
            sr, sc = self.selected
            x1 = offset_x + sc * cell_size
            y1 = offset_y + sr * cell_size
            x2 = x1 + cell_size
            y2 = y1 + cell_size
            self.canvas.create_rectangle(x1, y1, x2, y2, fill=C_SEL, outline="", stipple="gray50")

        # Draw valid move dots
        for _, _, tr, tc in self.valid_moves:
            cx = offset_x + tc * cell_size + cell_size // 2
            cy = offset_y + tr * cell_size + cell_size // 2
            radius = max(4, cell_size // 8)
            self.canvas.create_oval(cx - radius, cy - radius, cx + radius, cy + radius,
                                    fill=C_DOT, outline="")

        # Draw pieces
        for r in range(BOARD_SIZE):
            for c in range(BOARD_SIZE):
                piece = self.board[r][c]
                if piece == EMPTY:
                    continue

                x1 = offset_x + c * cell_size + 2
                y1 = offset_y + r * cell_size + 2
                x2 = x1 + cell_size - 4
                y2 = y1 + cell_size - 4

                if piece == ATTACKER:
                    self.canvas.create_oval(x1, y1, x2, y2, fill=C_ATK, outline="#8B2020", width=2)
                elif piece == DEFENDER:
                    self.canvas.create_oval(x1, y1, x2, y2, fill=C_DEF, outline="#1A4F9A", width=2)
                elif piece == KING:
                    self.canvas.create_oval(x1, y1, x2, y2, fill=C_KING_F, outline=C_KING_O, width=3)
                    cx = offset_x + c * cell_size + cell_size // 2
                    cy = offset_y + r * cell_size + cell_size // 2
                    self.canvas.create_text(cx, cy, text="K",
                                            font=("Helvetica", max(8, cell_size // 3), "bold"),
                                            fill="#7A5000")

    def on_click(self, event):
        if self.game_over:
            return

        if self.turn != ATTACKER:
            self.status_var.set("Please wait - AI is thinking...")
            return

        width = self.canvas.winfo_width()
        height = self.canvas.winfo_height()
        cell_size = self.cell_size
        offset_x = max(0, (width - BOARD_SIZE * cell_size) // 2)
        offset_y = max(0, (height - BOARD_SIZE * cell_size) // 2)

        col = (event.x - offset_x) // cell_size
        row = (event.y - offset_y) // cell_size

        if not in_bounds(row, col):
            return

        if self.selected is None:
            if self.board[row][col] == ATTACKER:
                self.selected = (row, col)
                self.valid_moves = piece_moves(self.board, row, col)
                self.status_var.set(
                    f"Selected piece at {chr(97 + col)}{row + 1} - {len(self.valid_moves)} moves available")
                self.draw_board()
            else:
                self.status_var.set("Click on a red attacker piece to move")
        else:
            sr, sc = self.selected
            move = next((m for m in self.valid_moves if m[2] == row and m[3] == col), None)

            if move:
                apply_move(self.board, move)
                self.selected = None
                self.valid_moves = []
                self.update_counts()
                self.draw_board()

                winner = check_winner(self.board)
                if winner:
                    self.end_game(winner)
                    return

                if not all_moves(self.board, DEFENDER):
                    self.end_game(ATTACKER)
                    return

                state = tuple(tuple(row) for row in self.board)
                self.move_history.append(state)
                if self.move_history.count(state) >= 3:
                    self.end_game('draw')
                    return

                self.turn = switch_turn(self.turn)
                self.status_var.set("AI is thinking...")
                self.root.after(100, self.ai_move)
            else:
                if self.board[row][col] == ATTACKER:
                    self.selected = (row, col)
                    self.valid_moves = piece_moves(self.board, row, col)
                    self.status_var.set(
                        f"Selected piece at {chr(97 + col)}{row + 1} - {len(self.valid_moves)} moves available")
                    self.draw_board()
                else:
                    self.selected = None
                    self.valid_moves = []
                    self.status_var.set("Invalid move - click on a red piece to move")
                    self.draw_board()

    def ai_move(self):
        if self.game_over or self.turn != DEFENDER:
            return

        self.status_var.set("AI is thinking...")
        self.root.update()

        move = get_ai_move(self.board, self.ai_depth, time_limit=2.0)

        if move:
            apply_move(self.board, move)
            self.update_counts()
            self.draw_board()

            winner = check_winner(self.board)
            if winner:
                self.end_game(winner)
                return

            if not all_moves(self.board, ATTACKER):
                self.end_game(DEFENDER)
                return

            state = tuple(tuple(row) for row in self.board)
            self.move_history.append(state)
            if self.move_history.count(state) >= 3:
                self.end_game('draw')
                return

            self.turn = switch_turn(self.turn)
            self.status_var.set("Your turn - Attackers (Red) - Click a red piece to move")
        else:
            self.status_var.set("AI has no moves - You win!")
            self.end_game(ATTACKER)

    def end_game(self, winner):
        self.game_over = True
        self.selected = None
        self.valid_moves = []

        if winner == ATTACKER:
            title = "Attackers Win!"
            message = "You have captured the King!\n\nAttackers (Red) win!"
            self.status_var.set("Game Over - Attackers Win!")
        elif winner == DEFENDER:
            title = "Defenders Win!"
            message = "The King has escaped to a corner!\n\nDefenders (Blue) win!"
            self.status_var.set("Game Over - Defenders Win!")
        else:
            title = "Draw"
            message = "The game is a draw due to repetition!"
            self.status_var.set("Game Over - Draw!")

        messagebox.showinfo(title, message)
        self.draw_board()


# ── Main ──────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    root = tk.Tk()
    root.geometry("900x850")
    app = HnefataflGUI(root)
    root.mainloop()
