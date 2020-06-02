#include "Button.h"

Button::Button(PinName pin, PinMode mode, uint64_t debounceTime) : input(pin, mode)
{
  this->debounceTimer.reset();
  this->debounceTime = debounceTime;  // debouncetime in us
}

bool Button::debounceRead(bool initialButton)
{  // Function to return debounced value
  bool returnValue = initialButton;
  bool readValue = this->input.read();
  if (initialButton != readValue && this->debounceTimer.read_us() == 0)  // Is the buttonstate changed? & is it not
                                                                         // bouncing anymore?
  {
    returnValue = readValue;      // Yes the button gets the new value
    this->debounceTimer.start();  // Start timer for debounce time
  }

  if (this->debounceTimer.read_us() >= this->debounceTime)  // debounce time has passed, reset timer
  {
    this->debounceTimer.stop();
    this->debounceTimer.reset();
  }

  return returnValue;
}

bool Button::read()
{
  return this->input.read();
}