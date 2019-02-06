#ifndef __MQTT_H
#define __MQTT_H

#include <functional>
#include <PubSubClient.h>
#include <WiFiClient.h>

#include "myiot_timer_system.h"

namespace MyIOT
{
class Mqtt : public MyIOT::ITimer
{
  enum {MAX_NUMBER_OF_SUBSCRIPTIONS = 5};

  
public:
  typedef std::function<void()> F_OnConnected;

  Mqtt():client(espClient), device_name{0}, mqtt_server{0}
  {
  }

  void setup(const char* deviceName, const char* mqttServer)
  {
    strncpy(device_name, deviceName, sizeof(device_name));
    device_name[sizeof(device_name)-1] = 0; 
    strncpy(mqtt_server, mqttServer, sizeof(mqtt_server));
    mqtt_server[sizeof(mqtt_server)-1] = 0; 

    client.setServer(mqtt_server, 1883);
    
    client.setCallback( 
      [this]
      (char* topic, byte* payload, unsigned int length)
      {this->i_callback(topic, payload, length);} 
      );
  }

  void publish(const char* topic, const char* message)
  {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s/%s", device_name, topic);
    client.publish(buffer, message);
  }

  bool subscribe(const char* topic, std::function<void(const char* message)> reaction)
  {
	 for (Subscription& sub : subscriptions)
     {
        if (sub.empty())
        {
          sub.set(topic, reaction);
          return true;
        }
     }
     return false;
  }

  virtual void expire()
  {
    check_mqtt_client();
  }

  virtual void destroy(){}

  void setOnConnected(const F_OnConnected& onConnected)
  {
	  OnConnected = onConnected;
  }

private:
   class Subscription {
      public:
        Subscription(): topic{0}
        {}
        bool empty() const { return 0 == topic[0]; }
        const char* getTopic() const {return topic;}
        void set(const char* xtopic, std::function<void(const char* message)> fCallback)
        {
          strncpy(topic, xtopic, sizeof(topic));
          topic[sizeof(topic)-1] = 0;
          callback = fCallback;
        }
        bool equals(const char* xtopic)
        {
          return 0 == strcmp(topic, xtopic);
        }
        void execute(const char* message)
        {
          if (callback) callback(message);
        }
      private:
        char topic[20];
        std::function<void(const char* message)> callback;
   } subscriptions[MAX_NUMBER_OF_SUBSCRIPTIONS];

    void subscribe(const char* topic)
    {
      char buffer[256];
      snprintf(buffer, sizeof(buffer), "%s/%s", device_name, topic);
      client.subscribe(buffer);
    }
    
    void i_callback(char* topic, byte* payload, unsigned int length)
    {
      info("MQTT callback: ", topic);
      char buffer[256] = {0};
      strncpy(buffer, (const char*)payload,  length>sizeof(buffer) ? sizeof(buffer) : length);
      topic = topic + strlen(device_name) + 1; // device_name + '/'
 	 for (Subscription& sub : subscriptions)
      {
        if (sub.equals(topic))
        {
          sub.execute(buffer);
        }
      }
    }

    void register_subscriptions()
    {
   	 for (Subscription& sub : subscriptions)
       {
          if (!sub.empty())
          {
            subscribe(sub.getTopic());
          }
       }
    }

    bool invalidConfig()
    {
      return (0 == strlen(mqtt_server)) || (0 == strlen(device_name));
    }

    void check_mqtt_client()
    {
      if (!client.connected()) 
      {
        if (invalidConfig()) return;
        
        if (client.connect(device_name))
        {
          info("MQTT client connected", device_name);
          register_subscriptions();
          if (OnConnected) OnConnected();
        }
        else
        {
          error("MQTT client failed to connect", device_name);
        }
      }
      else
      {
        client.loop();
      }
    }    

    void info(const char* msg1, const char* msg2 = NULL)
    {
      i_print("info", msg1, msg2);
    }
    void error(const char* msg1, const char* msg2 = NULL)
    {
      i_print("error", msg1, msg2);
    }

    void i_print(const char* type, const char* msg1, const char* msg2)
    {
      Serial.print("Mqtt ");
      Serial.print(device_name);
      Serial.print(" ");
      Serial.print(type);
      Serial.print(":");
      if (msg1)
      {
        Serial.print(" ");
        Serial.print(msg1);
      }
      if (msg2)
      {
        Serial.print(" ");
        Serial.print(msg2);
      }
      Serial.println("");
    }

    WiFiClient espClient;
    PubSubClient client;

    char device_name[64];
    char mqtt_server[64];

    F_OnConnected OnConnected;
};
}
#endif
