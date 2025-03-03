# Individual Weekly Report

**Name**: Will Tatum

**Team**: The Gauntlet

**Date**: 03/03/2025

## Current Status
The current status of the project is that we are making good progress on both the hardware and software and are over halfway done.

### What did _you_ work on this past week?

| Task | Status | Time Spent | 
| ---- | ------ | ---------- |
|   Translation Movement   |    In Progress    |      4 hrs      |
|   Keyboard Controls Research   |    Complete    |      3 hrs      |
|   Fixing Gesture Sensor Code   |    Complete    |      1 hr      |

*Include screenshots/diagrams/figures/etc. to illustrate what you did this past week.*
Small code snippet from the translation movement code which is basically what we already have with some additions.

unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  // Update velocity by integrating acceleration.
  // Mapping: horizontal movement from accel_y and vertical movement from -accel_x
  vel_x += accel_y * dt;
  vel_y += -accel_x * dt;

  // Apply damping to reduce drift
  vel_x *= 0.9;
  vel_y *= 0.9;

  // Scale the integrated velocity to determine mouse movement.
  // (You can tune the scale factor below as needed.)
  int mouse_dx = (int)(vel_x * 10);
  int mouse_dy = (int)(vel_y * 10);

  // Move the mouse accordingly
  blehid.mouseMove(mouse_dx, mouse_dy);
### What problems did you run into? What is your plan for them?

We ran into issues with finding the left control key. The plan is to try detecting it as a modifier rather than a key.

### What is the current overall project status from your perspective? 

I would say it is about halfway to a bit over halfway done.

### How is your team functioning from your perspective?

The team is functioning well.

### What new ideas did you have or skills did you develop this week?

I developed skills on trying to get the board to detect translational movement for moving the mouse.

### Who was your most awesome team member this week and why?

Grant was my most awesome team member. He got the buttons to function with a test circuit.

## Plans for Next Week

The plan for next week is to refine our mouse movement and get gestures working.
