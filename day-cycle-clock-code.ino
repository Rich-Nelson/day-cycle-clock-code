#include <Pushbutton.h>
#include "RTClib.h"
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

TimeLord tardis;

//Initial Settings
int8_t latitude = 40;
int16_t longitude = -75;
int8_t time_zone = -4; // non DSTint8_t time_zone = -5;

//Calulated Time Settings
uint16_t sunrise_minute;
uint16_t sunset_minute;
uint8_t moon_phase_precentage;


#define UPDATE_INTERVAL 60000
long last_update = millis() - UPDATE_INTERVAL;

void setup()
{
  Serial.begin(115200);
  rtc.begin();
  displayoutput.begin();

  tardis.TimeZone(time_zone * 60);
  tardis.Position(latitude, longitude);

//  tardis.DstRules(3,2,11,1, 60);

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
  displayoutput.printMenuTitle(title);
  displayoutput.printValue(value);
  while (!button_left.getSingleDebouncedPress()) {
    if (button_up.getSingleDebouncedPress()) {
      if (value < val_max) {
        value++;
      } else {
        value = val_min;
      }
      displayoutput.printValue(value);
    }
    if (button_down.getSingleDebouncedPress()) {
      if (value > val_min) {
        value--;
      } else {
        value = val_max;
      }
      displayoutput.printValue(value);
    }
  }
  return value;
}

void settingsMenu() {
  displayoutput.drawSun();
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



void displayTime() {
  calculateDayParams ();
  displayoutput.drawSun();
  displayoutput.printTime(rtc.now().hour(), rtc.now().minute());
  delay(2000);
  displayoutput.printDate(rtc.now().month(), rtc.now().day(), rtc.now().year());
  delay(2000);
  displayoutput.drawSun();
  displayoutput.printMenuTitle("Rise");
  displayoutput.printTime(sunrise_minute / 60, sunrise_minute % 60);
  delay(2000);
  displayoutput.printMenuTitle("Set");
  displayoutput.printTime(sunset_minute / 60, sunset_minute % 60);
  delay(2000);
//  printMenuTitle("Moon");
//  printValue(moon_phase_precentage);
//  delay(2000);
}







void updateScreen(bool daytime, uint8_t moon_phase_precentage){
  #ifdef DEBUG
    Serial.println(daytime);
  #endif
  if (daytime){
    displayoutput.drawSun();
  }else{
    displayoutput.updateMoon(moon_phase_precentage);
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
    displayoutput.updateMoon(phase);
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
