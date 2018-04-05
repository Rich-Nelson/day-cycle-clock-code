#include <Pushbutton.h>
#include "RTClib.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <TimeLord.h>
#include <Servo.h>
#include "FastLED.h"
#include "ColorState.h"




ColorState colorstate(false);


#define SERVO_PIN 3
Servo servo;



#define BUTTON_UP   16
#define BUTTON_LEFT 15
#define BUTTON_DOWN 14

Pushbutton button_up(BUTTON_UP);
Pushbutton button_left(BUTTON_LEFT);
Pushbutton button_down(BUTTON_DOWN);


RTC_DS1307 rtc;
String daysOfTheMonth[13] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

#define __CS 8
#define __DC 9
#define __A0 10
TFT_ILI9163C tft = TFT_ILI9163C(__CS, __A0 , __DC);
uint8_t tft_width = 128;
uint8_t tft_height = 128;


#define NUM_LEDS 45
#define NUM_ROWS 3
#define LED_PIN 6
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

int color[3];
int red;
int green;
int blue;
int row;

CRGB leds[NUM_LEDS];




#define BLACK   0x0000
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
int16_t sunYellow = 0xccff00;
int16_t bgColor = 0xFFFF;


TimeLord tardis;

//Initial Settings
int8_t latitude = 40;
int16_t longitude = -75;
int8_t time_zone = -5;

//Calulated Time Settings
uint16_t sunrise_minute;
uint16_t sunset_minute;
uint8_t moon_phase;

#define UPDATE_INTERVAL 60000
long last_update = millis() - UPDATE_INTERVAL;

void setup()
{
  Serial.begin(115200);
  tft.begin();
  rtc.begin();
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  tardis.TimeZone(time_zone * 60);
  tardis.Position(latitude, longitude);
  //tardis.DstRules(3,2,11,1, 60);

  drawSun();
}



void stripRGBRow(int r, int g, int b, int row) {
  int first_led_in_row = NUM_LEDS / NUM_ROWS * row;
  int last_led_in_row  = NUM_LEDS / NUM_ROWS * (row + 1);
  for (int x = first_led_in_row; x <  last_led_in_row; x++) {
    leds[x] = CRGB(r, g, b);
  }
  FastLED.show();
}

void stripRGB(int colors[NUM_ROWS][3]){
      Serial.print("{");
  for (int row; row < NUM_ROWS; row++){
      Serial.print("{");
      Serial.print(colors[row][0]);
      Serial.print(", ");
      Serial.print(colors[row][1]);
      Serial.print(", ");
      Serial.print(colors[row][2]);
      Serial.print("}, ");
    stripRGBRow(colors[row][0], colors[row][1], colors[row][2], row);
  }
      Serial.println("}");
}





/**
    Shows current name and value of a variabl on the screen
    Allow the value to be adjusted via buttons
    Returns the new value

    @param title the name of the variable being changed
    @param value the current value of the variable
    @param val_min the minumum value for this variable
    @param val_mxn the maimum value for this variable
    @return The new value
*/
int selectValue(String title, int value, int val_min, int val_max) {
  printMenuTitle(title);
  printValue(value);
  while (!button_left.getSingleDebouncedPress()) {
    if (button_up.getSingleDebouncedPress()) {
      if (value < val_max) {
        value++;
      } else {
        value = val_min;
      }
      printValue(value);
    }
    if (button_down.getSingleDebouncedPress()) {
      if (value > val_min) {
        value--;
      } else {
        value = val_max;
      }
      printValue(value);
    }
  }
  return value;
}

void settingsMenu() {
  Serial.println("Setting");
  int8_t new_month = selectValue("Mnth", rtc.now().month(), 1, 12);
  int8_t new_day = selectValue("Day", rtc.now().day(), 1, 31);
  int16_t new_year = selectValue("Year", rtc.now().year(), 2000, 2100);
  int8_t new_hour = selectValue("Hour", rtc.now().hour(), 0, 23);
  int8_t new_minute = selectValue("Min", rtc.now().minute(), 0, 59);
  rtc.adjust(DateTime(new_year, new_month, new_day, new_hour, new_minute, 30));
  latitude = selectValue("Lat", latitude, -90, 90);
  longitude = selectValue("Long", longitude, -180, 180);
  tardis.Position(latitude, longitude);

}


