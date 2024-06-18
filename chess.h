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

#define WHITE 0b0
#define BLACK 0b1

typedef struct {
    unsigned int turn : 1;
    unsigned int w_short_castle: 1;
    unsigned int w_long_castle: 1;
    unsigned int b_short_castle: 1;
    unsigned int b_long_castle: 1;
    unsigned int en_passant_file: 4; //0-15 0-7 + 1 for not available :(
    unsigned int m50_rule: 6; //0-63 (max 50)
    //17 flag bits remaining. (make short?)
    unsigned int board[8];
} ChessBoard;

ChessBoard init_chessboard();
void print_board(const ChessBoard board);
bool move(ChessBoard* board, const char* from, const char* to);
#endif //CHESS_H
