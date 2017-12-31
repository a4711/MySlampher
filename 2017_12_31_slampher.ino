#include <Arduino.h>
#include "src/myiot_timer_system.h"
#include "src/DeviceConfig.h"
#include "src/webServer.h"
#include "src/mqtt.h"
#include "src/ota.h"

#include "src/DeviceButton.h"
#include "src/led.h"
#include "src/relay.h"
#include "src/TestVcc.h"

MyIOT::TimerSystem tsystem;
Mqtt mqtt;
DeviceConfig config;
DeviceButton button(0); // GPIO 0 is the button
Led led(13);  // LED of sonoff
Relay relay(12);

void setup() {
   Serial.begin(115200);

   config.setup();
   mqtt.setup(config.getDeviceName(), config.getMqttServer());
   ota.setup(config.getDeviceName());
   webServer.setup(config);

   led.setup();
   relay.setup([](const char* topic, const char* msg){mqtt.publish(topic,msg);} );

   std::function<void(bool)> onStateChanged = [](bool state)   {config.setState(state?"1":"0"); config.save();};
   relay.setOnStateChanged( onStateChanged );
   relay.enable(0 == strcmp(config.getState(), "1")); //keep old state

   // button.setup([](){ mqtt.publish("button", "pressed"); }  );
   button.setup([](){ relay.toggle(); }  );
   testVcc.setup([](const char* topic, const char* msg){mqtt.publish(topic,msg);}  );

   mqtt.subscribe("control", [] (const char*msg) {
		if (0 == ::strcmp("on", msg))
		{
			relay.enable(true);
		}
		else if (0 == ::strcmp("off",msg))
		{
			relay.enable(false);
		}
		else if (0 == ::strcmp("toggle", msg))
		{
			relay.toggle();
		}
		else
		{
			int cmd = ::atoi(msg);
			if (cmd > 0)
			  relay.timed_on(cmd); // Schalte für N Sekunden ein.
			else
			  relay.enable(::atoi(msg) == 1);
		}
   });


   tsystem.add(&ota, MyIOT::TimerSystem::TimeSpec(0,10e6));
   tsystem.add(&mqtt, MyIOT::TimerSystem::TimeSpec(0,10e6));
   tsystem.add(&webServer, MyIOT::TimerSystem::TimeSpec(0,100e6));

   tsystem.add(&led, MyIOT::TimerSystem::TimeSpec(0,500000000));
   tsystem.add(&relay, MyIOT::TimerSystem::TimeSpec(0,10e6));
   tsystem.add(&button, MyIOT::TimerSystem::TimeSpec(0,100e6));
   tsystem.add(&testVcc, MyIOT::TimerSystem::TimeSpec(3));
}



void loop() {
  tsystem.run_loop(10,1);
}
