#include "DisplayOutput.h"



DisplayOutput::DisplayOutput() : matrix(A, B, C, D, CLK, LAT, OE, false, 64),
                                 lcd(LCD_I2C_ADDR, LCD_ROW , LCD_COL),
                                 lcd2(LCD2_I2C_ADDR, LCD_ROW , LCD_COL),
                                 stepper(2,STEPPER_STEP,STEPPER_DIR){


}

void DisplayOutput::begin(){
  lcd.begin();
	lcd.backlight();
  lcd.createChar(0, uparrow);
  lcd.setCursor(selector_location[0][COL],selector_location[0][ROW]);
  lcd.write(ARROW);
  lcd2.begin();
	lcd2.backlight();
  lcd2.setCursor(3, 0);
  lcd2.print("Day Cycle Clock");
  lcd2.setCursor(0, 2);
  lcd2.print("Push & Turn The Knob");
  lcd2.setCursor(0, 3);
  lcd2.print("Watch Display Update");
  lcd2.setCursor(0, 3);
  lcd2.print("    In Real Time    ");
  // lcd2.createChar(0, uparrow);
  // lcd2.setCursor(selector_location[0][COL],selector_location[0][ROW]);
  // lcd2.write(ARROW);


  matrix.begin();
  matrix.setRotation(3);

  // matrix.drawLine(0, 0, matrix.width()-1, matrix.height()-1, matrix.Color333(7, 0, 0));
  // matrix.drawLine(matrix.width()-1, 0, 0, matrix.height()-1, matrix.Color333(7, 0, 0));

  pinMode(STEPPER_STEP, OUTPUT);
  pinMode(STEPPER_DIR, OUTPUT);
  pinMode(STEPPER_SLEEP, OUTPUT);
  digitalWrite(STEPPER_SLEEP, HIGH);
  stepper.setMaxSpeed(STEPPER_SPEED*MICRO_STEPS);
  stepper.setAcceleration(STEPPER_ACCEL*MICRO_STEPS);



// fix the screen with green
// matrix.fillRect(0, 0, 32, 32, matrix.Color333(7, 7, 7));

// stripRGBRow(255,255,255,0);
//  while (true){}

  pinMode(LOWER_LIMIT, INPUT);
  pinMode(UPPER_LIMIT, INPUT);
  homeStepper();




// while(true){
  // Serial.println("Upper Limit: "); Serial.println(digitalRead(UPPER_LIMIT));
  // Serial.println("Lower Limit: "); Serial.println(digitalRead(LOWER_LIMIT));
// }

}

void DisplayOutput::homeStepper(){

  while(digitalRead(LOWER_LIMIT) == 0){
    // Serial.println(stepper.currentPosition());
    stepper.moveTo(stepper.currentPosition() - 20*MICRO_STEPS);
    stepperRun();
  }
  stepper.setCurrentPosition(0);
}

void DisplayOutput::stepperRun(){

    stepper.run();


}

void DisplayOutput::servoMoveTo(uint8_t degree, uint16_t s_delay){
  #ifdef DEBUG
  Serial.print("Degree: ");
  Serial.println(degree);
    Serial.print("Servo Move To: ");
    Serial.println(((float)degree/(float)180)*TOTAL_STEPS*MICRO_STEPS);
  #endif
  stepper.stop();
  stepper.moveTo(((float)degree/(float)180)*TOTAL_STEPS*MICRO_STEPS);
  stepperRun();
  // if ( stepper.distanceToGo()){
  //   Serial.println(stepper.distanceToGo());
  //   stepper.run();
  // }
//   servo.attach(SERVO_PIN);
// //  servo.writeMicroseconds(degree * 9.5 + 600);
//   servo.write(degree);
//   delay(s_delay);
//   servo.detach();
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
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.fillCircle(15, 15, 15, matrix.Color333(7, 7, 0));
}


