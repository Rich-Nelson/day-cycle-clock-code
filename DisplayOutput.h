#ifndef do
#define do

#if (ARDUINO >=100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Servo.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <RGBmatrixPanel.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>

#define DEBUG

#define STEPPER_STEP 34
#define STEPPER_DIR 35
#define STEPPER_SPEED 10000
#define STEPPER_ACCEL 20000
#define MICRO_STEPS 1

#define SERVO_PIN    300  //3

#define LOWER_LIMIT 2
#define UPPER_LIMIT 3

#define NUM_ROWS     3

//32x64 Matrix pins
#define OE  31
#define LAT 30
#define CLK 13
#define A   A0
#define B   A1
#define C   A2
#define D   A3

#define __CS 800 //8
#define __DC 900 //9
#define __A0 1000 //10

#define LCD_I2C_ADDR 0x3F
#define LCD_ROW 4
#define LCD_COL 20
#define ARROW 0

#define BLACK   0x0000
#define YELLOW  0xFFE0
#define WHITE   0xFFFF



enum MoonPhases {
            NEW_MOON,
            WAXING_CRESCENT,
            FIRST_QUARTER,
            WAXING_GIBBOUS,
            FULL_MOON,
            WANING_GIBBOUS,
            THIRD_QUARTER,
            WANING_CRESCENT
            };



class DisplayOutput {
  public:
    // Constructor
    DisplayOutput();


    // Configuration
    Servo servo;
    TFT_ILI9163C tft = TFT_ILI9163C(__CS, __A0 , __DC);
    LiquidCrystal_I2C lcd;
    RGBmatrixPanel matrix;
    AccelStepper stepper;

    void begin();

    // Mehods
    void servoMoveTo(uint8_t degree, uint16_t s_delay);
    void servoAttach();
    void servoDetach();
    void stripRGBRow(int r, int g, int b, int row);
    void stripRGB(int colors[NUM_ROWS][3]);
    void updateScreen(bool daytime, uint8_t moon_phase_precentage);
    void drawSun();
    void fillArc(int32_t x0, int32_t y0, int32_t r1, bool side, int8_t percent_transition, uint8_t phase, int16_t color);
    void updateMoon( uint8_t moon_phase_precentage);
    void drawMoon( uint8_t moon_phase, uint8_t moon_phase_precentage, int16_t x0, int16_t color);
    void printMenuTitle(String titleString);
    bool amCheck(int time_hour);
    int convert12Hr(int time_hour);
    void printAMPM(bool AM);
    void printTime(int time_hour, int time_min);
    void printDate(uint8_t month, uint8_t day, uint16_t year);
    void printValue(int value, bool hr = false);
    void fillMenuTitle();
    void fillValue();
    void fillAMPM();
    void fillCircle(uint16_t color);
    void updateSelector(int8_t selection);


    //variables


  private:
    String daysOfTheMonth[13] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    uint8_t moon_phase;
    uint8_t phase_buffer = 2;
    const bool MOON_SHADOW = 1;
    uint8_t tft_width = 128;
    uint8_t tft_height = 128;
    uint8_t uparrow[8] = {0x4, 0xe, 0x1f};

    uint8_t selector_location[5] = {7, 10, 26, 29, 35};
    uint8_t ledStripColor[NUM_ROWS][3] = {{4,5,6},{7,8,9},{10,11,12}};

};

#endif
