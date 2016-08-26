#include <stdio.h>

#include "src/ucci.h"
#include "src/search.h"


int main(int argc, char *argv[]) {
    ucci_comm_enum ucci_enum = UCCI_COMM_NONE;
    ucci_comm_struct ucci_struct;
    if (boot_line() == UCCI_COMM_UCCI) {
        Search chess_search;
        printf("id name happygg\n");
        fflush(stdout);
        printf("id copyright 2015\n");
        fflush(stdout);
        printf("id anthor Chaozz\n");
        fflush(stdout);
        printf("ucciok\n");
        fflush(stdout);
        while (ucci_enum != UCCI_COMM_QUIT) {
            ucci_enum = idle_line(&ucci_struct);
            switch (ucci_enum) {
                case UCCI_COMM_ISREADY:
                    printf("readyok\n");
                    fflush(stdout);
                    break;
                case UCCI_COMM_STOP:
                    printf("nobestmove\n");
                    fflush(stdout);
                    break;
                case UCCI_COMM_BANMOVES:
                    break;
                case UCCI_COMM_SETOPTION:
                    switch (ucci_struct.option.uo_type) {
                        case UCCI_OPTION_BATCH:
                            break;
                        case UCCI_OPTION_NEWGAME:
                            chess_search.InitSearch();
                            break;
                        default:
                            break;
                    }

                    break;
                case UCCI_COMM_POSITION:
                    fflush(stdout);
                    break;
                case UCCI_COMM_GO:
                case UCCI_COMM_GOPONDER:
                    if (ucci_struct.search.ut_mode == UCCI_DEPTH) {
                        chess_search.ThinkMoveDepth(ucci_struct.search.depth_time.depth);
                    } else if (ucci_struct.search.ut_mode == UCCI_TIME) {
                        chess_search.ThinkMoveTime(ucci_struct.search.depth_time.time);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    printf("bye\n");
    fflush(stdout);
    return 0;
}
