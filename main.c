#include <stdio.h>
#include "chess.h"
#include <string.h>

int main(void) {
    const ChessBoard board = init_chessboard();
    print_board(board);
    return 0;
}
