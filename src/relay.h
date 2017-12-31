#ifndef MY_IOT_RELAY_H
#define MY_IOT_RELAY_H

#include "myiot_timer_system.h"

class Relay : public MyIOT::ITimer
{
  const static unsigned long SIXTY_SECONDS = 60e3;
 
  enum {
    PIN = 12
  };
  
  public:
  Relay(int xpin = PIN):state(false), last_relay_status(false), last_status_update(0), pin(xpin), start_on(0), seconds_on(0){}

  void enable(bool xstate){state = xstate;}

  void toggle()
  {
	  state = !digitalRead(pin);
  }

  void timed_on(int seconds)
  {
    seconds_on = seconds;
    if (0 < seconds)
    {
      start_on = millis();
      enable(true);
      expire();
    }
  }

  void check_timed_on()
  {
    if (0 < seconds_on)
    {
      unsigned long delay_ms = seconds_on * 1e3;
      if (delay_ms < (millis() - start_on))
      {
        seconds_on = 0;
        start_on = 0;
        enable(false);
      }
    }
  }
  
  void setup(const std::function<void(const char* topic, const char* message)>& xpublish){
        publish = xpublish;
        pinMode(pin, OUTPUT);  
    }

  void setOnStateChanged(std::function<void(bool state)>& xOnStateChanged)
  {
    onStateChanged = xOnStateChanged;
  }

  virtual void expire()
  {
    check_timed_on();
    digitalWrite(pin, state? HIGH:LOW);
    if (isStateChanged() && onStateChanged) onStateChanged(state);
    publish_status();
    last_relay_status = state;
  }
  virtual void destroy(){}

private:
    bool isStateChanged() const
    {
      return last_relay_status != state;
    }

    void publish_status()
    {
      unsigned long current = millis();

      if (isStateChanged() || (current - last_status_update > SIXTY_SECONDS) )
      {
        const char* msg = state ? "{\"enable\":1}" : "{\"enable\":0}";
        publish("status", msg);
        last_status_update = current;
      }
    }

  bool state;
  bool last_relay_status;
  unsigned long last_status_update;
  std::function<void(const char* topic, const char* message)> publish;
  std::function<void(bool state)> onStateChanged;
  int pin;

  unsigned long start_on;
  int seconds_on;
  
};

#endif
