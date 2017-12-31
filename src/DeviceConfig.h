#ifndef __DEVICE_CONFIG_H__
#define __DEVICE_CONFIG_H__

#include <FS.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

class DeviceConfig
{
  static constexpr const char *CONFIG_FILE = "/config.json";
  
  public:
  DeviceConfig():deviceName{0}, mqttServer{0}, state{0} {}

  const char* getDeviceName() const{return deviceName;}
  const char* getMqttServer() const {return mqttServer;}
  const char* getState() const {return state;}

  void setDeviceName(const char* name) { secureCopy(deviceName, name, sizeof(deviceName)); }
  void setMqttServer(const char* server) { secureCopy(mqttServer, server, sizeof(mqttServer)); }
  void setState(const char* server) { secureCopy(state, server, sizeof(state)); }
  
  void setup()
  {
     fsReadConfig();
     WiFiManager wifiManager;
     WiFiManagerParameter custom_device_name("device", "Device Name", deviceName, sizeof(deviceName));
     WiFiManagerParameter custom_mqtt_server("server", "MQTT Server", mqttServer, sizeof(mqttServer));
     
     wifiManager.addParameter(&custom_device_name);
     wifiManager.addParameter(&custom_mqtt_server);

     wifiManager.setSaveConfigCallback([](){saveConfig = true;});
     wifiManager.autoConnect();  

     if (saveConfig)
     {
       strncpy(deviceName, custom_device_name.getValue(), sizeof(deviceName));
       strncpy(mqttServer, custom_mqtt_server.getValue(), sizeof(mqttServer));
       fsSaveConfig();
     }

     WiFi.hostname(this->getDeviceName());
     IPAddress localIp = WiFi.localIP();
     Serial.print("localIp: ");
     Serial.println(localIp.toString());
  }

  void save(){fsSaveConfig();}
  
  private:

    void error(const char* msg)
    {
      Serial.print("DeviceConfig error: ");
      Serial.println(msg);
    }
  
    void info(const char* msg)
    {
      Serial.print("DeviceConfig info: ");
      Serial.println(msg);
    }
    void info(const char* msg1, const char* msg2)
    {
      Serial.print("DeviceConfig info: ");
      Serial.print(msg1);
      Serial.print(" ");
      Serial.println(msg2);
    }

    void secureCopy(char* buffer, const char* value, size_t bufferLength)
    {
	strncpy(buffer, value, bufferLength);
	buffer[bufferLength-1] = 0;
    }

    bool readValue(JsonObject& json, const char* valueName, char* buffer, size_t bufferLength)
    {
      auto jsonValue = json[valueName];
      if (!jsonValue.success()) return false;
      secureCopy(buffer, jsonValue, bufferLength);
      info(valueName, buffer);
      return true;
    }

    void fsReadConfig()
    {
      info("fsReadConfig()");
      if (SPIFFS.begin())
      {
        if (SPIFFS.exists(CONFIG_FILE))
        {
          File configFile = SPIFFS.open(CONFIG_FILE, "r");
          if (configFile)
          {
            size_t size = configFile.size();
            std::unique_ptr<char[]> buffer(new char[size]); // dynamic memory !!!
            configFile.readBytes(buffer.get(), size);
            configFile.close();
            
            DynamicJsonBuffer jsonBuffer;
            JsonObject& json = jsonBuffer.parseObject(buffer.get());
            if (json.success())
            {
        	readValue(json, "device_name", deviceName, sizeof(deviceName));
        	readValue(json, "mqtt_server", mqttServer, sizeof(mqttServer));
        	readValue(json, "state", state, sizeof(state));
            }
            else error("failed to parse config file data");
          }
          else error("failed to open config file");
        }
        else info("no config file");
      }
      else error("SPIFFS.begin() failed");
    }

    void fsSaveConfig()
    {
      info("fsSaveConfig()");
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();

      info("device_name", deviceName);
      info("mqtt_server", mqttServer);
                      
      json["device_name"] = deviceName;
      json["mqtt_server"] = mqttServer;
      json["state"] = state;
      File configFile = SPIFFS.open(CONFIG_FILE, "w");
      if (configFile)
      {
        json.printTo(configFile);
        configFile.close();
      } 
      else error ("failed to save config file");
    }

     char deviceName[40];
     char mqttServer[40];
     char state[40];
     static bool saveConfig;
};

bool DeviceConfig::saveConfig = false;

#endif
