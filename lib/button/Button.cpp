#include "button.h"

Button::Button(){
    this->dbtimer.reset();
}
    
bool Button::debouncef (bool button, bool initbutton, uint8_t dbtime) //dbtime in us
{                                             // Function to check if the button is debounced
  if (initbutton == button)                   // Is the button in the same state?
  {  
    if (this->dbtimer.read_us() > dbtime)     // Howmuch time has passed? is this long enough
    {
      return true;                            // Yes the button is not bouncing anymore
    }
    else return false;                        // No the button is bouncing.
  }
  else
  {
    this->dbtimer.reset();                    // A change has happened
    this->dbtimer.start();                    // Start timer again, for next check
    return false;                             // The button has changed ergo is still bouncing
  } 
}