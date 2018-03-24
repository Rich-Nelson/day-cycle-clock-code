#ifndef cs
#define cs

#if (ARDUINO >=100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

class ColorState {
  public:
    // Constructor
    ColorState();
    // Configuration
    bool RiseSetTimes(char sunrise_hour,
                      char sunrise_minute,
                      char sunset_hour,
                      char sunset_minute);
    
    // Mehods
    bool updateColors();
    int riseTime();
    
  private:
    int sunrise_in_minutes;
    int sunset_in_minutes;
  
};



#endif
