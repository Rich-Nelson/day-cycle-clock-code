#include "DisplayOutput.h"

DisplayOutput::DisplayOutput(){
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
}

void DisplayOutput::servoMoveTo(uint8_t degree, uint16_t s_delay){
  servo.attach(SERVO_PIN);
//  servo.writeMicroseconds(degree * 9.5 + 600);
  servo.write(degree);
  delay(s_delay);
  servo.detach();
}

void DisplayOutput::servoAttach(){
  servo.attach(SERVO_PIN);
}

void DisplayOutput::servoDetach(){
  servo.attach(SERVO_PIN);
}


void DisplayOutput::stripRGBRow(int r, int g, int b, int row) {
  int first_led_in_row = NUM_LEDS / NUM_ROWS * row;
  int last_led_in_row  = NUM_LEDS / NUM_ROWS * (row + 1);
  for (int x = first_led_in_row; x <  last_led_in_row; x++) {
    leds[x] = CRGB(r, g, b);
  }
  FastLED.show();
}

void DisplayOutput::stripRGB(int colors[NUM_ROWS][3]){
    #ifdef DEBUG
      Serial.print("{");
    #endif
  for (int row; row < NUM_ROWS; row++){
    #ifdef DEBUG
      Serial.print("{");
      Serial.print(colors[row][0]);
      Serial.print(", ");
      Serial.print(colors[row][1]);
      Serial.print(", ");
      Serial.print(colors[row][2]);
      Serial.print("}, ");
    #endif
    stripRGBRow(colors[row][0], colors[row][1], colors[row][2], row);
  }

      #ifdef DEBUG
        Serial.println("}");
      #endif
}
