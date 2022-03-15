# Lyra

**A SamD21 Based Rocket Deployment Altimeter**

This altimeter will be able to be used for single deployment events in mid to high power rocketry. A compact design was emphasized in order to allow for use in 24mm and larger body tubes, or attached to the parachute for a cable cutter or similar deployment method.

## Current State

As of right now, I am working on flight software with basic functionality. The original PCB design is filled with many flaws, however, I still included them for use as a reference. Plans have been made for a new version of the PCB to fix many, if not all, of the shortcomings my original design had. I believe I have found most of them, but one includes the difficulty of updating firmware and testing, which will inevitably lead to something being overlooked. Also to come are some pictures of the one prototype board that I have assembled. The changelog below is more to keep track of what I have or haven't tackled yet, so feel free to ignore it.

I have now successfully uploaded a version of the code to a Lyra board. One thing I did was replace the buzzer on the board with a bodged LED. This will allow for silent debugging and a guarantee that the lines are working. *This is the purpose of any digital writes to the buzzer pin.* The code that I uploaded ran until it attempted connection with the barometer in the setup. I will look further into the issue at a later point in time.

## The Bad, the Worse, and the Annoying

During my initial development of the Lyra pcb, I made some naive choices in terms of design, and overlooked other quality of life aspects which have led to many headaches during development. I'm going to go into some of the things that I have identified as issues in the current design or things I want to add in the next one.

The biggest issue that I made in the design of this pcb was not using a USB port. I figured that as long as I had the SWD I would be fine, but it just complicates development because I can't have the flash and SD card board attached at the same time. Another issue is lack of physical indicators. having LEDs on the power lines, pyro lines, and just attached to a GPIO pin would have helped a lot in diagnosing issues. Instead of using the BMP388, a barometer that I have used before and knew worked, I chose the MS5607 for Lyra. I don't remember why I made that choice, but the MS5607 has a few downsides compared to the BMP388. For one, there is no data ready register. This means that your code has to blindly wait for the reading to be completed, and then check back after. The libraries for using this sensor are also less tested than those for the BMP388. I also did not think very hard about the use of Lyra during an actual launch. I planned for the code to read a settings file from the SD card at launch, however, if I planned on using Lyra in multiple rockets or adjust settings on the go, I would have to bring a laptop. On the next itteration, one solution would be to use an oled shield, such as the adafruit feather oled, with a simple menu system for settings and readout. Finally, I wish I had broken out more pins to the daughter pcb. I didn't use all of the pins on the connector, and could have broken out I2C lines or similar for further prototyping.

## Changelog

Latest Changes:

**V0.2 - "Hands up, Don't Chute!"**

- Fixed typo of `stateMacine` to `stateMachine`
- Fixed `state` being initialized as an integer instead of an instance of the `stateMachine` enum
- Fixed lack of function prototypes

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
