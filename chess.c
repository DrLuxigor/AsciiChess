//
// Created by lukas on 16/06/2024.
//

#include "chess.h"

#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

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
    char currentRank = '8';
    for (int rank = 7; rank >= 0; rank--) {
        printf("\033[0m%c | ", currentRank);
        currentRank -= 1;
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
    printf("\033[0m--------------------------------\n");
    printf("    a   b   c   d   e   f   g   h\n");

}

bool try_move_pawn(ChessBoard *board, const char piece, const char fileFrom, const char rankFrom,
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
    if (moves == 2 && ((black && rankFrom != 6) || (!black && rankFrom != 1))) { return false; }

    assert(moves == 1 || moves == 2);

    //no captures
    if (fileFrom == fileTo) {
        if (moves == 1 && is_empty(*board, rankTo, fileTo)) {
            set_piece(board, piece, rankTo, fileTo);
            set_empty(board, rankFrom, fileFrom);
            return true;
        }
        if (moves == 2) {
            if (black) {
                if (is_empty(*board, rankFrom - 1, fileFrom) && is_empty(*board, rankTo, fileTo)) {
                    set_piece(board, piece, rankTo, fileTo);
                    set_empty(board, rankFrom, fileFrom);
                    return true;
                }
            } else {
                if (is_empty(*board, rankFrom + 1, fileFrom) && is_empty(*board, rankTo, fileTo)) {
                    set_piece(board, piece, rankTo, fileTo);
                    set_empty(board, rankFrom, fileFrom);
                    return true;
                }
            }
        }
    } else {
        //capture
        assert(moves == 1);

        //can only move 1 left or right for capture
        if (fileFrom - fileTo > 1) { return false; }

        if (!is_empty(*board, rankTo, fileTo)) {
            set_piece(board, piece, rankTo, fileTo);
            set_empty(board, rankFrom, fileFrom);
            return true;
        } else if (false) {
            //TODO ente passente
        }
    }


    return true;
}

bool try_move_bishop(ChessBoard *board, const char piece, const char fileFrom, const char rankFrom,
                     const char fileTo, const char rankTo) {
    //const bool black = piece & COLOR_MASK;
    const signed char rankOffset = rankTo - rankFrom;
    const signed char fileOffset = fileTo - fileFrom;
    const signed char rankDir = rankOffset / abs(rankOffset);
    const signed char fileDir = fileOffset / abs(fileOffset);
    //move not diagonal
    if (abs(fileOffset) != abs(rankOffset)) { return false; }
    for (int i = 1; i < abs(rankOffset); ++i) {
        if(!is_empty(*board, rankFrom + i*rankDir, fileFrom + i*(fileDir)))
            return false;
    }
    set_piece(board, piece, rankTo, fileTo);
    set_empty(board, rankFrom, fileFrom);
    return true;
}

bool try_move_rook(ChessBoard *board, const char piece, const char fileFrom, const char rankFrom,
                     const char fileTo, const char rankTo) {
    const signed char rankOffset = rankTo - rankFrom;
    const signed char fileOffset = fileTo - fileFrom;

    //moves in 2 directions
    if(rankOffset != 0 && fileOffset != 0) { return false;}
    assert(!(rankOffset == 0 && fileOffset == 0));

    const signed char rankDir = rankOffset == 0 ? 0 : rankOffset / abs(rankOffset);
    const signed char fileDir = fileOffset == 0 ? 0 : fileOffset / abs(fileOffset);

    for (int i = 1; i < abs(rankOffset); ++i) {
        if(!is_empty(*board, rankFrom + i*rankDir, fileFrom + i*(fileDir)))
            return false;
    }

    set_piece(board, piece, rankTo, fileTo);
    set_empty(board, rankFrom, fileFrom);
    return true;
}
bool try_move_knight(ChessBoard *board, const char piece, const char fileFrom, const char rankFrom,
                     const char fileTo, const char rankTo) {
    const signed char rankOffset = rankTo - rankFrom;
    const signed char fileOffset = fileTo - fileFrom;

    //not valid knight move
    if(!((abs(rankOffset) == 2 && abs(fileOffset) == 1) || (abs(rankOffset) == 1 && abs(fileOffset) == 2))) { return false;}

    set_piece(board, piece, rankTo, fileTo);
    set_empty(board, rankFrom, fileFrom);
    return true;
}

bool move(ChessBoard *board, const char *from, const char *to) {
    const char piece_to_move = get_board_at_square(*board, from);
    const char piece_at_dest = get_board_at_square(*board, to);

    //from and to are same squares
    if (strcmp(from, to) == 0) { return false; }

    //no piece at square
    if (piece_to_move == 0) { return false; }

    //not your turn
    const char source_color = (piece_to_move & COLOR_MASK) >> 3;
    if (source_color != (board->flags & 0b1)) { return false; }

    //captures own piece
    const char dest_color = (piece_at_dest & COLOR_MASK) >> 3;
    if ((piece_at_dest & PIECE_MASK) != EMPTY && source_color == dest_color) { return false; }

    //trying to move opponents piece
    if (board->flags & 0b1 != (piece_to_move & COLOR_MASK) >> 3) { return false; }

    const char fileFrom = from[0] - 'a';
    const char rankFrom = from[1] - '1';
    const char fileTo = to[0] - 'a';
    const char rankTo = to[1] - '1';

    bool ok = false;
    //check if move is allowed
    if ((piece_to_move & PIECE_MASK) == PAWN) {
        ok = try_move_pawn(board, piece_to_move, fileFrom, rankFrom, fileTo, rankTo);
    } else if ((piece_to_move & PIECE_MASK) == ROOK) {
        ok = try_move_rook(board, piece_to_move, fileFrom, rankFrom, fileTo, rankTo);
    } else if ((piece_to_move & PIECE_MASK) == KNIGHT) {
        ok = try_move_knight(board, piece_to_move, fileFrom, rankFrom, fileTo, rankTo);
    } else if ((piece_to_move & PIECE_MASK) == BISHOP) {
        ok = try_move_bishop(board, piece_to_move, fileFrom, rankFrom, fileTo, rankTo);
    } else if ((piece_to_move & PIECE_MASK) == QUEEN) {
    } else if ((piece_to_move & PIECE_MASK) == KING) {
    }
    if (ok) { board->flags ^= 0b1; } else { return false; }

    return true;
}

void set_piece(ChessBoard *board, const char piece, const char rank, const char file) {
    unsigned int clearMask = 0b11111111111111111111111111111111;
    const unsigned int clears = 0b1111 << ((7 - file) * 4);
    clearMask ^= clears;
    board->board[rank] &= clearMask;
    board->board[rank] |= piece << ((7 - file) * 4);
}

void set_empty(ChessBoard *board, const char rank, const char file) {
    unsigned int clearMask = 0b11111111111111111111111111111111;
    const unsigned int clears = 0b1111 << ((7 - file) * 4);
    clearMask ^= clears;
    board->board[rank] &= clearMask;
}

bool is_empty(const ChessBoard board, const char rank, const char file) {
    return (get_board_at(board, rank, file) & PIECE_MASK) == EMPTY;
}
