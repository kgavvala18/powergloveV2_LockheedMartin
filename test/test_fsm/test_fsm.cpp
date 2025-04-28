#include <unity.h>
#include "gesture_fsm.h"

// IDLE → LEFT‑CLICK
void test_idle_to_left_click(void) {
    auto r = handleGestureFsm(IDLE, LEFT_CLICK_GESTURE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(LEFT_CLICK_EVENT, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_PRESS_LEFT, r.action);
    TEST_ASSERT_FALSE(r.mouseEnabled);
    TEST_ASSERT_FALSE(r.toggleMouse);
}

// LEFT‑CLICK → IDLE on NONE
void test_left_click_release_on_none(void) {
    auto r = handleGestureFsm(LEFT_CLICK_EVENT, NONE, 0.0f, false, false);
    TEST_ASSERT_EQUAL(IDLE, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_RELEASE_LEFT, r.action);
    TEST_ASSERT_TRUE(r.mouseEnabled);
}

// IDLE → RIGHT‑CLICK
void test_idle_to_right_click(void) {
    auto r = handleGestureFsm(IDLE, RIGHT_CLICK_GESTURE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(RIGHT_CLICK_EVENT, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_PRESS_RIGHT, r.action);
    TEST_ASSERT_FALSE(r.mouseEnabled);
}

// RIGHT‑CLICK → IDLE on NONE
void test_right_click_release_on_none(void) {
    auto r = handleGestureFsm(RIGHT_CLICK_EVENT, NONE, 0.0f, false, false);
    TEST_ASSERT_EQUAL(IDLE, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_RELEASE_RIGHT, r.action);
    TEST_ASSERT_TRUE(r.mouseEnabled);
}

// IDLE → DRAG
void test_idle_to_drag(void) {
    auto r = handleGestureFsm(IDLE, DRAG_GESTURE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(DRAG_EVENT, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_PRESS_LEFT, r.action);
    TEST_ASSERT_TRUE(r.mouseEnabled);
}

// DRAG → IDLE on NONE
void test_drag_release_on_none(void) {
    auto r = handleGestureFsm(DRAG_EVENT, NONE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(IDLE, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_RELEASE_LEFT, r.action);
    TEST_ASSERT_TRUE(r.mouseEnabled);
}

// IDLE → LASER
void test_idle_to_laser(void) {
    auto r = handleGestureFsm(IDLE, LASER_GESTURE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(LASER, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_ENABLE_LASER, r.action);
    TEST_ASSERT_FALSE(r.mouseEnabled);
}

// LASER → IDLE on NONE
void test_laser_release_on_none(void) {
    auto r = handleGestureFsm(LASER, NONE, 0.0f, false, false);
    TEST_ASSERT_EQUAL(IDLE, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_DISABLE_LASER, r.action);
    TEST_ASSERT_TRUE(r.mouseEnabled);
}

// IDLE → SNIP
void test_idle_to_snip(void) {
    auto r = handleGestureFsm(IDLE, SNIP_GESTURE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(SNIP, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_SNIP, r.action);
    TEST_ASSERT_TRUE(r.mouseEnabled);
}

// SNIP → IDLE on NONE
void test_snip_release_on_none(void) {
    auto r = handleGestureFsm(SNIP, NONE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(IDLE, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_NONE, r.action);
    TEST_ASSERT_TRUE(r.mouseEnabled);
}

// IDLE → ALT‑TAB
void test_idle_to_alt_tab(void) {
    auto r = handleGestureFsm(IDLE, ALT_TAB_GESTURE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(ALT_TAB, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_ALT_TAB, r.action);
    TEST_ASSERT_TRUE(r.mouseEnabled);
}

// ALT‑TAB → IDLE on NONE
void test_alt_tab_release_on_none(void) {
    auto r = handleGestureFsm(ALT_TAB, NONE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(IDLE, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_NONE, r.action);
    TEST_ASSERT_TRUE(r.mouseEnabled);
}

// IDLE → ALT‑SHIFT‑TAB
void test_idle_to_alt_shift_tab(void) {
    auto r = handleGestureFsm(IDLE, ALT_SHIFT_TAB_GESTURE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(ALT_SHIFT_TAB, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_ALT_SHIFT_TAB, r.action);
    TEST_ASSERT_TRUE(r.mouseEnabled);
}

// ALT‑SHIFT‑TAB → IDLE on NONE
void test_alt_shift_tab_release_on_none(void) {
    auto r = handleGestureFsm(ALT_SHIFT_TAB, NONE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(IDLE, r.nextState);
    TEST_ASSERT_EQUAL(ACTION_NONE, r.action);
    TEST_ASSERT_TRUE(r.mouseEnabled);
}

void test_toggle_mouse(void) {
    // 1) First disable gesture: IDLE → DISABLE_MOUSE, toggleMouse flips true
    auto r1 = handleGestureFsm(IDLE, DISABLE_MOUSE_GESTURE, 0.0f, true, false);
    TEST_ASSERT_EQUAL(DISABLE_MOUSE,      r1.nextState);
    TEST_ASSERT_EQUAL(ACTION_TOGGLE_MOUSE, r1.action);
    TEST_ASSERT_FALSE(r1.mouseEnabled);
    TEST_ASSERT_TRUE(r1.toggleMouse);
  
    // 2) Finish the gesture (NONE): DISABLE_MOUSE → IDLE, mouse still disabled
    auto r2 = handleGestureFsm(DISABLE_MOUSE, NONE, 0.0f, r1.mouseEnabled, r1.toggleMouse);
    TEST_ASSERT_EQUAL(IDLE,              r2.nextState);
    TEST_ASSERT_FALSE(r2.mouseEnabled);
    TEST_ASSERT_TRUE(r2.toggleMouse);
  
    // 3) Second disable gesture (toggles back on): IDLE → DISABLE_MOUSE, toggleMouse flips false
    auto r3 = handleGestureFsm(IDLE, DISABLE_MOUSE_GESTURE, 0.0f, r2.mouseEnabled, r2.toggleMouse);
    TEST_ASSERT_EQUAL(DISABLE_MOUSE,      r3.nextState);
    TEST_ASSERT_EQUAL(ACTION_TOGGLE_MOUSE, r3.action);
    TEST_ASSERT_FALSE(r3.mouseEnabled);
    TEST_ASSERT_FALSE(r3.toggleMouse);
  
    // 4) Finish the “re‑enable” gesture: DISABLE_MOUSE → IDLE, mouse now re‑enabled
    auto r4 = handleGestureFsm(DISABLE_MOUSE, NONE, 0.0f, r3.mouseEnabled, r3.toggleMouse);
    TEST_ASSERT_EQUAL(IDLE,    r4.nextState);
    TEST_ASSERT_TRUE(r4.mouseEnabled);
    TEST_ASSERT_FALSE(r4.toggleMouse);
  }

// IDLE → SCROLL UP
void test_scroll_up_and_back(void) {
    auto r1 = handleGestureFsm(IDLE, SCROLL_GESTURE,  5.0f, true, false);
    TEST_ASSERT_EQUAL(SCROLL, r1.nextState);
    TEST_ASSERT_EQUAL(ACTION_SCROLL_UP, r1.action);
    TEST_ASSERT_FALSE(r1.mouseEnabled);

    auto r2 = handleGestureFsm(SCROLL, NONE, 0.0f, r1.mouseEnabled, false);
    TEST_ASSERT_EQUAL(IDLE, r2.nextState);
    TEST_ASSERT_EQUAL(ACTION_NONE, r2.action);
    TEST_ASSERT_TRUE(r2.mouseEnabled);
}

// IDLE → SCROLL DOWN
void test_scroll_down_and_back(void) {
    auto r1 = handleGestureFsm(IDLE, SCROLL_GESTURE, -3.0f, true, false);
    TEST_ASSERT_EQUAL(SCROLL, r1.nextState);
    TEST_ASSERT_EQUAL(ACTION_SCROLL_DOWN, r1.action);
    TEST_ASSERT_FALSE(r1.mouseEnabled);

    auto r2 = handleGestureFsm(SCROLL, NONE, 0.0f, r1.mouseEnabled, false);
    TEST_ASSERT_EQUAL(IDLE, r2.nextState);
    TEST_ASSERT_EQUAL(ACTION_NONE, r2.action);
    TEST_ASSERT_TRUE(r2.mouseEnabled);
}

// IDLE → ZOOM IN
void test_zoom_in_and_back(void) {
    auto r1 = handleGestureFsm(IDLE, ZOOM_GESTURE,  4.0f, true, false);
    TEST_ASSERT_EQUAL(ZOOM, r1.nextState);
    TEST_ASSERT_EQUAL(ACTION_ZOOM_IN, r1.action);
    TEST_ASSERT_FALSE(r1.mouseEnabled);

    auto r2 = handleGestureFsm(ZOOM, NONE, 0.0f, r1.mouseEnabled, false);
    TEST_ASSERT_EQUAL(IDLE, r2.nextState);
    TEST_ASSERT_EQUAL(ACTION_NONE, r2.action);
    TEST_ASSERT_TRUE(r2.mouseEnabled);
}

// IDLE → ZOOM OUT
void test_zoom_out_and_back(void) {
    auto r1 = handleGestureFsm(IDLE, ZOOM_GESTURE, -3.0f, true, false);
    TEST_ASSERT_EQUAL(ZOOM, r1.nextState);
    TEST_ASSERT_EQUAL(ACTION_ZOOM_OUT, r1.action);
    TEST_ASSERT_FALSE(r1.mouseEnabled);

    auto r2 = handleGestureFsm(ZOOM, NONE, 0.0f, r1.mouseEnabled, false);
    TEST_ASSERT_EQUAL(IDLE, r2.nextState);
    TEST_ASSERT_EQUAL(ACTION_NONE, r2.action);
    TEST_ASSERT_TRUE(r2.mouseEnabled);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();

    RUN_TEST(test_idle_to_left_click);
    RUN_TEST(test_left_click_release_on_none);
    RUN_TEST(test_idle_to_right_click);
    RUN_TEST(test_right_click_release_on_none);
    RUN_TEST(test_idle_to_drag);
    RUN_TEST(test_drag_release_on_none);
    RUN_TEST(test_idle_to_laser);
    RUN_TEST(test_laser_release_on_none);
    RUN_TEST(test_idle_to_snip);
    RUN_TEST(test_snip_release_on_none);
    RUN_TEST(test_idle_to_alt_tab);
    RUN_TEST(test_alt_tab_release_on_none);
    RUN_TEST(test_idle_to_alt_shift_tab);
    RUN_TEST(test_alt_shift_tab_release_on_none);
    RUN_TEST(test_toggle_mouse);
    RUN_TEST(test_scroll_up_and_back);
    RUN_TEST(test_scroll_down_and_back);
    RUN_TEST(test_zoom_in_and_back);
    RUN_TEST(test_zoom_out_and_back);

    return UNITY_END();
}
