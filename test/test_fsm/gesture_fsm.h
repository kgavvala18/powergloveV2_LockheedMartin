// src/gesture_fsm.h
#pragma once
#include "gesture.h"

// All the “side‑effects” your FSM can request:
enum Action {
  ACTION_NONE,
  ACTION_PRESS_LEFT,
  ACTION_RELEASE_LEFT,
  ACTION_PRESS_RIGHT,
  ACTION_RELEASE_RIGHT,
  ACTION_ENABLE_LASER,
  ACTION_DISABLE_LASER,
  ACTION_TOGGLE_MOUSE,
  ACTION_SNIP,          // Win+Shift+S
  ACTION_ALT_TAB,
  ACTION_ALT_SHIFT_TAB,
  ACTION_SCROLL_UP,
  ACTION_SCROLL_DOWN,
  ACTION_ZOOM_IN,
  ACTION_ZOOM_OUT,
};

static constexpr Gestures LEFT_CLICK_GESTURE        = I;
static constexpr Gestures RIGHT_CLICK_GESTURE       = M;
static constexpr Gestures DRAG_GESTURE              = TI;
static constexpr Gestures LASER_GESTURE             = T;
static constexpr Gestures DISABLE_MOUSE_GESTURE     = TIMRP;
static constexpr Gestures SNIP_GESTURE              = TMRP;
// static constexpr Gestures ALT_F4_GESTURE          = TIRP;  // (if you ever re‑enable)
//
static constexpr Gestures ALT_TAB_GESTURE           = MRP;
static constexpr Gestures ALT_SHIFT_TAB_GESTURE     = IRP;
static constexpr Gestures ZOOM_GESTURE              = TP;
static constexpr Gestures SCROLL_GESTURE            = TRP;

// Your FSM’s states:
enum GestureState {
  IDLE,
  LEFT_CLICK_EVENT,
  RIGHT_CLICK_EVENT,
  DRAG_EVENT,
  LASER,
  SNIP,
  ALT_TAB,
  ALT_SHIFT_TAB,
  DISABLE_MOUSE,
  ZOOM,
  SCROLL
};

// The result of one tick through the FSM:
struct FsmResult {
  GestureState nextState;
  Action      action;
  bool        mouseEnabled;
  bool        toggleMouse;
};

/**
 * Run one step of the gesture state machine.
 *
 * @param prevState     the state the machine was in last tick
 * @param currentGest   the gesture() output this tick
 * @param ax            filtered accelerometer X (for scroll/zoom)
 * @param mouseEnabled  current mouseEnabled flag
 * @param toggleMouse   current toggleMouse flag
 * @returns             next state, the action to perform, and new flags
 */
FsmResult handleGestureFsm(
    GestureState prevState,
    Gestures     currentGest,
    float        ax,
    bool         mouseEnabled,
    bool         toggleMouse
);
