# Individual Weekly Report

**Name**: Will Tatum

**Team**: The Gauntlet

**Date**: 04/14/2025

## Current Status
The glove is nearly done now and just needs a little more refinement on the gesture detection and the casing.

### What did _you_ work on this past week?

| Task | Status | Time Spent | 
| ---- | ------ | ---------- |
|   Gesture Detection   |    In Progress    |      8hr      |
|      |        |            |
|      |        |            |

*Include screenshots/diagrams/figures/etc. to illustrate what you did this past week.*
  // left mouse button
  switch (leftState)
  {
  case MOUSE_IDLE:
    // At idle, check if we have a new left-click or drag gesture.
    if (gesture_ == R) // should be I
    {
      // Initiate a discrete left-click event.
      // (According to our grammar, a click is I followed by NONE.)
      blehid.mouseButtonPress(MOUSE_BUTTON_LEFT);
      leftState = CLICK_EVENT;
      mouseEnabled = false;
    }

### What problems did you run into? What is your plan for them?

We ran into problems with the gesture detection having conflicts with itself. The plan is to basically wait until the new gesture has stabilized before switching to it.

### What is the current overall project status from your perspective? 

I would say that the project is about 90% done.

### How is your team functioning from your perspective?

The team is functioning well and we have minimal issues. We got a lot done this last week and are on track.

### What new ideas did you have or skills did you develop this week?

We had ideas on using the mean and stdev of the sensors to detect if they are stabilized or not. Another idea was a stabilization window for the gestures.

### Who was your most awesome team member this week and why?

Kris is my most awesome teammate this week because he spent a lot of time getting everything working.

## Plans for Next Week

*What are you going to work on this week?*

I am going to continue working on gesture detection.
