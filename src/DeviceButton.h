#ifndef __DEVICE_BUTTON_H__
#define __DEVICE_BUTTON_H__

#include <functional>
#include <ESP8266WiFi.h>
#include "myiot_timer_system.h"

class DeviceButton: public MyIOT::ITimer
{  
    const static unsigned long TEN_SECONDS = 10e3;
    const static unsigned long THREE_SECONDS = 3e3;
  
  public:
    DeviceButton(int xpin):start(0), measuring(false), pin(xpin)
    {
    }

    void setup(std::function<void()> reaction)
    {
      onButtonPressed = reaction;
      pinMode(pin, INPUT); 
    }
    
    virtual void expire()
    {
      bool high = digitalRead(pin);
      if (!high)
      {
        if (!measuring)
        {
          measuring = true;
          start = millis();
        }
      }
      else
      {
        if (measuring)
        {
          measuring = false;
          unsigned long hold_time = millis() - start;
  
          if (hold_time > TEN_SECONDS)
          {
            info("Reset Flash");
            WiFi.disconnect();
            delay(1000);
            ESP.reset();
            delay(1000);
          }
          else if (hold_time > THREE_SECONDS)
          {
            info("Reset Device");
            ESP.reset();
            delay(1000);        }
          else
          {
            info("Button pressed");
            if (onButtonPressed)
            {
              onButtonPressed();
            }
            /*
              const char* msg = "Button pressed: go to sleep";
              publish("button", msg);
              delay(100);
              ESP.deepSleep(10e6,WAKE_RF_DEFAULT);
            */
          }
        }
      }
    }
    virtual void destroy(){}
  private:
  
    void info(const char* msg)
    {
      Serial.print("DeviceButton info: ");
      Serial.println(msg);
    }
    void info(const char* msg1, const char* msg2)
    {
      Serial.print("DeviceButton info: ");
      Serial.print(msg1);
      Serial.print(" ");
      Serial.println(msg2);
    }
  
    unsigned long start;
    bool measuring;
    int pin;
    std::function<void()> onButtonPressed;
};

#endif
