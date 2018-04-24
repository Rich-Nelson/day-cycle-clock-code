#ifndef do
#define do

#if (ARDUINO >=100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Servo.h>
#include "FastLED.h"

#define SERVO_PIN    3

#define NUM_LEDS     45
#define NUM_ROWS     3
#define LED_PIN      6
#define LED_TYPE     WS2812B
#define COLOR_ORDER  GRB



class DisplayOutput {
  public:
    // Constructor
    DisplayOutput();
    Servo servo;
    CRGB leds[NUM_LEDS];

    // Configuration


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
    //variables


  private:


};

#endif
