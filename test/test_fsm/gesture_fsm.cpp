// src/gesture_fsm.cpp
#include "gesture_fsm.h"

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

FsmResult handleGestureFsm(
    GestureState prev, Gestures g, float ax,
    bool mouseEnabled, bool toggleMouse)
{
  FsmResult r{prev, ACTION_NONE, mouseEnabled, toggleMouse};

  switch (prev)
  {
  case IDLE:
    if (g == DRAG_GESTURE)
    {
      r.action = ACTION_PRESS_LEFT;
      r.nextState = DRAG_EVENT;
    }
    else if (g == LEFT_CLICK_GESTURE)
    {
      r.action = ACTION_PRESS_LEFT;
      r.nextState = LEFT_CLICK_EVENT;
      r.mouseEnabled = false;
    }
    else if (g == RIGHT_CLICK_GESTURE)
    {
      r.action = ACTION_PRESS_RIGHT;
      r.nextState = RIGHT_CLICK_EVENT;
      r.mouseEnabled = false;
    }
    else if (g == LASER_GESTURE)
    {
      r.action = ACTION_ENABLE_LASER;
      r.nextState = LASER;
      r.mouseEnabled = false;
    }
    else if (g == DISABLE_MOUSE_GESTURE)
    {
      r.action = ACTION_TOGGLE_MOUSE;
      r.toggleMouse = !toggleMouse;
      r.nextState = DISABLE_MOUSE;
      r.mouseEnabled = false;
    }
    else if (g == SNIP_GESTURE)
    {
      r.action = ACTION_SNIP;
      r.nextState = SNIP;
    }
    else if (g == ALT_TAB_GESTURE)
    {
      r.action = ACTION_ALT_TAB;
      r.nextState = ALT_TAB;
    }
    else if (g == ALT_SHIFT_TAB_GESTURE)
    {
      r.action = ACTION_ALT_SHIFT_TAB;
      r.nextState = ALT_SHIFT_TAB;
    }
    else if (g == SCROLL_GESTURE)
    {
      if (ax >= 3.0f)
        r.action = ACTION_SCROLL_UP, r.nextState = SCROLL, r.mouseEnabled = false;
      else if (ax <= -2.0f)
        r.action = ACTION_SCROLL_DOWN, r.nextState = SCROLL, r.mouseEnabled = false;
    }
    else if (g == ZOOM_GESTURE)
    {
      if (ax > 3.0f)
        r.action = ACTION_ZOOM_IN;
      else if (ax <= -2.0f)
        r.action = ACTION_ZOOM_OUT;
      if (r.action != ACTION_NONE)
      {
        r.nextState = ZOOM;
        r.mouseEnabled = false;
      }
    }
    break;

  case LEFT_CLICK_EVENT:
    if (g == NONE)
    {
      r.action = ACTION_RELEASE_LEFT;
      r.nextState = IDLE;
      r.mouseEnabled = true;
    }
    break;

  case RIGHT_CLICK_EVENT:
    if (g == NONE)
    {
      r.action = ACTION_RELEASE_RIGHT;
      r.nextState = IDLE;
      r.mouseEnabled = true;
    }
    break;

  case DRAG_EVENT:
    if (g == NONE)
    {
      r.action = ACTION_RELEASE_LEFT;
      r.nextState = IDLE;
    }
    break;

  case LASER:
    if (g == NONE)
    {
      r.action = ACTION_DISABLE_LASER;
      r.nextState = IDLE;
      r.mouseEnabled = true;
    }
    break;

  case DISABLE_MOUSE:
    // this one didn’t compare g at all
    if (!r.toggleMouse)
    {
      r.mouseEnabled = true;
    }
    r.nextState = IDLE;
    break;

  case SNIP:
    if (g == NONE)
    {
      r.nextState = IDLE;
    }
    break;

  case ALT_TAB:
    if (g == NONE)
    {
      r.nextState = IDLE;
    }
    break;

  case ALT_SHIFT_TAB:
    if (g == NONE)
    {
      r.nextState = IDLE;
    }
    break;

  case SCROLL:
    if (g == NONE)
    {
      r.action = ACTION_NONE;
      r.nextState = IDLE;
      r.mouseEnabled = true;
    }
    break;

  case ZOOM:
    if (g == NONE)
    {
      r.action = ACTION_NONE;
      r.nextState = IDLE;
      r.mouseEnabled = true;
    }
    break;
  }

  return r;
}
