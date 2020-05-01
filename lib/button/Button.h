#ifndef BUTTON_H
#define BUTTON_H

#include <mbed.h>

class Button
{
private:
  Timer debounceTimer; 
  DigitalIn input; 
  uint8_t debounceTime;
public:
  Button(PinName pin, PinMode mode, uint8_t debounceTime);     //constructor
  bool debounceRead (bool initialbutton);
  bool read();
  
};

#endif  // BUTTON_H