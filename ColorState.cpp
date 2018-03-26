#include "ColorState.h"


ColorState::ColorState(bool debug_print){
}

bool ColorState::transitionTimes(int16_t sunrise_minute,
                                 int16_t sunset_minute){
  transition_time[night_end] = sunrise_minute - rise_set_duration / 2;
  transition_time[rise_peak] = sunrise_minute;
  transition_time[day_start] = sunrise_minute + rise_set_duration / 2;
  transition_time[day_mid] = sunrise_minute + (sunset_minute - sunrise_minute)/2;
  transition_time[day_end] = sunset_minute - rise_set_duration / 2;
  transition_time[set_peak] = sunset_minute;
  transition_time[night_start] = sunset_minute + rise_set_duration / 2;
  transition_time[night_mid] = 1440; //sunset_minute + (sunrise_minute + 1440 - sunset_minute)/2;



  for (int8_t i = 0; i < transition_array_size; i++){
    Serial.print(i);
    Serial.print(" ");
    Serial.println(transition_time[i]);
  }




  return true;
}

int8_t ColorState::nextTransition(int16_t current_time_in_minutes){

    for(int8_t i = 0; i < transition_array_size; i++){

      if (i == night_end){
        transition_prev = 0;
      } else {
        transition_prev = transition_time[i-1];
      }

      if (i == night_mid){
        transition_next = 1440;
      } else {
        transition_next = transition_time[i];
      }

      if (current_time_in_minutes < transition_next && current_time_in_minutes >= transition_prev){
        next_transition = i;
      }
    }
    return next_transition;
}

bool ColorState::updateColors() {

  return true;
}
