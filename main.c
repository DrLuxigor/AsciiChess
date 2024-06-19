#include <stdio.h>
#include "chess.h"
//#include <ncurses.h>
#include <string.h>

int main(void) {
    ChessBoard board = init_chessboard();
    char in1[5];
    char in2[5];
    print_board_better(board);
    while (true) {
        printf("Move from: ");
        if (fgets(in1, sizeof(in1), stdin) == NULL) {
            break;
        }
        in1[strcspn(in1, "\n")] = '\0';
        if (strcmp(in1, "quit") == 0) {
            break;
        }
        printf("Move to: ");
        if (fgets(in2, sizeof(in2), stdin) == NULL) {
            break;
        }
        in2[strcspn(in2, "\n")] = '\0';

        const bool ok = move(&board, in1, in2);
        if(ok) {
            print_board_better(board);
        } else {
           printf("invalid move\n");
        }
    }
    printf("Done");
    return 0;
}
