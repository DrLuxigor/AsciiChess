#include <stdio.h>
#include "chess.h"

int main(void) {
    printf("Hello, World!\n");
    const ChessBoard board = init_chessboard();
    for (int row = 0; row < 8; row++) {
        printf("%08x\n", board.board[row]);
    }
    return 0;
}