void DisplayOutput::fillArc(int32_t x0, int32_t y0, int32_t r1, bool side, int8_t percent_transition, uint8_t phase, int16_t color){
  int32_t y1;
  int32_t y2;
  int32_t width = r1*percent_transition/100;
  int32_t r2 = width/2+sq(r1)/(2*width);
  int32_t x02 = x0 - (r2 - width);
  #ifdef DEBUG
     Serial.print(" x0: ");
     Serial.print(x0);
     Serial.print(" y0: ");
     Serial.print(y0);
     Serial.print(" r1: ");
     Serial.println(r1);
     Serial.print("percent_transition: ");
     Serial.print(percent_transition);
     Serial.print(" width: ");
     Serial.print(width);
     Serial.print(" r2: ");
     Serial.print(r2);
     Serial.print(" x02: ");
     Serial.println(x02);

  #endif
  // if(x0+width < tft_width && x0+width > 0){

    for(int16_t x = x0; x < x0+r1; x++ ){
      y1 = round(sqrt(sq(r1)-sq(x-x0)));
      y2 = round(sqrt(sq(r2)-sq(x-x02)));
      if(y2<0){y2=0;}
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
           matrix.drawFastVLine(2*x0-x, y0-y1, y1-y2, color);
           matrix.drawFastVLine(2*x0-x, y0+y2, y1-y2, color);
        }
        if(side == 1){
           matrix.drawFastVLine(x, y0-y1, y1-y2, color);
           matrix.drawFastVLine(x, y0+y2, y1-y2, color);
        }
      }else{
        if (y2){
          if(side == 0){
             matrix.drawFastVLine(2*x0-x, y0-y2, y2*2, color);
          }
          if(side == 1){
             matrix.drawFastVLine(x, y0-y2, y2*2, color);
          }
        }
      }
    }

  // }
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

  matrix.fillScreen(matrix.Color333(0, 0, 0));
  //Draw Moon shadow gradient
  // if (MOON_SHADOW && moon_phase_precentage % 50 > phase_buffer*2 && moon_phase_precentage % 50 < 50 - phase_buffer*2){
  //   if (moon_phase <= FULL_MOON){
  //     moon_progress = -1;
  //   }else{
  //     moon_progress = 1;
  //   }
  //     drawMoon(moon_phase, moon_phase_precentage, tft_width/2+(9*moon_progress), 0x4208);
  //     drawMoon(moon_phase, moon_phase_precentage, tft_width/2+(6*moon_progress), 0x8410);
  //     drawMoon(moon_phase, moon_phase_precentage, tft_width/2+(3*moon_progress), 0xDEDB);
  // }
  drawMoon(moon_phase, moon_phase_precentage, tft_width/2, matrix.Color333(7, 7, 7));
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
      fillArc(x0, tft_height/2, tft_height/2, 1, percent_transition, moon_phase, color);
      break;
    case FIRST_QUARTER:
      fillArc(x0, tft_height/2, tft_height/2, 1, 100, moon_phase, color);
      break;
    case WAXING_GIBBOUS:
      percent_transition = (moon_phase_precentage-25)*100/25;
      fillArc(x0, tft_height/2, tft_height/2, 1, 100, moon_phase, color);
      fillArc(x0, tft_height/2, tft_height/2, 0, percent_transition, moon_phase, color);
      break;
    case FULL_MOON:
      fillArc(x0, tft_height/2, tft_height/2, 0, 100, moon_phase, color);
      fillArc(x0, tft_height/2, tft_height/2, 1, 100, moon_phase, color);
      break;
    case WANING_GIBBOUS:
      percent_transition = 100-(moon_phase_precentage-50)*100/25;
      fillArc(x0, tft_height/2, tft_height/2, 0, 100, moon_phase, color);
      fillArc(x0, tft_height/2, tft_height/2, 1, percent_transition, moon_phase, color);
      break;
    case THIRD_QUARTER:
       fillArc(x0, tft_height/2, tft_height/2, 0, 100, moon_phase, color);
      break;
    case WANING_CRESCENT:
      percent_transition = (moon_phase_precentage-75)*100/25;
      fillArc(x0, tft_height/2, tft_height/2, 0, percent_transition, moon_phase, color);
      break;
  }

  #ifdef DEBUG
    Serial.print("Moon Phase: ");
    Serial.println(moon_phase_precentage);
  #endif
}

void DisplayOutput::printTime(int16_t *currentTime) {

 for (uint8_t i = 0; i < NUM_SETTINGS; i++){
   printValue(i, currentTime[i]);
 }

  lcd.setCursor(8, 0);
  lcd.print(":");

  lcd.setCursor(10, 2);
  lcd.print(",");

//  lcd.setCursor(selector_location[0][COL],selector_location[0][ROW]);
//  lcd.write(ARROW);
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
  lcd.setCursor(11,0);
  if(AM){
    lcd.print("am");
  }else{
    lcd.print("pm");
  }
}

void DisplayOutput::printValue(int unit, int value) {
  if(unit == HOUR){
    bool AM = amCheck(value);
    printAMPM(AM);
    value = convert12Hr(value);
  }

  lcd.setCursor(time_setting_location[unit][COL],
                time_setting_location[unit][ROW]);

  if(unit == MONTH){
    lcd.print(daysOfTheMonth[value]);
  }else if( value < 10){
    lcd.print("0");
    lcd.print(value);
  }else{
    lcd.print(value);
  }
}

void DisplayOutput::updateSelector(int8_t selection) {

  for (uint8_t i; i<2 ; i++ ){
    lcd.setCursor(selector_location[selection][COL],selector_location[selection][ROW]);
    if(i){
      lcd.print(" "); //Erase selector arrow from previous position
    }else{
      lcd.write(ARROW);
    }

    selection = (((selection - 1) % 5) + 5) % 5;
  }

}

void DisplayOutput::fillMenuTitle() {
  // tft.fillRect(32, 16, 70, 24, YELLOW);
}

void DisplayOutput::fillValue() {
  // tft.fillRect(7, 48, 118, 30, YELLOW);
}

void DisplayOutput::fillAMPM() {
  // tft.fillRect(48, 80, 28, 24, YELLOW);
}

void DisplayOutput::fillCircle(uint16_t color) {
  // tft.fillCircle(tft_width/2, tft_height/2, tft_width/2-2, color);
}
