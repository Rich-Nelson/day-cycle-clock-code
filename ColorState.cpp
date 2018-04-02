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



  // for (int8_t i = 0; i < number_of_transition_times; i++){
  //   Serial.print(i);
  //   Serial.print(" ");
  //   Serial.println(transition_time[i]);
  // }




  return true;
}

int8_t ColorState::nextTransition(int16_t current_time_in_minutes){

    for(int8_t i = 0; i < number_of_transition_times; i++){

      if (i == night_end){
        prev_transition_time = 0;
      } else {
        prev_transition_time = transition_time[i-1];
      }

      if (i == night_mid){
        next_transition_time = 1440;
      } else {
        next_transition_time = transition_time[i];
      }

      if (current_time_in_minutes < next_transition_time && current_time_in_minutes >= prev_transition_time){
        next_transition = i;
        if (i==0){
          prev_transition = 7;
        }else{
          prev_transition = i-1;
        }

        break;
      }
    }
    return next_transition;
}

int ColorState::currentColors(int16_t current_time_in_minutes){
    int next_transition =  nextTransition(current_time_in_minutes);
    // Serial.print("Current Time: ");
    // Serial.println(current_time_in_minutes);
    // Serial.print("Prev Transition Time: ");
    // Serial.println(prev_transition_time);
    // Serial.print("Next Transition Time: ");
    // Serial.println(next_transition_time);
    int16_t time_since_last_transition = current_time_in_minutes - prev_transition_time;
    // Serial.print("Time since last: ");
    // Serial.println(time_since_last_transition);
    int8_t transition_progress = 100* time_since_last_transition / (next_transition_time - prev_transition_time);//(time_since_last_transition / (next_transition_time - prev_transition_time))*100;
    // Serial.print("Transition Progress: ");
    // Serial.println(transition_progress);
    // Serial.print("prev_transition: ");
    // Serial.println(prev_transition);
    // Serial.print("next_transition: ");
    // Serial.println(next_transition);
    for (int i=0; i < 3; i++){
      for (int j=0; j < 3; j++){
        // Serial.print(i);
        // Serial.println(j);
        // Serial.print(colors[prev_transition][i][j]);
        // Serial.print(" + ");
        // Serial.print((float)transition_progress/100);
        // Serial.print(" * (");
        // Serial.print(colors[next_transition][i][j]);
        // Serial.print(" - ");
        // Serial.print(colors[prev_transition][i][j]);
        // Serial.println(" )");
        float float_color_value = (float)colors[prev_transition][i][j] + (colors[next_transition][i][j] - colors[prev_transition][i][j])*transition_progress/100;
        current_colors[i][j] = static_cast<int>(float_color_value);
        //******************************
        // Serial.println(current_colors[i][j]); //commenting out breaks program?
        //****************S**************
      }
    }

    return current_colors;
}

uint8_t ColorState::currentAngle(int16_t current_time_in_minutes){
  if (next_transition >= rise_peak &&  next_transition <= night_start){
    current_angle = static_cast<int>((float)(western_horizon - eastern_horizon)*(current_time_in_minutes - transition_time[night_end])/(transition_time[night_start] - transition_time[night_end]));
    // Serial.print("SUN Angle: ");
    daytime = 1;
  } else if (next_transition == night_mid){
    current_angle = static_cast<int>((float)(western_horizon - eastern_horizon) - (western_horizon - eastern_horizon)/2*(current_time_in_minutes - transition_time[night_start])/(transition_time[night_mid] - transition_time[night_start]));
    // Serial.print("MOON Rising: ");
    daytime = 0;
  } else if (next_transition == night_end){
    current_angle = static_cast<int>((float)(western_horizon - eastern_horizon)/2 - (western_horizon - eastern_horizon)/2*(current_time_in_minutes)/(transition_time[night_end]));
    // Serial.print("MOON Setting: ");
    daytime = 0;
  }


  // Serial.println(current_angle);
  return current_angle;
}

bool ColorState::updateColors(int16_t current_time_in_minutes) {
  nextTransition(current_time_in_minutes);
  currentColors(current_time_in_minutes);
  currentAngle(current_time_in_minutes);
  return true;
}
