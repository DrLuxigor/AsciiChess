#include <stdio.h>
#include "chess.h"
#include <string.h>

int main(void) {
    ChessBoard board = init_chessboard();
    print_board(board);
    move(&board, "e2", "e4");
    move(&board, "e7", "e5");
    move(&board, "d2", "d4");
    print_board(board);
    return 0;
}
