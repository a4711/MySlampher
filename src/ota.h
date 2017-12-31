#ifndef OTA_H_
#define OTA_H_


#include <ArduinoOTA.h>
#include "myiot_timer_system.h"

class OTA : public MyIOT::ITimer
{
  public:
  OTA(){}
  void setup(const char* name){
	  if (name)
	  {
		  ArduinoOTA.setHostname(name);
	  }

      ArduinoOTA.onStart([]() {
        Serial.println("Start");
      });
      ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
      });
      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      });
      ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
      });
      ArduinoOTA.begin();  
    }
  virtual void expire()
  {
      ArduinoOTA.handle();
  }
  virtual void destroy(){}
} ota;


#endif
