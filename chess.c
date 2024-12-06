//
// Created by lukas on 16/06/2024.
//

#include "chess.h"

#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

const signed char knight_moves[8][2] = {
    { 2,  1},
    { 2, -1},
    {-2,  1},
    {-2, -1},
    { 1,  2},
    { 1, -2},
    {-1,  2},
    {-1, -2}
};

const signed char rook_moves[4][2] = {
    {1, 0},
    {-1, 0},
    {0, 1},
    {0, -1}
};

const signed char bishop_moves[4][2] = {
    {1,1},
    {1, -1},
    {-1, 1},
    {-1, -1}
};

ChessBoard init_chessboard() {
    ChessBoard board;
    board.turn = 0;
    board.w_short_castle = 1;
    board.w_long_castle = 1;
    board.b_short_castle = 1;
    board.b_long_castle = 1;
    board.en_passant_file = 0;
    board.m50_rule = 0;
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

char get_board_at(const ChessBoard board, const char rank, const char file) {
    assert(rank >= 0 && rank < 8);
    assert(file >= 0 && file < 8);
    return board.board[rank] >> ((7 - file) * 4) & 0b1111;
}

char get_board_at_square(const ChessBoard board, const char *square) {
    assert(square[0] >= 'a' && square[0] <= 'h');
    assert(square[1] >= '1' && square[1] <= '8');
    const char file = square[0] - 'a';
    const char rank = square[1] - '1';
    return get_board_at(board, rank, file);
}

bool in_bounds(const signed char rank, const signed char file) {
    return rank >= 0 && rank < 8 && file >= 0 && file < 8;
}

void set_piece(ChessBoard *board, char piece, const char rank, const char file) {
    if ((piece & PIECE_MASK) == PAWN && (rank == 0 || rank == 7)) {
        piece = (piece & COLOR_MASK) | QUEEN;
    }
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

void find_piece(const ChessBoard board, const char piece, char* rank, char* file) {
    for (unsigned char i = 0; i < 8; i++) {
        for (unsigned char j = 0; j < 8; j++) {
            if (get_board_at(board, i, j) == piece) {
                *rank = i;
                *file = j;
                return;
            }
        }
    }
    *rank = -1;
    *file = -1;
}

bool is_empty(const ChessBoard board, const char rank, const char file) {
    return (get_board_at(board, rank, file) & PIECE_MASK) == EMPTY;
}

bool square_attacked_by_pawn(const ChessBoard board, const char rank, const char file, const char byColor) {
    if(byColor) {
        //blacks pawns cant be on 8th rank
        if (rank + 1 > 6)
            return false;
        //black attacks white
        if (file + 1 < 8 && get_board_at(board, rank+1, file+1) == (PAWN | COLOR_MASK) ) {
            return true;
        }
        if (file - 1 >= 0 && get_board_at(board, rank+1, file-1) == (PAWN | COLOR_MASK) ) {
            return true;
        }
    } else {
        //white attacks black
        if (rank - 1 < 1)
            return false;
        //black attacks white
        if (file + 1 < 8 && get_board_at(board, rank-1, file+1) == PAWN) {
            return true;
        }
        if (file - 1 >= 0 && get_board_at(board, rank-1, file-1) == PAWN) {
            return true;
        }
    }
    return false;
}

bool square_attacked_by_knight(const ChessBoard board, const char rank, const char file, const char byColor) {
    for (int i = 0; i < 8; i++) {
        const signed char target_rank = rank + knight_moves[i][0];
        const signed char target_file = file + knight_moves[i][1];
        if (target_rank < 0 || target_rank > 7 || target_file < 0 || target_file > 7)
            continue;
        if (get_board_at(board, target_rank, target_file) == (KNIGHT | byColor << 3))
            return true;
    }
    return false;
}

bool square_attacked_by_rook_q_k(const ChessBoard board, const char rank, const char file, const char byColor) {
    for(char i = 0; i < 4; i++) {
        signed char steps = 1;
        const signed char dir_rank = rook_moves[i][0];
        const signed char dir_file = rook_moves[i][1];
        while(in_bounds(rank + dir_rank*steps, file + dir_file*steps)) {
            const char piece = get_board_at(board, rank + dir_rank*steps, file + dir_file*steps);
            if (piece == EMPTY) {
                steps++;
                continue;
            }
            if (piece == (ROOK | byColor << 3) || piece == (QUEEN | byColor << 3))
                return true;
            if (steps == 1 && piece == (KING | byColor << 3))
                return true;
            break;
        }
    }
    return false;
}

bool square_attacked_by_bishop_q_k(const ChessBoard board, const char rank, const char file, const char byColor) {
    for(char i = 0; i < 4; i++) {
        signed char steps = 1;
        const signed char dir_rank = bishop_moves[i][0];
        const signed char dir_file = bishop_moves[i][1];
        while(in_bounds(rank + dir_rank*steps, file + dir_file*steps)) {
            const char piece = get_board_at(board, rank + dir_rank*steps, file + dir_file*steps);
            if (piece == EMPTY) {
                steps++;
                continue;
            }
            if (piece == (BISHOP | byColor << 3) || piece == (QUEEN | byColor << 3))
                return true;
            if (steps == 1 && piece == (KING | byColor << 3))
                return true;
            break;
        }
    }
    return false;
}

bool square_attacked(const ChessBoard board, const char rank, const char file, const char byColor) {
    if(square_attacked_by_pawn(board, rank, file, byColor))
        return true;
    if(square_attacked_by_knight(board, rank, file, byColor))
        return true;
    if(square_attacked_by_rook_q_k(board, rank, file, byColor))
        return true;
    if(square_attacked_by_bishop_q_k(board, rank, file, byColor))
        return true;
    return false;
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

char* get_repr_str(const ChessBoard board, const char rank, const char file) {
    const char piece = get_board_at(board, rank, file);
    if ((piece & PIECE_MASK) == EMPTY) { return " "; }
    if ((piece & PIECE_MASK) == PAWN) { return piece & COLOR_MASK ? "\033[0;35mp\033[0;37m" : "p"; }
    if ((piece & PIECE_MASK) == KNIGHT) { return piece & COLOR_MASK ? "\033[0;35mN\033[0;37m" : "N"; }
    if ((piece & PIECE_MASK) == ROOK) { return piece & COLOR_MASK ? "\033[0;35mR\033[0;37m" : "R"; }
    if ((piece & PIECE_MASK) == BISHOP) { return piece & COLOR_MASK ? "\033[0;35mB\033[0;37m" : "B"; }
    if ((piece & PIECE_MASK) == QUEEN) { return piece & COLOR_MASK ? "\033[0;35mQ\033[0;37m" : "Q"; }
    if ((piece & PIECE_MASK) == KING) { return piece & COLOR_MASK ? "\033[0;35mK\033[0;37m" : "K"; }
    return " ";
}

void print_board_better(const ChessBoard board) {

    printf("    a     b     c     d     e     f     g     h    \n");
    printf("  +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    printf("8 |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  | 8\n",
        get_repr_str(board, 7, 0),
        get_repr_str(board, 7, 1),
        get_repr_str(board, 7, 2),
        get_repr_str(board, 7, 3),
        get_repr_str(board, 7, 4),
        get_repr_str(board, 7, 5),
        get_repr_str(board, 7, 6),
        get_repr_str(board, 7, 7));
    printf("  +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    printf("7 |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  | 7\n",
        get_repr_str(board, 6, 0),
        get_repr_str(board, 6, 1),
        get_repr_str(board, 6, 2),
        get_repr_str(board, 6, 3),
        get_repr_str(board, 6, 4),
        get_repr_str(board, 6, 5),
        get_repr_str(board, 6, 6),
        get_repr_str(board, 6, 7));
    printf("  +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    printf("6 |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  | 6\n",
        get_repr_str(board, 5, 0),
        get_repr_str(board, 5, 1),
        get_repr_str(board, 5, 2),
        get_repr_str(board, 5, 3),
        get_repr_str(board, 5, 4),
        get_repr_str(board, 5, 5),
        get_repr_str(board, 5, 6),
        get_repr_str(board, 5, 7));
    printf("  +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    printf("5 |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  | 5\n",
        get_repr_str(board, 4, 0),
        get_repr_str(board, 4, 1),
        get_repr_str(board, 4, 2),
        get_repr_str(board, 4, 3),
        get_repr_str(board, 4, 4),
        get_repr_str(board, 4, 5),
        get_repr_str(board, 4, 6),
        get_repr_str(board, 4, 7));
    printf("  +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    printf("4 |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  | 4\n",
        get_repr_str(board, 3, 0),
        get_repr_str(board, 3, 1),
        get_repr_str(board, 3, 2),
        get_repr_str(board, 3, 3),
        get_repr_str(board, 3, 4),
        get_repr_str(board, 3, 5),
        get_repr_str(board, 3, 6),
        get_repr_str(board, 3, 7));
    printf("  +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    printf("3 |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  | 3\n",
        get_repr_str(board, 2, 0),
        get_repr_str(board, 2, 1),
        get_repr_str(board, 2, 2),
        get_repr_str(board, 2, 3),
        get_repr_str(board, 2, 4),
        get_repr_str(board, 2, 5),
        get_repr_str(board, 2, 6),
        get_repr_str(board, 2, 7));
    printf("  +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    printf("2 |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  | 2\n",
        get_repr_str(board, 1, 0),
        get_repr_str(board, 1, 1),
        get_repr_str(board, 1, 2),
        get_repr_str(board, 1, 3),
        get_repr_str(board, 1, 4),
        get_repr_str(board, 1, 5),
        get_repr_str(board, 1, 6),
        get_repr_str(board, 1, 7));
    printf("  +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    printf("1 |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  |  %s  | 1\n",
        get_repr_str(board, 0, 0),
        get_repr_str(board, 0, 1),
        get_repr_str(board, 0, 2),
        get_repr_str(board, 0, 3),
        get_repr_str(board, 0, 4),
        get_repr_str(board, 0, 5),
        get_repr_str(board, 0, 6),
        get_repr_str(board, 0, 7));
    printf("  +-----+-----+-----+-----+-----+-----+-----+-----+\n");
    printf("    a     b     c     d     e     f     g     h    \n");
}

bool try_move_pawn(ChessBoard *board, const char piece, const signed char fileFrom, const signed char rankFrom,
                   const signed char fileTo, const signed char rankTo) {
    //trying to move piece backward or sideways
    const bool black = piece & COLOR_MASK;
    if (black && rankFrom <= rankTo) { return false; }
    if (!black && rankFrom >= rankTo) { return false; }
    if (rankFrom == rankTo) { return false; }

    if (abs(rankTo - rankFrom) > 2)
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
        if (abs(fileFrom - fileTo) != 1) { return false; }

        if (!is_empty(*board, rankTo, fileTo)) {
            set_piece(board, piece, rankTo, fileTo);
            set_empty(board, rankFrom, fileFrom);
            return true;
        } else {
            if (board->en_passant_file == fileTo) {
                if (piece & COLOR_MASK && rankFrom == 3) {
                    //black
                    set_piece(board, piece, rankTo, fileTo);
                    set_empty(board, rankFrom, fileFrom);
                    set_empty(board, rankFrom, fileTo);
                    return true;
                }
                if (!(piece & COLOR_MASK) && rankFrom == 4) {
                    set_piece(board, piece, rankTo, fileTo);
                    set_empty(board, rankFrom, fileFrom);
                    set_empty(board, rankFrom, fileTo);
                    return true;
                }
            }
        }
    }
    return false;
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
        if (!is_empty(*board, rankFrom + i * rankDir, fileFrom + i * (fileDir)))
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
    if (rankOffset != 0 && fileOffset != 0) { return false; }
    assert(!(rankOffset == 0 && fileOffset == 0));

    const signed char rankDir = rankOffset == 0 ? 0 : rankOffset / abs(rankOffset);
    const signed char fileDir = fileOffset == 0 ? 0 : fileOffset / abs(fileOffset);

    for (int i = 1; i < abs(rankOffset); ++i) {
        if (!is_empty(*board, rankFrom + i * rankDir, fileFrom + i * (fileDir)))
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
    if (!((abs(rankOffset) == 2 && abs(fileOffset) == 1) || (abs(rankOffset) == 1 && abs(fileOffset) == 2))) {
        return false;
    }

    set_piece(board, piece, rankTo, fileTo);
    set_empty(board, rankFrom, fileFrom);
    return true;
}

bool try_move_queen(ChessBoard *board, const char piece, const char fileFrom, const char rankFrom,
                    const char fileTo, const char rankTo) {
    bool success = false;
    success = try_move_bishop(board, piece, fileFrom, rankFrom, fileTo, rankTo);
    if (!success) {
        success = try_move_rook(board, piece, fileFrom, rankFrom, fileTo, rankTo);
    }
    return success;
}

bool try_move_king(ChessBoard *board, const char piece, const char fileFrom, const char rankFrom,
                   const char fileTo, const char rankTo) {
    const signed char rankOffset = rankTo - rankFrom;
    const signed char fileOffset = fileTo - fileFrom;

    //castles
    if(rankOffset == 0 && abs(fileOffset) == 2) {
        if(rankFrom == 7 && fileOffset < 0 && board->b_long_castle) {
            //castle black long
            if(!square_attacked(*board, rankFrom, fileFrom, WHITE) &&
                !square_attacked(*board, rankFrom, fileFrom - 1, WHITE) &&
                !square_attacked(*board, rankFrom, fileTo, WHITE)) {
                set_empty(board, rankFrom, 0);
                set_empty(board, rankFrom, fileFrom);
                set_piece(board, piece, rankTo, fileTo);
                set_piece(board, COLOR_MASK | ROOK, rankTo, fileFrom - 1);
                return true;
            }
        } else if(rankFrom == 7 && fileOffset > 0 && board->b_short_castle) {
            //caste black short
            if(!square_attacked(*board, rankFrom, fileFrom, WHITE) &&
                !square_attacked(*board, rankFrom, fileFrom + 1, WHITE) &&
                !square_attacked(*board, rankFrom, fileTo, WHITE)) {
                set_empty(board, rankFrom, 7);
                set_empty(board, rankFrom, fileFrom);
                set_piece(board, piece, rankTo, fileTo);
                set_piece(board, COLOR_MASK | ROOK, rankTo, fileFrom + 1);
                return true;
            }
        } else if(rankFrom == 0 && fileOffset < 0 && board->w_long_castle) {
            //castle white long
            if(!square_attacked(*board, rankFrom, fileFrom, BLACK) &&
                !square_attacked(*board, rankFrom, fileFrom - 1, BLACK) &&
                !square_attacked(*board, rankFrom, fileTo, BLACK)) {
                set_empty(board, rankFrom, 0);
                set_empty(board, rankFrom, fileFrom);
                set_piece(board, piece, rankTo, fileTo);
                set_piece(board, ROOK, rankTo, fileFrom - 1);
                return true;
            }
        } else if (rankFrom == 0 && fileOffset > 0 && board->w_short_castle) {
            //castle white short
            if(!square_attacked(*board, rankFrom, fileFrom, BLACK) &&
                !square_attacked(*board, rankFrom, fileFrom + 1, BLACK) &&
                !square_attacked(*board, rankFrom, fileTo, BLACK)) {
                set_empty(board, rankFrom, 7);
                set_empty(board, rankFrom, fileFrom);
                set_piece(board, piece, rankTo, fileTo);
                set_piece(board, ROOK, rankTo, fileFrom + 1);
                return true;
            }
        }
    }

    //king can only move 1 square
    if (abs(rankOffset) > 1 || abs(fileOffset) > 1) { return false; }
    assert(rankOffset != 0 || fileOffset != 0);

    if (rankOffset != 0 && fileOffset != 0) {
        return try_move_bishop(board, piece, fileFrom, rankFrom, fileTo, rankTo);
    }
    return try_move_rook(board, piece, fileFrom, rankFrom, fileTo, rankTo);
}

bool move(ChessBoard *board, const char *from, const char *to) {
    printf("Move!\n Turn: %d", board->turn);
    ChessBoard* boardCopy = malloc(sizeof(ChessBoard));
    memcpy(boardCopy, board, sizeof(ChessBoard));

    const char piece_to_move = get_board_at_square(*board, from);
    const char piece_at_dest = get_board_at_square(*board, to);

    //from and to are same squares
    if (strcmp(from, to) == 0) { return false; }

    //no piece at square
    if (piece_to_move == 0) { return false; }

    //not your turn / moving opponents piece
    const char source_color = (piece_to_move & COLOR_MASK) >> 3;
    if (source_color != board->turn) { return false; }

    //captures own piece
    const char dest_color = (piece_at_dest & COLOR_MASK) >> 3;
    if ((piece_at_dest & PIECE_MASK) != EMPTY && source_color == dest_color) { return false; }

    const char fileFrom = from[0] - 'a';
    const char rankFrom = from[1] - '1';
    const char fileTo = to[0] - 'a';
    const char rankTo = to[1] - '1';

    bool ok = false;
    if ((piece_to_move & PIECE_MASK) == PAWN) {
        ok = try_move_pawn(board, piece_to_move, fileFrom, rankFrom, fileTo, rankTo);
    } else if ((piece_to_move & PIECE_MASK) == ROOK) {
        ok = try_move_rook(board, piece_to_move, fileFrom, rankFrom, fileTo, rankTo);
    } else if ((piece_to_move & PIECE_MASK) == KNIGHT) {
        ok = try_move_knight(board, piece_to_move, fileFrom, rankFrom, fileTo, rankTo);
    } else if ((piece_to_move & PIECE_MASK) == BISHOP) {
        ok = try_move_bishop(board, piece_to_move, fileFrom, rankFrom, fileTo, rankTo);
    } else if ((piece_to_move & PIECE_MASK) == QUEEN) {
        ok = try_move_queen(board, piece_to_move, fileFrom, rankFrom, fileTo, rankTo);
    } else if ((piece_to_move & PIECE_MASK) == KING) {
        ok = try_move_king(board, piece_to_move, fileFrom, rankFrom, fileTo, rankTo);
    }
    //TODO check if king of current move color is in check --> rollback move, return false
    char king_rank, king_file;
    find_piece(*board, KING | board->turn << 3, &king_rank, &king_file);
    printf("Checking if king attacked at %d %d", king_rank, king_file);
    const bool attacked = square_attacked(*board, king_rank, king_file, !board->turn);
    if(attacked) {
        printf("Attacked!!\n");
        memcpy(board, boardCopy, sizeof(ChessBoard));
        free(boardCopy);
        return false;
    }



    if (ok) {
        //Update board state
        board->turn ^= 0b1;

        //50 move rule
        if ((piece_to_move & PIECE_MASK) == PAWN || (piece_at_dest & PIECE_MASK) != EMPTY) {
            board->m50_rule = 0;
        } else {
            board->m50_rule++;
        }

        //Castling
        if ((piece_to_move & PIECE_MASK) == KING) {
            if (piece_to_move & COLOR_MASK) {
                board->b_short_castle = 0;
                board->b_long_castle = 0;
            } else {
                board->w_short_castle = 0;
                board->b_long_castle = 0;
            }
        }
        if (board->w_long_castle && get_board_at(*board, 0, 0) != ROOK) {
            board->w_long_castle = 0;
        }
        if (board->w_short_castle && get_board_at(*board, 0, 7) != ROOK) {
            board->w_short_castle = 0;
        }
        if (board->b_long_castle && get_board_at(*board, 7, 0) != (COLOR_MASK | ROOK)) {
            board->b_long_castle = 0;
        }
        if (board->b_short_castle && get_board_at(*board, 7, 7) != (COLOR_MASK | ROOK)) {
            board->b_short_castle = 0;
        }

        //En passant
        if ((piece_to_move & PIECE_MASK) == PAWN && abs(rankFrom - rankTo) == 2) {
            board->en_passant_file = fileTo;
        } else {
            board->en_passant_file = 0b1111;
        }
        return true;
    }
    return false;
}
