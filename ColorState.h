#ifndef cs
#define cs

#if (ARDUINO >=100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

enum DayTransitions {NIGHT_END,
                      RISE_PEAK,
                      DAY_START,
                      DAY_MID,
                      DAY_END,
                      SET_PEAK,
                      NIGHT_START,
                      NIGHT_MID};

#define number_of_transition_times 8

// #define DEBUG

#define eastern_horizon 0
#define western_horizon 170

#define rise_set_duration 180

class ColorState {
  public:
    // Constructor
    ColorState(bool debug_print);
    // Configuration
    bool transitionTimes(int16_t sunrise_minute,
                         int16_t sunset_minute);

    // Mehods
    bool updateColors(int16_t);
    int8_t nextTransition(int16_t);
    int currentColors(int16_t);
    uint8_t currentAngle(int16_t);
    //variables
    int16_t transition_time[number_of_transition_times] = {};
    int8_t next_transition;
    int8_t prev_transition;
    uint8_t current_angle;
    bool daytime;
    int current_colors[3][3];
    int colors[8][3][3] = {
                            {{4,35,115},{35,30,25},{0,15,52}},
                            {{245, 15, 0},{225,70,0},{247,103,10}},
                            {{50, 100, 255},{100, 70, 50},{70,50,45}},
                            {{79, 170, 255},{180, 140, 140},{120, 100, 100}},
                            {{50, 100, 255},{150,80,15},{80,50,30}},
                            {{253,60,15},{189,32,40},{132,103,159}},
                            {{4,35,115},{125,100,45},{0,15,52}},
                            {{0,0,80},{15,10,30},{0,5,20}}
                          };

  private:

    bool _debug_print;
    int16_t prev_transition_time;
    int16_t next_transition_time;
    // int16_t next_transition_time;
    // int16_t ;
};

#endif
