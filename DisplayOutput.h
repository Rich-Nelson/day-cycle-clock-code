#ifndef do
#define do

#if (ARDUINO >=100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Servo.h>
#include "FastLED.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//#define DEBUG



#define SERVO_PIN    3

#define NUM_LEDS     45
#define NUM_ROWS     3
#define LED_PIN      6
#define LED_TYPE     WS2812B
#define COLOR_ORDER  GRB

#define __CS 8
#define __DC 9
#define __A0 10

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
    CRGB leds[NUM_LEDS];
    TFT_ILI9163C tft = TFT_ILI9163C(__CS, __A0 , __DC);
    LiquidCrystal_I2C lcd;

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


    //variables


  private:
    String daysOfTheMonth[13] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    uint8_t moon_phase;
    uint8_t phase_buffer = 2;
    const bool MOON_SHADOW = 1;
    uint8_t tft_width = 128;
    uint8_t tft_height = 128;
    uint8_t uparrow[3] = {0x4, 0xe, 0x1f};


};

#endif
