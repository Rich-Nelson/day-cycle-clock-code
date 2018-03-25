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

#define rise_set_length 120

class ColorState {
  public:
    // Constructor
    ColorState(bool debug);
    // Configuration
    bool transitionTimes(unsigned short sunrise_minute,
                         unsigned short sunset_minute);

    // Mehods
    bool updateColors();


  private:
    unsigned short transition_time[7] = {};
};

#endif
