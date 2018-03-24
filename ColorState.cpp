#include "ColorState.h"

ColorState::ColorState(){

}

bool ColorState::RiseSetTimes(char sunrise_hour, char sunrise_minute, char sunset_hour, char sunset_minute){
   sunrise_in_minutes =  sunrise_hour*60 + sunrise_minute;
   sunset_in_minutes =  sunset_hour*60 + sunset_minute;

   return true;
}

bool ColorState::updateColors() {
  
  return true;
}

int ColorState::riseTime() {
  
  return sunrise_in_minutes;
}

