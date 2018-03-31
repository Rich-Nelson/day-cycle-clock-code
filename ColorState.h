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
    int currentColors(int16_t);
    //variables
    int8_t next_transition;
    int8_t prev_transition;
    int colors[8][3][3] = {
                            {{146, 160, 200},{29, 150, 218},{160, 120, 100}},
                            {{0,  255, 200},{16, 220, 175},{32, 175, 150}},
                            {{148, 175, 255},{40, 120, 255},{45,120,200}},
                            {{148, 255, 255},{40, 50, 175},{45,50,175}},
                            {{148, 175, 255},{40, 120, 255},{45,120,200}},
                            {{0,  255, 200},{16, 220, 175},{32, 175, 150}},
                            {{146, 160, 200},{29, 150, 218},{160, 120, 100}},
                            {{146, 160, 200},{160, 125, 125},{160, 75, 100}}
                          };

  private:
    int16_t transition_time[transition_array_size] = {};
    bool _debug_print;
    int16_t prev_transition_time;
    int16_t next_transition_time;
    // int16_t next_transition_time;
    // int16_t ;
};

#endif
