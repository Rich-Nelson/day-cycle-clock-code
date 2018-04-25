#include <Pushbutton.h>
#include "RTClib.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <TimeLord.h>
#include "ColorState.h"
#include "DisplayOutput.h"

//#define DEBUG

ColorState colorstate(false);
DisplayOutput displayoutput;




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







//Colors in RGB565
#define BLACK   0x0000
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
int16_t GREY =  0x8a8a8a;
int16_t sunYellow = 0xccff00;
int16_t bgColor = 0xFFFF;


TimeLord tardis;

//Initial Settings
int8_t latitude = 40;
int16_t longitude = -75;
int8_t time_zone = -4; // non DSTint8_t time_zone = -5;

//Calulated Time Settings
uint16_t sunrise_minute;
uint16_t sunset_minute;
uint8_t moon_phase_precentage;
uint8_t moon_phase;
uint8_t phase_buffer = 2;
const bool MOON_SHADOW = 1;

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

#define UPDATE_INTERVAL 60000
long last_update = millis() - UPDATE_INTERVAL;

void setup()
{
  Serial.begin(115200);
  tft.begin();
  rtc.begin();
  displayoutput.begin();

  tardis.TimeZone(time_zone * 60);
  tardis.Position(latitude, longitude);

//  tardis.DstRules(3,2,11,1, 60);

  drawSun();
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
  drawSun();
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

void calculateDayParams () {
  tardis.Position(latitude, longitude);
  byte sunrise[] = {  0, 0, 0, rtc.now().day(), rtc.now().month(), rtc.now().year() - 2000};
  tardis.SunRise(sunrise);
  sunrise_minute = (sunrise[tl_hour] * 60) + sunrise[tl_minute];
  byte sunset[] = {  0, 0, 0, rtc.now().day(), rtc.now().month(), rtc.now().year() - 2000};
  tardis.SunSet(sunset);
  sunset_minute = (sunset[tl_hour] * 60) + sunset[tl_minute];
  byte phase_today[] = {  0, 0, 12, rtc.now().day(), rtc.now().month(), rtc.now().year() - 2000};
  moon_phase_precentage = tardis.MoonPhase(phase_today) * 100;

}

uint16_t currentTimeInMinutes(){
   return rtc.now().hour() * 60 + rtc.now().minute();
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

void displayTime() {
  calculateDayParams ();
  drawSun();
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
//  printMenuTitle("Moon");
//  printValue(moon_phase_precentage);
//  delay(2000);
}

void drawSun() {
  tft.fillScreen();
  tft.fillCircle(64, 64, 62, sunYellow);
  bgColor = sunYellow;
}


void fillArc(int32_t x0, int32_t y0, int32_t r1, bool side, int8_t percent_transition, uint8_t phase, int16_t color){
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
//        Serial.print("x: ");
//        Serial.print(x);
//        Serial.print(" y1: ");
//        Serial.print(y1);
//        Serial.print(" y2: ");
//        Serial.print(y2);
//        Serial.print(" h: ");
//        Serial.println(y1*2);
//        delay(500);
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


void updateMoon( uint8_t moon_phase_precentage){
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

void drawMoon( uint8_t moon_phase, uint8_t moon_phase_precentage, int16_t x0, int16_t color){


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

void updateScreen(bool daytime, uint8_t moon_phase_precentage){
  #ifdef DEBUG
    Serial.println(daytime);
  #endif
  if (daytime){
    drawSun();
  }else{
    updateMoon(moon_phase_precentage);
  }
}


void updateColorDisplay(){
  calculateDayParams ();
  colorstate.transitionTimes(sunrise_minute, sunset_minute);
  colorstate.updateColors(currentTimeInMinutes());
  displayoutput.stripRGB(colorstate.current_colors);
  displayoutput.servoMoveTo(colorstate.current_angle, 500);
  updateScreen(colorstate.daytime, moon_phase_precentage);
}

void fastDayCycle() {
  calculateDayParams ();
  colorstate.transitionTimes(sunrise_minute, sunset_minute);
  int colorex[3][3] = {{255,255,255},{255,255,255},{255,255,255}};
  displayoutput.stripRGB(colorex);
  uint16_t servo_pos;
  int8_t last_transition;
  displayoutput.servoAttach();
  for(uint16_t current_time_in_minutes = colorstate.transition_time[NIGHT_END]; current_time_in_minutes < colorstate.transition_time[NIGHT_MID]; current_time_in_minutes = current_time_in_minutes+5){
    colorstate.updateColors(current_time_in_minutes);

    if(last_transition != colorstate.next_transition && (colorstate.next_transition == RISE_PEAK || colorstate.next_transition == NIGHT_MID)){
          updateScreen(colorstate.daytime, moon_phase_precentage);
    }

    
    displayoutput.servoDetach();
    displayoutput.stripRGB(colorstate.current_colors);
    
    displayoutput.servoMoveTo(colorstate.current_angle, 28);
    displayoutput.servoAttach();
//    delay(28);
    last_transition = colorstate.next_transition;
  }
  for(uint16_t current_time_in_minutes = 1; current_time_in_minutes < colorstate.transition_time[NIGHT_END]; current_time_in_minutes = current_time_in_minutes+5){
    colorstate.updateColors(current_time_in_minutes);

    if(last_transition != colorstate.next_transition && (colorstate.next_transition == RISE_PEAK || colorstate.next_transition == NIGHT_MID)){
          updateScreen(colorstate.daytime, moon_phase_precentage);
    }


    displayoutput.servoDetach();
    displayoutput.stripRGB(colorstate.current_colors);
    
    displayoutput.servoMoveTo(colorstate.current_angle, 28);
    displayoutput.servoAttach();

//    delay(28);
    last_transition = colorstate.next_transition;
  }
  displayoutput.servoDetach();
}

void fastMoonCycle() {
  for(uint8_t phase = 0; phase <= 100; phase = phase + 1){
    updateMoon(phase);
    delay(250);
  }
}
void loop()
{
  //Settings Menu
  if (button_left.getSingleDebouncedPress()) {
    
    displayoutput.servoMoveTo(90, 500);
    settingsMenu();
    updateColorDisplay();
  }
  //Display Time, Date, SuneRise, Sunset
  if (button_up.getSingleDebouncedPress()) {
    displayTime();
    updateColorDisplay();
  }

  if (button_down.getSingleDebouncedPress()) {
    fastDayCycle();
//    fastMoonCycle();
    updateColorDisplay();

  }

if( millis() > last_update + UPDATE_INTERVAL){
  updateColorDisplay();
  last_update = millis();
}

;}
