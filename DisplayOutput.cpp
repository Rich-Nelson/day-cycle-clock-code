#include "DisplayOutput.h"

DisplayOutput::DisplayOutput(){
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
}

void DisplayOutput::begin(){
  tft.begin();
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

void DisplayOutput::drawSun() {
  tft.fillScreen();
  tft.fillCircle(64, 64, 62, YELLOW);
}


void DisplayOutput::fillArc(int32_t x0, int32_t y0, int32_t r1, bool side, int8_t percent_transition, uint8_t phase, int16_t color){
  int32_t y1;
  int32_t y2;
  int32_t width = r1*percent_transition/100;
  int32_t r2 = width/2+sq(r1)/(2*width);
  int32_t x02 = x0 - (r2 - width);
  #ifdef DEBUG
     Serial.print("percent_transition: ");
     Serial.print(percent_transition);
     Serial.print("width: ");
     Serial.print(width);
     Serial.print(" r2: ");
     Serial.print(r2);
     Serial.print(" x02: ");
     Serial.println(x02);

  #endif
  if(x0+width < tft_width && x0+width > 0){

    for(int16_t x = x0; x < x0+r1; x++ ){
      y1 = sqrt(sq(r1)-sq(x-x0));
      y2 = sqrt(sq(r2)-sq(x-x02));
      #ifdef DEBUG
       Serial.print("x: ");
       Serial.print(x);
       Serial.print(" y1: ");
       Serial.print(y1);
       Serial.print(" y2: ");
       Serial.print(y2);
       Serial.print(" h: ");
       Serial.println(y1*2);
       delay(500);
      #endif

      if (phase == WAXING_CRESCENT || phase == WANING_CRESCENT){
        if(side == 0){
           tft.drawFastVLine(2*x0-x, y0-y1, y1-y2, color);
           tft.drawFastVLine(2*x0-x, y0+y2, y1-y2, color);
        }
        if(side == 1){
           tft.drawFastVLine(x, y0-y1, y1-y2, color);
           tft.drawFastVLine(x, y0+y2, y1-y2, color);
        }
      }else{
        if(side == 0){
           tft.drawFastVLine(2*x0-x, y0-y2, y2*2, color);
        }
        if(side == 1){
           tft.drawFastVLine(x, y0-y2, y2*2, color);
        }
      }
    }

  }
}


void DisplayOutput::updateMoon( uint8_t moon_phase_precentage){
  uint8_t quot = moon_phase_precentage / 25;
  uint8_t rem = moon_phase_precentage % 25;
  int8_t moon_progress;

  if (rem <= phase_buffer){
    moon_phase = quot * 2;
  }else if( rem < 25 - phase_buffer){
    moon_phase = quot * 2 + 1;
  }else{
    moon_phase = quot * 2 + 2;
  }

  tft.fillScreen();
  //Draw Moon shadow gradient
  if (MOON_SHADOW && moon_phase_precentage % 50 > phase_buffer*2 && moon_phase_precentage % 50 < 50 - phase_buffer*2){
    if (moon_phase <= FULL_MOON){
      moon_progress = -1;
    }else{
      moon_progress = 1;
    }
      drawMoon(moon_phase, moon_phase_precentage, tft_width/2+(9*moon_progress), 0x4208);
      drawMoon(moon_phase, moon_phase_precentage, tft_width/2+(6*moon_progress), 0x8410);
      drawMoon(moon_phase, moon_phase_precentage, tft_width/2+(3*moon_progress), 0xDEDB);
  }
  drawMoon(moon_phase, moon_phase_precentage, tft_width/2, WHITE);
}

void DisplayOutput::drawMoon( uint8_t moon_phase, uint8_t moon_phase_precentage, int16_t x0, int16_t color){


  uint8_t percent_transition;


  #ifdef DEBUG
    Serial.print("Moon Phase Test: ");
    Serial.println(moon_phase);
  #endif

  switch (moon_phase) {
    case NEW_MOON:

      break;
    case WAXING_CRESCENT:
      percent_transition = 100-moon_phase_precentage*100/25;
      fillArc(x0, tft_height/2, 62, 1, percent_transition, moon_phase, color);
      break;
    case FIRST_QUARTER:
      fillArc(x0, tft_height/2, 62, 1, 100, moon_phase, color);
      break;
    case WAXING_GIBBOUS:
      percent_transition = (moon_phase_precentage-25)*100/25;
      fillArc(x0, tft_height/2, 62, 1, 100, moon_phase, color);
      fillArc(x0, tft_height/2, 62, 0, percent_transition, moon_phase, color);
      break;
    case FULL_MOON:
      fillArc(x0, tft_height/2, 62, 0, 100, moon_phase, color);
      fillArc(x0, tft_height/2, 62, 1, 100, moon_phase, color);
      break;
    case WANING_GIBBOUS:
      percent_transition = 100-(moon_phase_precentage-50)*100/25;
      fillArc(x0, tft_height/2, 62, 0, 100, moon_phase, color);
      fillArc(x0, tft_height/2, 62, 1, percent_transition, moon_phase, color);
      break;
    case THIRD_QUARTER:
       fillArc(x0, tft_height/2, 62, 0, 100, moon_phase, color);
      break;
    case WANING_CRESCENT:
      percent_transition = (moon_phase_precentage-75)*100/25;
      fillArc(x0, tft_height/2, 62, 0, percent_transition, moon_phase, color);
      break;
  }

  #ifdef DEBUG
    Serial.print("Moon Phase: ");
    Serial.println(moon_phase_precentage);
  #endif
}

void DisplayOutput::printMenuTitle(String titleString) {
  fillMenuTitle();
  int x_offset = (4 - titleString.length()) * 8;
  tft.setCursor(32 + x_offset, 16);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print(titleString);
}

void DisplayOutput::printTime(int time_hour, int time_min) {
  fillValue();
  tft.setCursor(7, 48);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.print(time_hour, DEC);
  tft.print(':');
  tft.println(time_min, DEC);
}

void DisplayOutput::printDate(uint8_t month, uint8_t day, uint16_t year) {
  fillValue();
  tft.setCursor(16, 42);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print(daysOfTheMonth[month]);
  tft.print(' ');
  tft.println(day, DEC);
  tft.setCursor(28, 72);
  tft.print(year, DEC);
}

void DisplayOutput::printValue(int value) {
  int val_length = String(value).length();
  int cursor_x;
  tft.setTextColor(BLACK);
  if (val_length <= 5) {
    tft.setTextSize(4);
    cursor_x = 6 + (5 - val_length) * 12;
  } else {
    tft.setTextSize(3);
    cursor_x = 6 + (5 - val_length) * 10;
  }
  fillValue();
  tft.setCursor(cursor_x, 48);
  tft.print(value);
}

void DisplayOutput::fillMenuTitle() {
  tft.fillRect(32, 16, 70, 24, YELLOW);
}

void DisplayOutput::fillValue() {
  tft.fillRect(7, 48, 118, 30, YELLOW);
}
