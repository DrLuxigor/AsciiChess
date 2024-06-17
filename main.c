#include <stdio.h>
#include "chess.h"
#include <string.h>

int main(void) {
    ChessBoard board = init_chessboard();
    move(&board, "d2", "d4");
    move(&board, "e7", "e5");
    move(&board, "d4", "d5");
    move(&board, "c7", "c5");
    move(&board, "d5", "c6");
    print_board(board);
    return 0;
}
