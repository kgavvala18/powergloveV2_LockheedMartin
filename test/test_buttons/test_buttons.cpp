#include <unity.h>
#include "buttons.h"
#include <stdio.h>

// Macro: define a test function named test_<ID> for button index IDX
#define BUTTON_TEST(IDX, ID)                                             \
void test_##ID(void) {                                                   \
    char msg[64];                                                        \
    snprintf(msg, sizeof(msg), "%s", buttonNames[IDX]);                  \
    TEST_ASSERT_EQUAL_INT_MESSAGE(                                       \
        IDX,                                                             \
        matchButton(buttonSignatures[IDX]),                              \
        msg                                                               \
    );                                                                   \
}

// “No button” case
void test_No_Button(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        -1,
        matchButton(button_none),
        "No button pressed"
    );
}

// Generate one test per button, using sanitized IDs:
BUTTON_TEST(0,  Button_0)
BUTTON_TEST(1,  Button_1)
BUTTON_TEST(2,  Button_2)
BUTTON_TEST(3,  Button_3)
BUTTON_TEST(4,  Button_4)
BUTTON_TEST(5,  Button_5)
BUTTON_TEST(6,  Button_6)
BUTTON_TEST(7,  Button_7)
BUTTON_TEST(8,  Button_8)
BUTTON_TEST(9,  Button_9)
BUTTON_TEST(10, Button_Prog)
BUTTON_TEST(11, Button_Enter)
BUTTON_TEST(12, Left_Arrow)
BUTTON_TEST(13, Up_Arrow)
BUTTON_TEST(14, Right_Arrow)
BUTTON_TEST(15, Down_Arrow)
BUTTON_TEST(16, Button_Center)
BUTTON_TEST(17, Button_Select)
BUTTON_TEST(18, Button_Start)
BUTTON_TEST(19, Button_B)
BUTTON_TEST(20, Button_A)

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_No_Button);

    RUN_TEST(test_Button_0);
    RUN_TEST(test_Button_1);
    RUN_TEST(test_Button_2);
    RUN_TEST(test_Button_3);
    RUN_TEST(test_Button_4);
    RUN_TEST(test_Button_5);
    RUN_TEST(test_Button_6);
    RUN_TEST(test_Button_7);
    RUN_TEST(test_Button_8);
    RUN_TEST(test_Button_9);
    RUN_TEST(test_Button_Prog);
    RUN_TEST(test_Button_Enter);
    RUN_TEST(test_Left_Arrow);
    RUN_TEST(test_Up_Arrow);
    RUN_TEST(test_Right_Arrow);
    RUN_TEST(test_Down_Arrow);
    RUN_TEST(test_Button_Center);
    RUN_TEST(test_Button_Select);
    RUN_TEST(test_Button_Start);
    RUN_TEST(test_Button_B);
    RUN_TEST(test_Button_A);

    return UNITY_END();
}
