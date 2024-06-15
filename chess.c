//
// Created by lukas on 16/06/2024.
//

#include "chess.h"

#include <assert.h>
#include <stdio.h>

ChessBoard init_chessboard() {
    ChessBoard board;
    board.flags = 0;
    board.board[0] = ROOK << 28 | KNIGHT << 24 | BISHOP << 20 | QUEEN << 16 | KING << 12 | BISHOP << 8 | KNIGHT << 4 |
                     ROOK;
    board.board[1] = PAWN << 28 | PAWN << 24 | PAWN << 20 | PAWN << 16 | PAWN << 12 | PAWN << 8 | PAWN << 4 | PAWN;
    board.board[2] = 0;
    board.board[3] = 0;
    board.board[4] = 0;
    board.board[5] = 0;
    constexpr unsigned int blackMask = 0b10001000100010001000100010001000;
    board.board[6] = (PAWN << 28 | PAWN << 24 | PAWN << 20 | PAWN << 16 | PAWN << 12 | PAWN << 8 | PAWN << 4 | PAWN) |
                     blackMask;
    board.board[7] = (ROOK << 28 | KNIGHT << 24 | BISHOP << 20 | QUEEN << 16 | KING << 12 | BISHOP << 8 | KNIGHT << 4 |
                      ROOK) | blackMask;
    return board;
}

char get_board_at(const ChessBoard board, const char *square) {
    assert(square[0] >= 'a' && square[0] <= 'h');
    assert(square[1] >= '1' && square[1] <= '8');
    const char file = square[0] - 'a';
    const char rank = square[1] - '1';
    return board.board[rank] >> ((7 - file) * 4) & 0b1111;
}

void print_board(const ChessBoard board) {
    bool square_black = false;
    printf("--------------------------------\033[0m\n");
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; ++file) {
            square_black = !square_black;
            //sets background of square
            if (square_black) { printf("\033[42m"); } else { printf("\033[44m"); }

            const char content = board.board[rank] >> ((7 - file) * 4) & 0b1111;

            //sets foreground color of square
            if (content & COLOR_MASK) { printf("\033[0;35m"); } else { printf("\033[0;37m"); }

            if((content & PIECE_MASK) == PAWN) {printf("p   ");}
            if((content & PIECE_MASK) == ROOK) {printf("R   ");}
            if((content & PIECE_MASK) == KNIGHT) {printf("N   ");}
            if((content & PIECE_MASK) == BISHOP) {printf("B   ");}
            if((content & PIECE_MASK) == QUEEN) {printf("Q   ");}
            if((content & PIECE_MASK) == KING) {printf("K   ");}
            if((content & PIECE_MASK) == EMPTY) {printf("    ");}

        }
        printf("\n\n");
    }
    printf("--------------------------------\033[0m\n");
}
