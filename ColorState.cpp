#include "ColorState.h"


ColorState::ColorState(bool debug){
}

bool ColorState::transitionTimes(unsigned short sunrise_minute,
                                 unsigned short sunset_minute){
  transition_time[night_end] = sunrise_minute - rise_set_length / 2;
  transition_time[rise_peak] = sunrise_minute;
  transition_time[day_start] = sunrise_minute + rise_set_length / 2;
  transition_time[day_mid] = sunrise_minute + (sunset_minute - sunrise_minute)/2;
  transition_time[day_end] = sunset_minute - rise_set_length / 2;
  transition_time[set_peak] = sunset_minute;
  transition_time[night_start] = sunset_minute + rise_set_length / 2;
  transition_time[night_mid] = sunset_minute + (sunrise_minute + 1440 - sunset_minute)/2;

  if (transition_time[night_mid] > 1440){
    transition_time[night_mid] = transition_time[night_mid] - 1440;
  }

  for (int i; i < 8; i++){
    Serial.print(i);
    Serial.print(" ");
      Serial.println(transition_time[i]);
  }

  return true;
}

bool ColorState::updateColors() {

  return true;
}
