# Individual Weekly Report

**Name**: Will Tatum

**Team**: The Gauntlet

**Date**: 03/31/2025

## Current Status
The glove is coming together very quickly and we now have a working prototype. I would say that the glove is 80% done. We do need to pick it up a bit though and will be on crunch time this week.

### What did _you_ work on this past week?

| Task | Status | Time Spent | 
| ---- | ------ | ---------- |
|   Report   |    In Progress    |      30min      |
|   GUI   |    In Progress    |     2hr       |
|      |        |            |

*Include screenshots/diagrams/figures/etc. to illustrate what you did this past week.*
Snippet of code to get the Bluetooth issues fixed.

void configBluetooth()
{
  // add any bluetooth config stuff here
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.configPrphConn(50, 8, 4, 4);
}

### What problems did you run into? What is your plan for them?

We ran into problems assembling the glove and solved them using 3D printing. The Bluetooth changes actually worked first try which was shocking so no problems there.

### What is the current overall project status from your perspective? 

I would say that the project is about 80% done. This is actually now true. Turns out I overestimated our completion.

### How is your team functioning from your perspective?

The team is functioning well and we have minimal issues. We got a lot done this last week and are on track.
### What new ideas did you have or skills did you develop this week?

No new ideas so far. A lot of catching up from before for us.

### Who was your most awesome team member this week and why?

Kris is my most awesome teammate this week because he spent a lot of time getting us good graphs of all the sensor data and filtering it.

## Plans for Next Week

*What are you going to work on this week?*

I am going to actually work on drivers this week since I ended up working on Bluetooth last week.
