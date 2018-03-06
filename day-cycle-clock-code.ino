#include <Pushbutton.h>
#include "RTClib.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>


#define BUTTON_UP   16
#define BUTTON_LEFT 15
#define BUTTON_DOWN 14

Pushbutton button_up(BUTTON_UP);
Pushbutton button_left(BUTTON_LEFT);
Pushbutton button_down(BUTTON_DOWN);


RTC_DS1307 rtc;


#define __CS 8
#define __DC 9
#define __A0 10
TFT_ILI9163C tft = TFT_ILI9163C(__CS, __A0 , __DC);

// Color definitions
#define BLACK   0x0000
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF
uint16_t sunYellow = 0xccff00;
uint16_t bgColor = 0xFFFF;


String menu_titles[] = {"Hour","Min","Day","Mon","Year","Lat","Long"};
int latitude = 40;
int longitude = -75;

void setup()
{
  Serial.begin(115200);
  tft.begin();
  rtc.begin();
  drawSun();
  printDateTime();
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
int selectValue(String title, int value, int val_min, int val_max){
  printMenuTitle(title);
  printValue(value);
  while(!button_left.getSingleDebouncedPress()){
    if(button_up.getSingleDebouncedPress()){
      if(value<val_max){
        value++;
      }else{
        value = val_min;
      }
      printValue(value);
    }
    if(button_down.getSingleDebouncedPress()){
      if(value>val_min){
        value--;
      }else{
        value = val_max;
      }
      printValue(value);
    }
  }
  return value;
}

void settingsMenu(){
  Serial.println("Setting");
  int new_hour = selectValue("Hour", rtc.now().hour(), 0, 23); 
  int new_minute = selectValue("Min", rtc.now().minute(), 0, 59);
  int new_month = selectValue("Mnth", rtc.now().month(), 1, 12);
  int new_day = selectValue("Day", rtc.now().day(), 1, 31);
  int new_year = selectValue("Year", rtc.now().year(), 2000, 2100);
  rtc.adjust(DateTime(new_year, new_month, new_day, new_hour, new_minute, 30));
  latitude = selectValue("Lat", latitude, -90, 90);
  longitude = selectValue("Long", longitude, -180, 180);
}


void printDateTime(){
  DateTime now = rtc.now();
//  tft.fillScreen(WHITE);
  fillValue();
  tft.setCursor(7, 48);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.print(now.hour(), DEC);
  tft.print(':');
  tft.println(now.minute(), DEC);
  tft.setTextSize(4);
//  tft.print(now.month(), DEC);
//  tft.print('/');
//  tft.println(now.day(), DEC);
//  tft.print(now.year(), DEC);
  delay(1000);
}

void drawSun(){
  tft.fillCircle(64,64, 62, sunYellow);
  bgColor = sunYellow;
  }

void printMenuTitle(String titleString){
  fillMenuTitle();
  int x_offset = (4 - titleString.length()) * 8;
  tft.setCursor(32 + x_offset, 16);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print(titleString);
}

void printValue(int value){
  fillValue();
//  int x_offset = (4 - titleString.length()) * 8;
  tft.setCursor(7, 48);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.print(value, DEC);
}

void fillMenuTitle(){
  tft.fillRect(32, 16, 70, 24, bgColor);
}

void fillValue(){
  tft.fillRect(7, 48, 118, 30, bgColor);
}

void loop()
{
  if (button_left.getSingleDebouncedPress()) {
    settingsMenu();
    drawSun();
    printDateTime();
  }
  
  

}

