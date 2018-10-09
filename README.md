# day-cycle-clock-code

This repository contains the code for the Day Cycle Clock project.  It was written to run on an Arduino Nano,  it should work on other Arduino compatible boards, but likely will need to change some pin definitions.

The CAD models for this project are in this repo: https://github.com/Rich-Nelson/day-cycle-clock

Links to the build video and blog post coming soon.

## Structure

The project could use some refactoring, but here is the high level project organization
- **day-cycle-clock-code.ino** Contains the main loop and it primarility handles the input buttons, clock settings, RTC, and menu
- **ColorState** Contains the functions to determine what the state of the clock should be based on the current date, time, and location.
- **DisplayOutput** Controls the output updating the LED strips, servo angle, and screen.

## Dependencies

You will need some 3rd party libraries as well see below includes for reference:
```
#include <Pushbutton.h>
#include "RTClib.h"
#include <TimeLord.h>
#include <Servo.h>
#include "FastLED.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
```

Come of these may already be included in the arduino environment, but I included them all jsut to be safe.