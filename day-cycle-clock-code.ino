#include <Pushbutton.h>
#include "RTClib.h"
#include <TimeLord.h>
#include "ColorState.h"
#include "DisplayOutput.h"
#include <Encoder.h>


//#define DEBUG

ColorState colorstate(false);
DisplayOutput displayoutput;


#define BUTTON_UP   16 //A2
#define BUTTON_LEFT 15 //A1
#define BUTTON_DOWN 14 //A0

#define ENCODER_BUTTON 17
#define ENCODER_DT     18
#define ENCODER_CLK    19


Pushbutton button_up(BUTTON_UP);
Pushbutton button_left(BUTTON_LEFT);
Pushbutton button_down(BUTTON_DOWN);
Pushbutton encoder_button(ENCODER_BUTTON);


#define NUM_SETTINGS 5

enum TimeLimits {
  TIME_LOWER_LIMIT,
  TIME_UPPER_LIMIT

};
uint16_t timeSettingLimit[5][2]= {{0,23}, {0,59}, {1,12}, {1,31}, {1900,2100}};

int16_t currentTime[5];
void updateCurrentTimeFromRTC();
bool user_changed_time = 0;
int last_rtc_update;

uint8_t selection = HOUR;

Encoder myEnc(ENCODER_DT ,ENCODER_CLK);
uint8_t last_encoder_position  = 0;
uint8_t encoder_position;
int8_t encoderIncrement;


RTC_DS1307 rtc;

TimeLord tardis;

//Initial Settings
//Note: consider storing these in persistant storage
int8_t latitude = 40;
int16_t longitude = -75;
int8_t time_zone = -4; // non DSTint8_t time_zone = -5;

//Calulated Time Settings
uint16_t sunrise_minute;
uint16_t sunset_minute;
uint8_t moon_phase_precentage;
bool DST;

//Last time

uint32_t last_date_in_days;
uint16_t last_time_in_minutes;
uint16_t last_year;
int8_t last_latitude;
int16_t last_longitude;
bool last_daytime;
uint8_t last_moon_phase_precentage;
uint8_t last_servo_angle;

#define FORCE 1


#define RTC_UPDATE_INTERVAL 30000
#define UPDATE_INTERVAL 60000
#define TFT_RESET_INTERVAL 3600000
long last_update = millis() - UPDATE_INTERVAL;
long last_tft_reset = TFT_RESET_INTERVAL;

void setup()
{
  Serial.begin(115200);
  rtc.begin();
  displayoutput.begin();

  tardis.TimeZone(time_zone * 60);
  tardis.Position(latitude, longitude);

  //  tardis.DstRules(3,2,11,1, 60);

  #ifdef DEBUG
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    DateTime now = rtc.now();
    
    Serial.print("RTC time at startup: ");  Serial.print(now.year(), DEC);  Serial.print('/'); Serial.print(now.month(), DEC); Serial.print('/');  Serial.print(now.day(), DEC);  Serial.print(" ");  Serial.print(now.hour(), DEC); Serial.print(':');  Serial.print(now.minute(), DEC); Serial.print(':');  Serial.print(now.second(), DEC); Serial.println();
  #endif
  updateCurrentTimeFromRTC();
  displayoutput.printTime(currentTime);
//  displayoutput.updateSelector(0);
}


void updateCurrentTimeFromRTC(){
  #ifdef DEBUG
  Serial.print("Time Array Before Update: ");  Serial.print(currentTime[HOUR], DEC);  Serial.print(':'); Serial.print(currentTime[MINUTE], DEC); Serial.print(' ');  Serial.print(currentTime[MONTH], DEC);  Serial.print("/");  Serial.print(currentTime[DAY], DEC); Serial.print('/');  Serial.print(currentTime[YEAR], DEC); Serial.println();
  #endif
  DateTime now = rtc.now();
  currentTime[HOUR] = now.hour();
  currentTime[MINUTE] = now.minute();
  currentTime[MONTH] = now.month();
  currentTime[DAY] = now.day();
  currentTime[YEAR] = now.year();

  #ifdef DEBUG
  Serial.print("Time Array After Update: ");  Serial.print(currentTime[HOUR], DEC);  Serial.print(':'); Serial.print(currentTime[MINUTE], DEC); Serial.print(' ');  Serial.print(currentTime[MONTH], DEC);  Serial.print("/");  Serial.print(currentTime[DAY], DEC); Serial.print('/');  Serial.print(currentTime[YEAR], DEC); Serial.println();
  #endif
}

