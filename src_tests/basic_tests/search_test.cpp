
#include "ucci.h"
#include "search.h"
#include "gtest/gtest.h"


class SearchFixture : public ::testing::Test {
protected:
    virtual void TearDown() {
    }

    virtual void SetUp() {
    }

public:
    SearchFixture() : Test() {
        chess_search = new Search();
        chess_search->InitSearch();
    }

    ~SearchFixture() {
        delete chess_search;
    }

    Search *chess_search;

    void StringToUcciStruct(const char *line_str, ucci_comm_struct *ucs_command) {
        if (strncmp(line_str, "position ", 9) == 0) {
            long coord_list[256]={0};
            line_str += 9;
            if (strncmp(line_str, "fen ", 4) == 0) {
                ucs_command->position.fen_str = line_str + 4;
            } else {
                return;
            }

            line_str = strstr(line_str, " moves ");
            ucs_command->position.move_num = 0;

            if (line_str != NULL) {
                line_str += 7;
                ucs_command->position.move_num = ((strlen(line_str) + 1) / 5);

                for (int i = 0; i < ucs_command->position.move_num; ++i) {
                    coord_list[i] = *(long *) line_str;
                    line_str += 5;
                }

                ucs_command->position.coord_list = coord_list;
            }
        }
    };
};

TEST_F(SearchFixture, move_check) {
    ucci_comm_struct test_struct;
    const char *test_position = "position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1 moves h2e2 h7e7";
    StringToUcciStruct(test_position, &test_struct);
    /*
    printf("\n%s\n", test_struct.position.fen_str);
    printf("%d\n", test_struct.position.move_num);
    for (int k = 0; k<test_struct.position.move_num; k++)
    {
        char * str = (char *)(&(test_struct.position.coord_list[k]));
        printf("%c", str[0]);
        printf("%c", str[1]);
        printf("%c", str[2]);
        printf("%c\n", str[3]);
    }
     */
    chess_search->position_.FenToBoard(test_struct.position.fen_str,test_struct.position.move_num,test_struct.position.coord_list);
    //printf("%d", chess_search->ThinkMoveDepth(7));
    //chess_search->ThinkMoveDepth(7);
    EXPECT_EQ(42436, chess_search->ThinkMoveDepth(7));
    const char *test_position2 = "position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1 moves b0c2 c6c5";
    ucci_comm_struct test_struct2;
    StringToUcciStruct(test_position2, &test_struct2);
    chess_search->position_.FenToBoard(test_struct2.position.fen_str,test_struct2.position.move_num,test_struct2.position.coord_list);
    EXPECT_EQ(43466, chess_search->ThinkMoveDepth(7));
}
