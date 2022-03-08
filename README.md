# Lyra

**A SamD21 Based Rocket Deployment Altimeter**

This altimeter will be able to be used for single deployment events in mid to high power rocketry. A compact design was emphasized in order to allow for use in 24mm and larger body tubes, or attached to the parachute for a cable cutter or similar deployment method.

## Current State

As of right now, I am working on flight software with basic functionality. The original PCB design is filled with many flaws, however, I still included them for use as a reference. Plans have been made for a new version of the PCB to fix many, if not all, of the shortcomings my original design had. I believe I have found most of them, but one includes the difficulty of updating firmware and testing, which will inevitably lead to something being overlooked. I will likely update this with more information about that part of the project in the future. Also to come are some pictures of the one prototype board that I have assembled. The changelog below is more to keep track of what I have or haven't tackled yet, so feel free to ignore it.

## Changelog

Latest Changes:

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
