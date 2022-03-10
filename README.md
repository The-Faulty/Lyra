# Lyra

**A SamD21 Based Rocket Deployment Altimeter**

This altimeter will be able to be used for single deployment events in mid to high power rocketry. A compact design was emphasized in order to allow for use in 24mm and larger body tubes, or attached to the parachute for a cable cutter or similar deployment method.

## Current State

As of right now, I am working on flight software with basic functionality. The original PCB design is filled with many flaws, however, I still included them for use as a reference. Plans have been made for a new version of the PCB to fix many, if not all, of the shortcomings my original design had. I believe I have found most of them, but one includes the difficulty of updating firmware and testing, which will inevitably lead to something being overlooked. I will likely update this with more information about that part of the project in the future. Also to come are some pictures of the one prototype board that I have assembled. The changelog below is more to keep track of what I have or haven't tackled yet, so feel free to ignore it.

## The Bad, the Worse, and the Annoying

During my initial development of the Lyra pcb, I made some naive choices in terms of design, and overlooked other quality of life aspects which have led to many headaches during development.

## Changelog

Latest Changes:

**V0.2 - "Hands up, Don't Chute!"**

- Fixed typo of `stateMacine` to `stateMachine`
- Fixed `state` being initialized as an integer instead of an instance of the `stateMachine` enum

- Changed `loop()` to use switch/case instead of `if else`
- Moved `readAlt()` to the start of if statement instead of in each case

- Added `ISDROGUE`, `ISAPOGEEDEPLOY` to user variables (I missed them in the last update)
- Added `DEPLOYALT` variable for setting the deployment altitude
- Added `SAVEDATA` boolean to toggle saving data to flash
- Added use of buzzer in `setup()` for indicating barometer found and setup complete
- Added `landed` case to `loop()`

**V0.1 - "No Function Junction"**

- Fixed variables being logged being out of order
- Fixed barometer code to use correct barometer model
- Fixed `readAlt()` function's for loop running until greater than the array's index
- Fixed not changing state from `startUp` to `idle` in setup

- Removed variable `rateLeng` and instead directly used the `rollAvg` array for vertical speed calculation

- Changed user-defined variables to be under define tags to allow usage in declaring array lengths

- Added vertical speed function (`getRate()`) and `avgRate` variable
- Added battery voltage function (`getBattVoltage()`)
- Added vertical speed to logging data
- Added `period` and `lastMillis` variables for tracking loop cycles
- Added majority of this ReadMe
