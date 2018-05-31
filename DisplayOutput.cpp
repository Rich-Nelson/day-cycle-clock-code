#include "DisplayOutput.h"



DisplayOutput::DisplayOutput() : matrix(A, B, C, D, CLK, LAT, OE, false, 64),
                                 lcd(LCD_I2C_ADDR, LCD_ROW , LCD_COL){


}

void DisplayOutput::begin(){
  lcd.begin();
	lcd.backlight();
  lcd.createChar(0, uparrow);

  lcd.setCursor(6, 0);
  lcd.print("12");
  lcd.setCursor(8, 0);
  lcd.print(":");
  lcd.setCursor(9, 0);
  lcd.print("34");
  lcd.setCursor(11, 0);
  lcd.print("pm");

  lcd.setCursor(7,1);
  lcd.write(ARROW);

  lcd.setCursor(4, 2);
  lcd.print("Mar");
  lcd.setCursor(8, 2);
  lcd.print("30");
  lcd.setCursor(10, 2);
  lcd.print(",");
  lcd.setCursor(12, 2);
  lcd.print("2018");

  matrix.begin();
  matrix.setRotation(3);
  matrix.fillCircle(15, 15, 15, matrix.Color333(7, 7, 0));
  // matrix.drawLine(0, 0, matrix.width()-1, matrix.height()-1, matrix.Color333(7, 0, 0));
  // matrix.drawLine(matrix.width()-1, 0, 0, matrix.height()-1, matrix.Color333(7, 0, 0));
  //matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));


}

void DisplayOutput::servoMoveTo(uint8_t degree, uint16_t s_delay){
  #ifdef DEBUG
    Serial.print("Servo Move To: ");
    Serial.println(degree);
  #endif
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
  #ifdef DEBUG
    Serial.print(ledStripColor[row][0]);
    Serial.print(" , ");
    Serial.print(ledStripColor[row][1]);
    Serial.print(" , ");
    Serial.print(ledStripColor[row][2]);
    Serial.print(" , row:");
    Serial.println(row);
  #endif
  analogWrite(ledStripColor[row][0],r);
  analogWrite(ledStripColor[row][1],g);
  analogWrite(ledStripColor[row][2],b);
}

void DisplayOutput::stripRGB(int colors[NUM_ROWS][3]){

    #ifdef DEBUG
      Serial.println("LED Strip Colors");
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
  matrix.fillCircle(15, 15, 15, matrix.Color333(7, 7, 0));
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

bool DisplayOutput::amCheck(int time_hour){
  if( time_hour < 12){
    return true;
  }else{
    return false;
  }
}

int DisplayOutput::convert12Hr(int time_hour){
  if (time_hour == 0){
    return 12;
  }else if( time_hour > 12){
    return time_hour - 12;
  }else{
    return time_hour;
  }

}

void DisplayOutput::printAMPM(bool AM){
  fillAMPM();

  tft.setCursor(48, 84);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  if(AM){
    tft.print("AM");
  }else{
    tft.print("PM");
  }
}

void DisplayOutput::printTime(int time_hour, int time_min) {
  bool AM = amCheck(time_hour);
  time_hour = convert12Hr(time_hour);
  printAMPM(AM);

  fillValue();
  tft.setCursor(7, 48);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  if(time_hour < 10){
    tft.print(0, DEC);
  }
  tft.print(time_hour, DEC);
  tft.print(':');
  if(time_min < 10){
    tft.print(0, DEC);
  }
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

void DisplayOutput::printValue(int value, bool hr = 0) {

  if(hr){
    bool AM = amCheck(value);
    value = convert12Hr(value);
    printAMPM(AM);
  }

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

void DisplayOutput::fillAMPM() {
  tft.fillRect(48, 80, 28, 24, YELLOW);
}

void DisplayOutput::fillCircle(uint16_t color) {
  tft.fillCircle(tft_width/2, tft_height/2, tft_width/2-2, color);
}

void DisplayOutput::updateSelector(int8_t selection) {

  for (uint8_t i; i<2 ; i++ ){
    uint8_t row = selector_location[selection] % 20;
    uint8_t col = 1 + 2 * (selector_location[selection] / 20);

    lcd.setCursor(row,col);
    if(i){
      lcd.print(" ");
    }else{
      lcd.write(ARROW);
    }

    selection = (((selection - 1) % 5) + 5) % 5;
  }

}
