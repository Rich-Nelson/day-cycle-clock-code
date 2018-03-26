#ifndef cs
#define cs

#if (ARDUINO >=100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define night_end 0
#define rise_peak 1
#define day_start 2
#define day_mid 3
#define day_end 4
#define set_peak 5
#define night_start 6
#define night_mid 7


#define transition_array_size 8

#define rise_set_duration 120

class ColorState {
  public:
    // Constructor
    ColorState(bool debug_print);
    // Configuration
    bool transitionTimes(int16_t sunrise_minute,
                         int16_t sunset_minute);

    // Mehods
    bool updateColors();
    int8_t nextTransition(int16_t);

    //variables
    int8_t next_transition;


  private:
    int16_t transition_time[transition_array_size] = {};
    bool _debug_print;
    int16_t transition_prev;
    int16_t transition_next;

};

#endif
