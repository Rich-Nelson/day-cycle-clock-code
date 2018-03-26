#include <Pushbutton.h>
#include "RTClib.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <TimeLord.h>
#include "FastLED.h"
#include "ColorState.h"


ColorState colorstate(false);


#define BUTTON_UP   16
#define BUTTON_LEFT 15
#define BUTTON_DOWN 14

Pushbutton button_up(BUTTON_UP);
Pushbutton button_left(BUTTON_LEFT);
Pushbutton button_down(BUTTON_DOWN);


RTC_DS1307 rtc;
String daysOfTheMonth[13] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Dec"};




#define __CS 8
#define __DC 9
#define __A0 10
TFT_ILI9163C tft = TFT_ILI9163C(__CS, __A0 , __DC);


#define BLACK   0x0000
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
short sunYellow = 0xccff00;
short bgColor = 0xFFFF;


TimeLord tardis;

//Initial Settings
signed char latitude = 40;
signed short longitude = -75;
signed char time_zone = -5;

//Calulated Time Settings
unsigned short sunrise_minute;
unsigned short sunset_minute;
unsigned char moon_phase;

void setup()
{
  Serial.begin(115200);
  tft.begin();
  rtc.begin();

  tardis.TimeZone(time_zone * 60);
  tardis.Position(latitude, longitude);
  //tardis.DstRules(3,2,11,1, 60);

  drawSun();

  test();
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
  char new_month = selectValue("Mnth", rtc.now().month(), 1, 12);
  char new_day = selectValue("Day", rtc.now().day(), 1, 31);
  short new_year = selectValue("Year", rtc.now().year(), 2000, 2100);
  char new_hour = selectValue("Hour", rtc.now().hour(), 0, 23);
  char new_minute = selectValue("Min", rtc.now().minute(), 0, 59);
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
  moon_phase = tardis.MoonPhase(phase_today)*100;

}

void displayTime() {
  calculateDayParams ();

  printTime(rtc.now().hour(), rtc.now().minute());
  delay(2000);
  printDate();
  delay(2000);
  drawSun();
  printMenuTitle("Rise");
  printTime(sunrise_minute/60, sunrise_minute%60);
  delay(2000);
  printMenuTitle("Set");
  printTime(sunset_minute/60, sunset_minute%60);
  delay(2000);
  printMenuTitle("Moon");
  printValue(moon_phase);
  delay(2000);
}

void drawSun() {
  tft.fillCircle(64, 64, 62, sunYellow);
  bgColor = sunYellow;
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
    test();
  }

}

void test(){
  calculateDayParams ();    
  colorstate.transitionTimes(sunrise_minute, sunset_minute);
  uint16_t current_time_in_minutes = rtc.now().hour() * 60 + rtc.now().minute();

  
  colorstate.nextTransition(current_time_in_minutes);
  Serial.print("Next Transition: ");
  Serial.println(colorstate.next_transition);
}

