#ifndef MYIOT_LED_H_
#define MYIOT_LED_H_

#include "myiot_timer_system.h"
class Led : public MyIOT::ITimer
{
  public:
  Led(int xpin = LED_BUILTIN):state(0),pin(xpin){}
  void setup()
  {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(pin, OUTPUT);    
  }
  virtual void expire()
  {
    digitalWrite(pin, !digitalRead(12)); // check state of relay
    // digitalWrite(pin, 0 != state%5 ? HIGH : LOW); 
    state++;
  }
  virtual void destroy(){}

  private:
    int state;
    int pin;
};

#endif