void printTime(int time_hour, int time_min) {
  DateTime now = rtc.now();
  fillValue();
  tft.setCursor(7, 48);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.print(time_hour, DEC);
  tft.print(':');
  tft.println(time_min, DEC);
}

void printDate() {
  DateTime now = rtc.now();
  fillValue();
  tft.setCursor(16, 42);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print(daysOfTheMonth[now.month()]);
  tft.print(' ');
  tft.println(now.day(), DEC);
  tft.setCursor(28, 72);
  tft.print(now.year(), DEC);
}

void calculateDayParams () {
  tardis.Position(latitude, longitude);
  byte sunrise[] = {  0, 0, 0, rtc.now().day(), rtc.now().month(), rtc.now().year() - 2000};
  tardis.SunRise(sunrise);
  sunrise_minute = (sunrise[tl_hour] * 60) + sunrise[tl_minute];
  byte sunset[] = {  0, 0, 0, rtc.now().day(), rtc.now().month(), rtc.now().year() - 2000};
  tardis.SunSet(sunset);
  sunset_minute = (sunset[tl_hour] * 60) + sunset[tl_minute];
  byte phase_today[] = {  0, 0, 12, rtc.now().day(), rtc.now().month(), rtc.now().year() - 2000};
  moon_phase = tardis.MoonPhase(phase_today) * 100;

}

void displayTime() {
  calculateDayParams ();

  printTime(rtc.now().hour(), rtc.now().minute());
  delay(2000);
  printDate();
  delay(2000);
  drawSun();
  printMenuTitle("Rise");
  printTime(sunrise_minute / 60, sunrise_minute % 60);
  delay(2000);
  printMenuTitle("Set");
  printTime(sunset_minute / 60, sunset_minute % 60);
  delay(2000);
  printMenuTitle("Moon");
  printValue(moon_phase);
  delay(2000);
}

void drawSun() {
  tft.fillScreen();
  tft.fillCircle(64, 64, 62, sunYellow);
  bgColor = sunYellow;
}


void fillArc(int16_t x0, int16_t y0, int16_t r, bool side, int8_t lum, int16_t color){
  int16_t y;
  int16_t width = r*lum/100;

  for(int16_t x = x0; x < x0+width; x++ ){
    y = sqrt(sq(r)-sq((x-x0)*100/lum));
//    Serial.print("x: ");
//     Serial.print(x);
//     Serial.print(" y: ");
//     Serial.print(y);
//     Serial.print(" h: ");
//     Serial.println(y*2);
     if(side == 0){
        tft.drawFastVLine(2*x0-x, y0-y, y*2, color);
     }
     if(side == 1){
          tft.drawFastVLine(x, y0-y, y*2, color);
     }
  }
//  Serial.print("lum: ");
//  Serial.print(lum);

}



