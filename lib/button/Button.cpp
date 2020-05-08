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
  if (initialButton != readValue && this->debounceTimer.read_us() == 0)  // Is the button in the same state? has it
                                                                         // stopped debouncing?
  {
    returnValue = readValue;      // Yes the button is the new value
    this->debounceTimer.start();  // Start timer again, for next check
  }

  if (this->debounceTimer.read_us() >= this->debounceTime)  // debounce time has passed, reset timer
  {
    this->debounceTimer.stop();   // A change has happened
    this->debounceTimer.reset();  // A change has happened
  }

  return returnValue;
}

bool Button::read()
{
  return this->input.read();
}