#ifndef BUTTON_H
#define BUTTON_H

#include <mbed.h>

class Button
{
private:
  Timer dbtimer; 
public:
  Button(knop, pulldown);
  bool debouncef (bool button, bool initbutton, uint8_t dbtime);
};

#endif  // BUTTON_H