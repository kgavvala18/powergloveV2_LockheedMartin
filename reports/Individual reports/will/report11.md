# Individual Weekly Report

**Name**: Will Tatum

**Team**: The Gauntlet

**Date**: 04/21/2025

## Current Status
The glove is nearly done now and just needs to be touched up on the outside.

### What did _you_ work on this past week?

| Task | Status | Time Spent | 
| ---- | ------ | ---------- |
|   Gesture Detection   |    Complete    |      8 hr      |
|   Report/Poster   |    Complete    |     2 hr       |
|      |        |            |

*Include screenshots/diagrams/figures/etc. to illustrate what you did this past week.*
  previousGesture = currentGesture;
  currentGesture = gesture(thumbFiltered, indexFiltered, middleFiltered, ringFiltered, pinkyFiltered);
  Serial.println(currentGesture);

  switch (gestureState)
  {
  case IDLE:
    if (currentGesture == DRAG_GESTURE)
    {
      blehid.mouseButtonPress(MOUSE_BUTTON_LEFT);
      gestureState = DRAG_EVENT;
    }
### What problems did you run into? What is your plan for them?

We ran into problems with the glove's wires coming out. We have solved this issue.

### What is the current overall project status from your perspective? 

I would say that the project is about 99% done.

### How is your team functioning from your perspective?

The team is functioning well and we have minimal issues. We got a lot done this last week and are on track.

### What new ideas did you have or skills did you develop this week?

We figured out how to get the gesture detection working as well as we could with this hardware.

### Who was your most awesome team member this week and why?

Grant is my most awesome team member this week since he did all of the button code and got it working.

## Plans for Next Week

*What are you going to work on this week?*

I am going to work on the poster and report to help get it finalized along with some tests and user studies.
