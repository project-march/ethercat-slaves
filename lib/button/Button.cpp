#include "button.h"

Button::Button(PinName pin, PinMode mode, uint8_t debounceTime) : input(pin, mode){
    this->debounceTimer.reset();
    this->debounceTime = debounceTime;    //debouncetime in us
}
    
bool Button::debounceRead (bool initialButton) 
{                                             // Function to return debounced value
  bool returnValue = initialButton;
  bool ReadValue = this->input.read();
  if (initialButton != ReadValue)                   // Is the button in the same state?
  {  
    if (this->debounceTimer.read_us() > debounceTime)     // Howmuch time has passed? is this long enough
    {
      returnValue = ReadValue;                            // Yes the button is the new value
    }
  }
  else
  {
    this->debounceTimer.reset();                    // A change has happened
    this->debounceTimer.start();                    // Start timer again, for next check
  } 
  return returnValue;
}

bool Button::read(){
    return this->input.read();
}