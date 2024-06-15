//
// Created by lukas on 16/06/2024.
//

#include "chess.h"

ChessBoard init_chessboard() {
    ChessBoard board;
    board.flags = 0;
    board.board[0] = ROOK << 28 | KNIGHT << 24 | BISHOP << 20 | QUEEN << 16 | KING << 12 | BISHOP << 8 | KNIGHT << 4 | ROOK;
    board.board[1] = PAWN << 28 | PAWN << 24 | PAWN << 20 | PAWN << 16 | PAWN << 12 | PAWN << 8 | PAWN << 4 | PAWN;
    board.board[2] = 0;
    board.board[3] = 0;
    board.board[4] = 0;
    board.board[5] = 0;
    constexpr unsigned int blackMask = 0b10001000100010001000100010001000;
    board.board[6] = (PAWN << 28 | PAWN << 24 | PAWN << 20 | PAWN << 16 | PAWN << 12 | PAWN << 8 | PAWN << 4 | PAWN) | blackMask;
    board.board[7] = (ROOK << 28 | KNIGHT << 24 | BISHOP << 20 | QUEEN << 16 | KING << 12 | BISHOP << 8 | KNIGHT << 4 | ROOK) | blackMask;
    return board;
}