void drawMoon( uint8_t moon_phase){
  tft.fillScreen();
  uint8_t phase_buffer =0;
  if (moon_phase >= 100 - phase_buffer || moon_phase <= 0 + phase_buffer) { // New Moon
    Serial.println("New Moon");
  }else if (moon_phase >= 75 - phase_buffer && moon_phase <= 75 + phase_buffer){ //Third Quarter
    Serial.println("Third Quarter");
    fillArc(tft_width/2, tft_height/2, 62, 0, 100, WHITE);
  }else if (moon_phase >= 50 - phase_buffer && moon_phase <= 50 + phase_buffer){ //Full Moon
    Serial.println("Full Moon");
    fillArc(tft_width/2, tft_height/2, 62, 0, 100, WHITE);
    fillArc(tft_width/2, tft_height/2, 62, 1, 100, WHITE);
  }else if (moon_phase >= 25 - phase_buffer && moon_phase <= 25 + phase_buffer){ //First Quarter
    Serial.println("First Quarter");
    fillArc(tft_width/2, tft_height/2, 62, 1, 100, WHITE);
  }else if (moon_phase > 50 + phase_buffer && moon_phase < 75 - phase_buffer){ //Waning Gibbous
    Serial.println("Waning Gibbous");
    fillArc(tft_width/2, tft_height/2, 62, 0, 100, WHITE);
    fillArc(tft_width/2, tft_height/2, 62, 1, 100-(moon_phase-50)*100/25, WHITE);
  }else if (moon_phase > 75 + phase_buffer && moon_phase < 100 - phase_buffer){ //Waning Crescent
    Serial.println("Waning Crescent");
    fillArc(tft_width/2, tft_height/2, 62, 0, 100, WHITE);
    fillArc(tft_width/2, tft_height/2, 62, 0, (moon_phase-75)*100/25, BLACK);    
  }else if (moon_phase > 0 + phase_buffer && moon_phase < 25 - phase_buffer){ //Waxing Crescent
    Serial.println("Waxing Crescent");
    fillArc(tft_width/2, tft_height/2, 62, 1, 100, WHITE);
    fillArc(tft_width/2, tft_height/2, 62, 1, 100-moon_phase*100/25, BLACK); 
  }else if (moon_phase > 25 + phase_buffer && moon_phase < 50 - phase_buffer){ //Waxing Crescent
    Serial.println("Waxing Gibbous");
    fillArc(tft_width/2, tft_height/2, 62, 0, (moon_phase-25)*100/25, WHITE);
    fillArc(tft_width/2, tft_height/2, 62, 1, 100, WHITE);
  }
  Serial.print("Moon Phase: ");
  Serial.println(moon_phase);
}

void printMenuTitle(String titleString) {
  fillMenuTitle();
  int x_offset = (4 - titleString.length()) * 8;
  tft.setCursor(32 + x_offset, 16);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print(titleString);
}

void printValue(int value) {
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

void fillMenuTitle() {
  tft.fillRect(32, 16, 70, 24, bgColor);
}

void fillValue() {
  tft.fillRect(7, 48, 118, 30, bgColor);
}

uint16_t currentTimeInMinutes(){
   return rtc.now().hour() * 60 + rtc.now().minute();
}

void updateScreen(bool daytime, uint8_t moon_phase){
  Serial.println(daytime);
  if (daytime){
    drawSun();
  }else{
    drawMoon(moon_phase);
  }
}


void updateColorDisplay(){
  calculateDayParams ();
  colorstate.transitionTimes(sunrise_minute, sunset_minute);
  colorstate.updateColors(currentTimeInMinutes());
  updateScreen(colorstate.daytime, moon_phase);
  servo.attach(SERVO_PIN);
  servo.writeMicroseconds(colorstate.current_angle * 9.5 + 600); 
  delay(500);
  servo.detach();
  
  
  

}

void fastDayCycle() {
  calculateDayParams ();
  colorstate.transitionTimes(sunrise_minute, sunset_minute);
  int colorex[3][3] = {{255,255,255},{255,255,255},{255,255,255}};
  stripRGB(colorex);
  servo.attach(SERVO_PIN);
  uint16_t servo_pos;
  for(uint16_t current_time_in_minutes = 1; current_time_in_minutes <1440; current_time_in_minutes =current_time_in_minutes+5){
    colorstate.updateColors(current_time_in_minutes);
    updateScreen(colorstate.daytime, moon_phase);
    servo.detach();
    
    stripRGB(colorstate.current_colors);
    servo.attach(SERVO_PIN);
    servo_pos = colorstate.current_angle * 9.5 + 600;
    servo.writeMicroseconds(servo_pos); 

    delay(28);
    
    
    
    
  }
  servo.detach();
}

void loop()
{
  //Settings Menu
  if (button_left.getSingleDebouncedPress()) {
    settingsMenu();
    drawSun();
  }
  //Display Time, Date, SuneRise, Sunset
  if (button_up.getSingleDebouncedPress()) {
    displayTime();
    drawSun();
  }

  if (button_down.getSingleDebouncedPress()) {
    //fastDayCycle();
    updateColorDisplay();
//    for(uint8_t phase = 0; phase <= 100; phase++){
//      drawMoon(phase);
//      delay(500);
//    }
  }

if( millis() > last_update + UPDATE_INTERVAL){
  updateColorDisplay();
  last_update = millis();
}

;}

