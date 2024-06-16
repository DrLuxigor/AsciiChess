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

char get_board_at_square(const ChessBoard board, const char *square) {
    assert(square[0] >= 'a' && square[0] <= 'h');
    assert(square[1] >= '1' && square[1] <= '8');
    const char file = square[0] - 'a';
    const char rank = square[1] - '1';
    return get_board_at(board, rank, file);
}

char get_board_at(const ChessBoard board, const char rank, const char file) {
    assert(rank >= 0 && rank < 8);
    assert(file >= 0 && file < 8);
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

            if ((content & PIECE_MASK) == PAWN) { printf("p   "); }
            if ((content & PIECE_MASK) == ROOK) { printf("R   "); }
            if ((content & PIECE_MASK) == KNIGHT) { printf("N   "); }
            if ((content & PIECE_MASK) == BISHOP) { printf("B   "); }
            if ((content & PIECE_MASK) == QUEEN) { printf("Q   "); }
            if ((content & PIECE_MASK) == KING) { printf("K   "); }
            if ((content & PIECE_MASK) == EMPTY) { printf("    "); }
        }
        printf("\n\n");
    }
    printf("--------------------------------\033[0m\n");
}

bool try_move_pawn(ChessBoard* board, const char piece, const char fileFrom, const char rankFrom,
                   const char fileTo, const char rankTo) {
    //trying to move piece backward or sideways
    const bool black = piece & COLOR_MASK;
    if (black && rankFrom <= rankTo) { return false; }
    if (!black && rankFrom >= rankTo) { return false; }
    if (rankFrom == rankTo) { return false; }

    //trying to move pawn too far. (overflow helps us here)
    if (rankTo - rankFrom > 2)
        return false;

    const char moves = black ? rankFrom - rankTo : rankTo - rankFrom;
    //trying to move pawn 2 squares when not at starting position
    if (moves == 2 && ((black && rankFrom != 6)) || (!black && rankFrom != 1)) { return false; }

    assert(moves == 1 || moves == 2);

    //no captures
    if (fileFrom == fileTo) {
        if (moves == 1 && is_empty(*board, rankTo, fileTo)) {
            set_piece(board, piece, rankTo, fileTo);
            set_empty(board, rankFrom, fileFrom);
        }
        if (moves == 2) {
            if(black) {
                if(is_empty(*board, rankFrom - 1, fileFrom) && is_empty(*board, rankTo, fileTo)) {
                    set_piece(board, piece, rankTo, fileTo);
                    set_empty(board, rankFrom, fileFrom);
                }
            } else {
                if(is_empty(*board, rankFrom + 1, fileFrom) && is_empty(*board, rankTo, fileTo)) {
                    set_piece(board, piece, rankTo, fileTo);
                    set_empty(board, rankFrom, fileFrom);
                }
            }
        }
    } else {
        //capture

    }


    return true;
}

bool move(ChessBoard* board, const char *from, const char *to) {
    const char piece_to_move = get_board_at_square(*board, from);

    //no piece at square
    if (piece_to_move == 0) { return false; }

    //trying to move opponents piece
    if (board->flags & 0b1 != (piece_to_move & COLOR_MASK) >> 3) { return false; }

    const char fileFrom = from[0] - 'a';
    const char rankFrom = from[1] - '1';
    const char fileTo = to[0] - 'a';
    const char rankTo = to[1] - '1';
    //check if move is allowed
    if ((piece_to_move & PIECE_MASK) == PAWN) {
        const bool ok =  try_move_pawn(board, piece_to_move, fileFrom, rankFrom, fileTo, rankTo);
        if(ok) { board->flags ^= 0b1; }
    } else if ((piece_to_move & PIECE_MASK) == ROOK) {
    } else if ((piece_to_move & PIECE_MASK) == KNIGHT) {
    } else if ((piece_to_move & PIECE_MASK) == BISHOP) {
    } else if ((piece_to_move & PIECE_MASK) == QUEEN) {
    } else if ((piece_to_move & PIECE_MASK) == KING) {
    } else { return false; } //not a valid piece


    return true;
}

void set_piece(ChessBoard* board, const char piece, const char rank, const char file) {
    unsigned int clearMask = 0b11111111111111111111111111111111;
    const unsigned int clears = 0b1111 << ((7-file) * 4);
    clearMask ^= clears;
    board->board[rank] &= clearMask;
    board->board[rank] |= piece << ((7-file) * 4);
}
void set_empty(ChessBoard* board ,const char rank,const char file) {
    unsigned int clearMask = 0b11111111111111111111111111111111;
    const unsigned int clears = 0b1111 << ((7-file) * 4);
    clearMask ^= clears;
    board->board[rank] &= clearMask;
}
bool is_empty(const ChessBoard board,const char rank,const char file)
{
    return (get_board_at(board, rank, file) & PIECE_MASK) == EMPTY;
}
