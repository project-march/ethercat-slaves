#ifndef MASTERONLINECHECKER_H
#define MASTERONLINECHECKER_H

#include <mbed.h>

class MasterOnlineChecker
{
private:
  bool lastPing;
  int timeOutValue;
  Timer pingTimer;

public:
  MasterOnlineChecker(int timeOutValue)
  {
    this->timeOutValue = timeOutValue;
    this->lastPing = false;
    this->pingTimer.start();
  }

  bool isOnline(uint8_t masterOnline)
  {
    bool ping = masterOnline & 1;  // Last bit
    if (ping != this->lastPing)
    {
      this->pingTimer.reset();
      this->lastPing = ping;
    }
    else if (this->pingTimer.read_ms() > this->timeOutValue)
    {
      return false;
    }
    return true;
  }
};

#endif  // MASTERONLINECHECKER_H