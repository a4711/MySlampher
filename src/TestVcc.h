#ifndef MYIOT_TESTVCC_H_
#define MYIOT_TESTVCC_H_

#include <ESP.h>
#include "myiot_timer_system.h"

ADC_MODE(ADC_VCC);
class TestVcc : public MyIOT::ITimer
{
  public:
  TestVcc(){}
  
  void setup(std::function<void(const char*, const char*)> fpub)
  {
    publish = fpub;;
  }
  virtual void expire()
  {
    if (publish)
    {
      uint16_t vcc = ESP.getVcc();
      char buffer[256];
      snprintf(buffer, sizeof(buffer), "{\"vcc\", %d}", vcc);
      publish("vcc", buffer);
    }
  }
  virtual void destroy(){}  

  private:
    std::function<void(const char*,const char*)> publish;
} testVcc;

#endif