void updateRTCFromCurrentTime(){
  
  DateTime now = rtc.now();
  
  #ifdef DEBUG
  Serial.print("RTC Before Update: ");  Serial.print(now.year(), DEC);  Serial.print('/'); Serial.print(now.month(), DEC); Serial.print('/');  Serial.print(now.day(), DEC);  Serial.print(" ");  Serial.print(now.hour(), DEC); Serial.print(':');  Serial.print(now.minute(), DEC); Serial.print(':');  Serial.print(now.second(), DEC); Serial.println();
  #endif
  
  rtc.adjust(DateTime(currentTime[YEAR], currentTime[MONTH], currentTime[DAY],currentTime[HOUR], currentTime[MINUTE]  , now.second()));
  
  #ifdef DEBUG
  now = rtc.now();
  Serial.print("RTC After Update: ");  Serial.print(now.year(), DEC);  Serial.print('/'); Serial.print(now.month(), DEC); Serial.print('/');  Serial.print(now.day(), DEC);  Serial.print(" ");  Serial.print(now.hour(), DEC); Serial.print(':');  Serial.print(now.minute(), DEC); Serial.print(':');  Serial.print(now.second(), DEC); Serial.println();
  #endif
}

/*
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
  displayoutput.fillCircle(YELLOW);
  bool hr = false;
  if (title.equals("Hour")){
    hr = true;
  }
  displayoutput.printMenuTitle(title);
  displayoutput.printValue(value, hr);
  while (!button_left.getSingleDebouncedPress()) {
    if (button_up.getSingleDebouncedPress()) {
      if (value < val_max) {
        value++;
      } else {
        value = val_min;
      }
      displayoutput.printValue(value, hr);
    }
    if (button_down.getSingleDebouncedPress()) {
      if (value > val_min) {
        value--;
      } else {
        value = val_max;
      }
      displayoutput.printValue(value, hr);
    }
  }
  return value;
}

//int getTime(enum SettingSelection_t time_unit, DateTime now){
//    switch(time_unit){
//    case HOUR:
//      return now.hour();
//      break;
//    case MINUTE:
//      return now.minute();
//      break;
//    case MONTH:
//      return now.month();
//      break;
//    case DAY:
//      return now.day();
//      break;
//    case YEAR:
//      return now.year();
//      break;
//    }
//}

int incrementValue(enum SettingSelection_t time_unit, int increment) {
  user_changed_time = 1;
  DateTime now = rtc.now();
  Serial.print("Time Array Before User Change: ");  Serial.print(currentTime[HOUR], DEC);  Serial.print(':'); Serial.print(currentTime[MINUTE], DEC); Serial.print(' ');  Serial.print(currentTime[MONTH], DEC);  Serial.print("/");  Serial.print(currentTime[DAY], DEC); Serial.print('/');  Serial.print(currentTime[YEAR], DEC); Serial.println();
  if(currentTime[time_unit] == timeSettingLimit[time_unit][TIME_LOWER_LIMIT] && increment == -1){
        currentTime[time_unit] = timeSettingLimit[time_unit][TIME_UPPER_LIMIT];
  }
  else if (currentTime[time_unit] == timeSettingLimit[time_unit][TIME_UPPER_LIMIT] && increment == 1){
        currentTime[time_unit] = timeSettingLimit[time_unit][TIME_LOWER_LIMIT];
  }else{
    currentTime[time_unit] = currentTime[time_unit] + increment;
  }
Serial.print("Time Array After User Change: ");  Serial.print(currentTime[HOUR], DEC);  Serial.print(':'); Serial.print(currentTime[MINUTE], DEC); Serial.print(' ');  Serial.print(currentTime[MONTH], DEC);  Serial.print("/");  Serial.print(currentTime[DAY], DEC); Serial.print('/');  Serial.print(currentTime[YEAR], DEC); Serial.println();
  displayoutput.printValue(time_unit, currentTime[time_unit]);
  
}

bool daylightSavings(){
   bool DST = 0;
   int year_of_centruy = currentTime[YEAR] - 2000;
   int sunday_offset = (year_of_centruy + year_of_centruy/4 + 2) % 7;
     
   if(currentTime[MONTH] == 3 && currentTime[DAY] == (14 - sunday_offset) && currentTime[HOUR] >= 2){                                   
        DST = 1;
       }
   if(currentTime[MONTH] == 3 && currentTime[DAY] > (14 - sunday_offset) || currentTime[MONTH] > 3){
        DST = 1;
       }
   if(currentTime[MONTH] == 11 && currentTime[DAY] == (7 - sunday_offset) && currentTime[HOUR] >= 2){
        DST = 0;
       }
   if(currentTime[MONTH] == 11 && currentTime[DAY] > (7 - sunday_offset) || currentTime[MONTH] > 11 || currentTime[MONTH] < 3){
        DST = 0;
       }
       return DST;
}

void calculateDayParams () {
  tardis.Position(latitude, longitude);
  byte sunrise[] = {  0, 0, 0, currentTime[DAY], currentTime[MONTH], currentTime[YEAR] - 2000};
  tardis.SunRise(sunrise);
  sunrise_minute = ((sunrise[tl_hour] - 1 + DST) * 60) + sunrise[tl_minute];
  byte sunset[] = { 0, 0, 0, currentTime[DAY], currentTime[MONTH], currentTime[YEAR] - 2000};
  tardis.SunSet(sunset);
  sunset_minute = ((sunset[tl_hour] - 1 + DST) * 60) + sunset[tl_minute];
  byte phase_today[] = { 0, 0, 12, currentTime[DAY], currentTime[MONTH], currentTime[YEAR] - 2000};
  moon_phase_precentage = tardis.MoonPhase(phase_today) * 100;
}

uint16_t currentTimeInMinutes() {
  return currentTime[HOUR] * 60 + currentTime[MINUTE];
}


uint32_t currentDateinDays() {
  //rough calc does not account for month lengths
  return currentTime[YEAR] * 365 + currentTime[MONTH] * 30 + currentTime[DAY];
}

//void displayTime() {
//  calculateDayParams ();
//  displayoutput.drawSun();
//  displayoutput.printTime(currentTime[HOUR], currentTime[MINUTE]);
//  delay(2000);
//  displayoutput.fillCircle(YELLOW);
//  displayoutput.printDate(currentTime[MONTH], currentTime[DAY], currentTime[YEAR]);
//  delay(2000);
//  displayoutput.fillCircle(YELLOW);
//  displayoutput.printMenuTitle("Rise");
//  displayoutput.printTime(sunrise_minute / 60, sunrise_minute % 60);
//  delay(2000);
//  displayoutput.printMenuTitle("Set");
//  displayoutput.printTime(sunset_minute / 60, sunset_minute % 60);
//  delay(2000);
//  //  printMenuTitle("Moon");
//  //  printValue(moon_phase_precentage);
//  //  delay(2000);
//}

void updateCelestialBody(bool daytime, uint8_t moon_phase_precentage) {
#ifdef DEBUG
  Serial.print("Daytime: ");
  Serial.println(daytime);
#endif
  if (daytime) {
    Serial.println("Sun");
    displayoutput.drawSun();
  } else {
    Serial.println("Moon");
    displayoutput.updateMoon(moon_phase_precentage);
  }

}

void updateColorDisplay(bool force_update = 0) {
  
  if (latitude != last_latitude || longitude != last_longitude || last_date_in_days != currentDateinDays() || last_year != currentTime[YEAR] || force_update) {
    force_update = true;
    calculateDayParams ();
    colorstate.transitionTimes(sunrise_minute, sunset_minute);
    DST = daylightSavings();
    
    last_latitude = latitude;
    last_longitude = longitude;
    last_year = currentTime[YEAR];
    last_date_in_days = currentDateinDays();
    #ifdef DEBUG
      Serial.println("Day Update");
      Serial.print("DST: ");
      Serial.println(DST);
    #endif
    
  }

  if (last_time_in_minutes != currentTimeInMinutes() || force_update) {
    #ifdef DEBUG
        Serial.print("Current Time in Minutes: ");
        Serial.println(currentTimeInMinutes());
    #endif
    colorstate.updateColors(currentTimeInMinutes());
    displayoutput.stripRGB(colorstate.current_colors);

    if (colorstate.current_angle != last_servo_angle || force_update) {
      displayoutput.servoMoveTo(colorstate.current_angle, 500);
      

      last_servo_angle = colorstate.current_angle;
    }

    last_time_in_minutes = currentTimeInMinutes();
  }

    if ( last_daytime != colorstate.daytime || last_moon_phase_precentage != moon_phase_precentage || force_update){
      #ifdef DEBUG
      Serial.println("Update Celestial Body");
      #endif
      updateCelestialBody(colorstate.daytime, moon_phase_precentage);
  
      last_daytime = colorstate.daytime;
      last_moon_phase_precentage = moon_phase_precentage;
    }

}

int8_t checkIncrement(){
   uint8_t encoder_reading = myEnc.read();
   int8_t increment = 0;
  if (encoder_reading % 4 == 0){
    encoder_position = encoder_reading/4;
  }
  if (encoder_position != last_encoder_position) {
    if( (encoder_position > last_encoder_position &&  encoder_position != last_encoder_position + 63) || last_encoder_position == encoder_position + 63){
      increment = -1;
    }else{
      increment = 1;
    }
    last_encoder_position = encoder_position;
  }
  return increment;
}

//void fastDayCycle() {
//  calculateDayParams ();
//  colorstate.transitionTimes(sunrise_minute, sunset_minute);
//  int colorex[3][3] = {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}};
//  displayoutput.stripRGB(colorex);
//  uint16_t servo_pos;
//  int8_t last_transition;
//  displayoutput.servoAttach();
//  for (uint16_t current_time_in_minutes = colorstate.transition_time[NIGHT_END]; current_time_in_minutes < colorstate.transition_time[NIGHT_MID]; current_time_in_minutes = current_time_in_minutes + 5) {
//    colorstate.updateColors(current_time_in_minutes);
//
//    if (last_transition != colorstate.next_transition && (colorstate.next_transition == RISE_PEAK || colorstate.next_transition == NIGHT_MID)) {
//      updateCelestialBody(colorstate.daytime, moon_phase_precentage);
//    }
//
//
//    displayoutput.servoDetach();
//    displayoutput.stripRGB(colorstate.current_colors);
//
//    displayoutput.servoMoveTo(colorstate.current_angle, 28);
//    displayoutput.servoAttach();
//    //    delay(28);
//    last_transition = colorstate.next_transition;
//  }
//  for (uint16_t current_time_in_minutes = 1; current_time_in_minutes < colorstate.transition_time[NIGHT_END]; current_time_in_minutes = current_time_in_minutes + 5) {
//    colorstate.updateColors(current_time_in_minutes);
//
//    if (last_transition != colorstate.next_transition && (colorstate.next_transition == RISE_PEAK || colorstate.next_transition == NIGHT_MID)) {
//      updateCelestialBody(colorstate.daytime, moon_phase_precentage);
//    }
//
//
//    displayoutput.servoDetach();
//    displayoutput.stripRGB(colorstate.current_colors);
//
//    displayoutput.servoMoveTo(colorstate.current_angle, 28);
//    displayoutput.servoAttach();
//
//    //    delay(28);
//    last_transition = colorstate.next_transition;
//  }
//  displayoutput.servoDetach();
//}
//
//void fastMoonCycle() {
//  for (uint8_t phase = 0; phase <= 100; phase = phase + 1) {
//    displayoutput.updateMoon(phase);
//    delay(250);
//  }
//}


void loop()
{
  //Settings Menu
//  if (button_left.getSingleDebouncedPress()) {
//
//    displayoutput.servoMoveTo(90, 500);
//    settingsMenu();
//    updateColorDisplay(FORCE);
//  }
//  //Display Time, Date, SuneRise, Sunset
//  if (button_up.getSingleDebouncedPress()) {
//    displayTime();
//    updateColorDisplay(FORCE);
//  }
//
//  if (button_down.getSingleDebouncedPress()) {
//    fastDayCycle();
//    //    fastMoonCycle();
//    updateColorDisplay(FORCE);
//  }

    if (encoder_button.getSingleDebouncedPress()) {
    selection++;
    if (selection == NUM_SETTINGS){
      selection = 0;
    }
    displayoutput.updateSelector(selection);
    #ifdef DEBUG
      //Serial.println(selection);
    #endif
  }

  if ( millis() > last_update + UPDATE_INTERVAL) {
    updateColorDisplay();
    last_update = millis();
  }

  if ( millis() > last_rtc_update + RTC_UPDATE_INTERVAL) {
    if(user_changed_time){
      updateRTCFromCurrentTime();
    }else{
      updateCurrentTimeFromRTC();
    }
    user_changed_time = 0;
    last_rtc_update = millis();
  }

  encoderIncrement = checkIncrement();
  if(encoderIncrement != 0){
    incrementValue(selection, encoderIncrement);
    updateColorDisplay();
  }


}
