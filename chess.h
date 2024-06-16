//
// Created by lukas on 16/06/2024.
//

#ifndef CHESS_H
#define CHESS_H

//[pawn, rook, knight, bishop, queen, king, color]
//
// 0 0 0 pawn
// 0 0 1 rook
// 0 1 0 knight
// 0 1 1 bishop
// 1 0 0 queen
// 1 0 1 king
// 1 1 0
// 1 1 1

#define EMPTY 0
#define PAWN 0x1
#define ROOK 0x2
#define KNIGHT 0x3
#define BISHOP 0x4
#define QUEEN 0x5
#define KING 0x6

#define PIECE_MASK 0b111
#define COLOR_MASK 0b1000

typedef struct {
    unsigned int flags;
    unsigned int board[8];
} ChessBoard;

ChessBoard init_chessboard();
char get_board_at_square(const ChessBoard board,const char* square);
char get_board_at(const ChessBoard board, const char rank, const char file);
void print_board(const ChessBoard board);
bool move(ChessBoard* board, const char* from, const char* to);
void set_piece(ChessBoard* board, char piece, char rank, char file);
void set_empty(ChessBoard* board, char rank, char file);
bool is_empty(const ChessBoard board, char rank, char file);
#endif //CHESS_H
