# Individual Weekly Report

**Name**: Will Tatum

**Team**: The Gauntlet

**Date**: 03/17/2025

## Current Status
The glove is nearing a functional prototype and is being assembled. The code is mostly done for movement but we are adding gestures and getting some driver code done along with a GUI.

### What did _you_ work on this past week?

| Task | Status | Time Spent | 
| ---- | ------ | ---------- |
|   Translation Movement Code   |    Complete    |      5hr      |
|   GUI   |    Ongoing    |      2hr      |
|      |        |            |

*Include screenshots/diagrams/figures/etc. to illustrate what you did this past week.*
const createWindow = () => {
  // Create the browser window.
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js')
    }
  })

### What problems did you run into? What is your plan for them?
The accelerometer was not good enough for the code. We have gotten a better separate one.


### What is the current overall project status from your perspective? 
70% since we need to assemble the glove and finish up some code.


### How is your team functioning from your perspective?
The team is functioning well. No issues so far.


### What new ideas did you have or skills did you develop this week?
Translation code used Kalman filters and other methods to try and reduce errors.


### Who was your most awesome team member this week and why?
Aaron because he got the flex sensors in the glove.


## Plans for Next Week
Work on the GUI.
*What are you going to work on this week?*
