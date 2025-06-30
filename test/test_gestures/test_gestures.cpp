#include <unity.h>
#include "gesture.h"

// Macro to generate one test per bitmask/gesture
#define MAKE_TEST(_mask, _gesture)                                                   \
    void test_##_gesture(void)                                                       \
    {                                                                                \
        float thumb = ((_mask & 0x10) ? THUMB_THRES + 1.0f : THUMB_THRES - 1.0f);    \
        float index = ((_mask & 0x08) ? INDEX_THRES + 1.0f : INDEX_THRES - 1.0f);    \
        float middle = ((_mask & 0x04) ? MIDDLE_THRES + 1.0f : MIDDLE_THRES - 1.0f); \
        float ring = ((_mask & 0x02) ? RING_THRES + 1.0f : RING_THRES - 1.0f);       \
        float pinky = ((_mask & 0x01) ? PINKY_THRES + 1.0f : PINKY_THRES - 1.0f);    \
        TEST_ASSERT_EQUAL_INT_MESSAGE(                                               \
            _gesture,                                                                \
            gesture(thumb, index, middle, ring, pinky),                              \
            #_gesture);                                                              \
    }

extern "C"
{
    void setUp(void)
    {
        // Optional: Initialize shared test state here
    }

    void tearDown(void)
    {
        // Optional: Clean up test state here
    }
}

// five fingers
MAKE_TEST(0b11111, TIMRP)

// four fingers
MAKE_TEST(0b11110, TIMR)
MAKE_TEST(0b10111, TMRP)
MAKE_TEST(0b11011, TIRP)
MAKE_TEST(0b11101, TIMP)
MAKE_TEST(0b01111, IMRP)

// three fingers
MAKE_TEST(0b11100, TIM)
MAKE_TEST(0b11010, TIR)
MAKE_TEST(0b11001, TIP)
MAKE_TEST(0b10110, TMR)
MAKE_TEST(0b10101, TMP)
MAKE_TEST(0b10011, TRP)
MAKE_TEST(0b01110, IMR)
MAKE_TEST(0b01101, IMP)
MAKE_TEST(0b01011, IRP)
MAKE_TEST(0b00111, MRP)

// two fingers
MAKE_TEST(0b11000, TI)
MAKE_TEST(0b10100, TM)
MAKE_TEST(0b10010, TR)
MAKE_TEST(0b10001, TP)
MAKE_TEST(0b01100, IM)
MAKE_TEST(0b01010, IR)
MAKE_TEST(0b01001, IP)
MAKE_TEST(0b00110, MR)
MAKE_TEST(0b00101, MP)
MAKE_TEST(0b00011, RP)

// one finger
MAKE_TEST(0b10000, T)
MAKE_TEST(0b01000, I)
MAKE_TEST(0b00100, M)
MAKE_TEST(0b00010, R)
MAKE_TEST(0b00001, P)

// NONE
MAKE_TEST(0b00000, NONE)

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Invoke every generated test
    RUN_TEST(test_TIMRP);

    RUN_TEST(test_TIMR);
    RUN_TEST(test_TMRP);
    RUN_TEST(test_TIRP);
    RUN_TEST(test_TIMP);
    RUN_TEST(test_IMRP);

    RUN_TEST(test_TIM);
    RUN_TEST(test_TIR);
    RUN_TEST(test_TIP);
    RUN_TEST(test_TMR);
    RUN_TEST(test_TMP);
    RUN_TEST(test_TRP);
    RUN_TEST(test_IMR);
    RUN_TEST(test_IMP);
    RUN_TEST(test_IRP);
    RUN_TEST(test_MRP);

    RUN_TEST(test_TI);
    RUN_TEST(test_TM);
    RUN_TEST(test_TR);
    RUN_TEST(test_TP);
    RUN_TEST(test_IM);
    RUN_TEST(test_IR);
    RUN_TEST(test_IP);
    RUN_TEST(test_MR);
    RUN_TEST(test_MP);
    RUN_TEST(test_RP);

    RUN_TEST(test_T);
    RUN_TEST(test_I);
    RUN_TEST(test_M);
    RUN_TEST(test_R);
    RUN_TEST(test_P);

    RUN_TEST(test_NONE);

    return UNITY_END();
}
