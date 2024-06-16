#include <stdio.h>
#include "chess.h"
#include <string.h>

int main(void) {
    ChessBoard board = init_chessboard();
    move(&board, "e2", "e4");
    move(&board, "e7", "e5");
    move(&board, "d2", "d4");
    move(&board, "e5", "d4");
    move(&board, "e4", "e5");
    move(&board, "f8", "d6");
    move(&board, "e5", "d6");
    move(&board, "a7", "a5");
    move(&board, "a2", "a4");
    move(&board, "a8", "a6");
    move(&board, "g1", "f3");
    move(&board, "h7", "h6");
    move(&board, "f3", "d4");
    print_board(board);
    return 0;
}
