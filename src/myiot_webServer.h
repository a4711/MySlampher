#ifndef WEBSERVER_H_
#define WEBSERVER_H_


#include <ESP8266WebServer.h>

#include "myiot_DeviceConfig.h"
#include "myiot_timer_system.h"

namespace MyIOT
{
class WebServer : public MyIOT::ITimer 
{
public:

  WebServer():server(80), config(nullptr){}

  void setup(MyIOT::DeviceConfig& rconfig)
  {
    config = &rconfig;

    server.on("/", [this](){ this->printStatus();} );
    server.on("/save", [this](){ this->handleSave();} );
    server.on("/reset", [this](){ this->handleReset();} );
    server.on("/reset_wifi", [this](){ this->handleResetWiFi();} );
    server.on("/details", [this](){ this->handleDetails();} );
    server.begin();
  }

  void on(const String& uri, const ESP8266WebServer::THandlerFunction& handler)
  {
    server.on(uri, handler);
  }

  void send(int code, const char* content_type, const String& content)
  {
    server.send(code, content_type, content);
  }

  virtual void expire()
  {
    server.handleClient();
  }

  virtual void destroy(){}

private:

  void printStatus()
  {
    String html1 = "<!DOCTYPE html>\r\n<html>\r\n\
<head><title>Configuration " + String(config->getDeviceName())+ "</title></head>\r\n\
<body><form action=\"save\" method=\"GET\">\
  DeviceName <INPUT type=\"text\" name=\"deviceName\" value=\"" + String(config->getDeviceName()) + "\"><br> \
  MQTT Server <INPUT type=\"text\" name=\"mqttServer\" value=\"" + String(config->getMqttServer()) + "\"><br> \
  <INPUT type=\"submit\" value=\"Save\"><br>\
  </form>\
  <form action=\"reset\" method=\"GET\"><INPUT type=\"submit\" value=\"Reset\"><br></form>\
  <form action=\"reset_wifi\" method=\"GET\"><INPUT type=\"submit\" value=\"Reset Wifi\"><br></form>\
  <form action=\"details\" method=\"GET\"><INPUT type=\"submit\" value=\"Details\"><br></form>\
</body> \
</html>";
    server.send(200, "text/html", html1);
  }

  void handleDetails()
  {
    String html1 = "<!DOCTYPE html>\r\n<html>\r\n\
<head><title>Configuration " + String(config->getDeviceName())+ "</title></head>\r\n\
<body><form action=\"save\" method=\"GET\">\
  DeviceName <INPUT type=\"text\" name=\"deviceName\" value=\"" + String(config->getDeviceName()) + "\"><br> \
  MQTT Server <INPUT type=\"text\" name=\"mqttServer\" value=\"" + String(config->getMqttServer()) + "\"><br> \
  <INPUT type=\"submit\" value=\"Save\"><br>\
  </form>\
  <form action=\"reset\" method=\"GET\"><INPUT type=\"submit\" value=\"Reset\"><br></form>\
  <form action=\"reset_wifi\" method=\"GET\"><INPUT type=\"submit\" value=\"Reset Wifi\"><br></form>\
  <hr>" + htmlWifiInfo() + "\
  <hr>" + htmlEspInfo() + "\
  <hr>" + htmlFirmwareInfo() + "\
</body> \
</html>";
    server.send(200, "text/html", html1);
  }

  void handleSave()
  {
    String deviceName = server.arg("deviceName");
    String mqttServer = server.arg("mqttServer");
    config->setDeviceName(deviceName.c_str());
    config->setMqttServer(mqttServer.c_str());
    config->save();

    String html1 = "<!DOCTYPE html>\r\n<html>\r\n\
<head><title>Configuration " + String(config->getDeviceName())+ "</title></head>\r\n\
<body>Values saved<br>\
<a href=\"/\">Show Values</a><br>\
<a href=\"/reset\">Reset Device</a><br>\
</body></html>\
";
    server.send(200, "text/html", html1);
  }

  void handleReset()
  {
    String html1 = "<!DOCTYPE html>\r\n<html>\r\n\
<head><title>Configuration " + String(config->getDeviceName())+ "</title></head>\r\n\
<body>Reset device ...<br>\
<a href=\"/\">Show Values</a><br>\
</body></html>\
";

    server.send(200, "text/html", html1);
    delay(1000);
    ESP.reset();
    delay(1000);
  }

  void handleResetWiFi()
  {
    String html1 = "<!DOCTYPE html>\r\n<html>\r\n\
<head><title>Configuration " + String(config->getDeviceName())+ "</title></head>\r\n\
<body>Reset device ...<br>\
<a href=\"/\">Show Values</a><br>\
</body></html>\
";

    server.send(200, "text/html", html1);
    delay(1000);
    WiFi.disconnect(true);
    delay(100);
    ESP.reset();
    delay(1000);
  }
private:
  String htmlEspInfo()
  {

    String html = String("<form>") + "\
  Chip ID <INPUT readonly type=\"text\" value=\"" + ESP.getChipId() + "\"><br> \
  Flash Size <INPUT readonly type=\"text\" value=\"" + ESP.getFlashChipSize() + "\"><br> \
  Real Flash Size <INPUT readonly type=\"text\" value=\"" + ESP.getFlashChipRealSize() + "\"><br> \
  Flash Chip ID <INPUT readonly type=\"text\" value=\"" + ESP.getFlashChipId() + "\"><br> \
  Flash Mode <INPUT readonly type=\"text\" value=\"" + ESP.getFlashChipMode() + "\"><br> \
  Free Heap <INPUT readonly type=\"text\" value=\"" + ESP.getFreeHeap() + "\"><br> \
  CpuFreq <INPUT readonly type=\"text\" value=\"" + ESP.getCpuFreqMHz() + "\"><br> \
  Vcc <INPUT readonly type=\"text\" value=\"" + ESP.getVcc() + "\"><br> \
  Reset Reason: " + ESP.getResetReason() + "<br> \
        </form>\
         ";
    return html;

  }

  String htmlFirmwareInfo()
  {
    String html = String("<form>") + "\
  SDK Version<INPUT readonly type=\"text\" value=\"" + ESP.getSdkVersion() + "\"><br> \
  Core Version <INPUT readonly type=\"text\" value=\"" + ESP.getCoreVersion() + "\"><br> \
  Sketch Size <INPUT readonly type=\"text\" value=\"" + ESP.getSketchSize() + "\"><br> \
  Free Sketch Size <INPUT readonly type=\"text\" value=\"" + ESP.getFreeSketchSpace() + "\"><br> \
        </form>\
         ";
    return html;


  }

  String htmlWifiInfo()
  {
    IPAddress ip = WiFi.localIP();
    IPAddress subnet = WiFi.subnetMask();
    IPAddress gateway = WiFi.gatewayIP();
    IPAddress dns0 = WiFi.dnsIP(0);
    IPAddress dns1 = WiFi.dnsIP(1);

    String html =
        "<form>\
        Hostname <INPUT readonly type=\"text\" value=\"" + WiFi.hostname() + "\"><br> \
        SSID <INPUT readonly type=\"text\" value=\"" + WiFi.SSID() + "\"><br> \
        BSSID <INPUT readonly type=\"text\" value=\"" + WiFi.BSSIDstr() + "\"><br> \
        WiFi mode <INPUT readonly type=\"text\" value=\"" + WiFi.getMode() + "\"><br> \
        IP Address <INPUT readonly type=\"text\" value=\"" + ip.toString() + "\"><br> \
        Subnet <INPUT readonly type=\"text\" value=\"" + subnet.toString() + "\"><br> \
        DNS 0 <INPUT readonly type=\"text\" value=\"" + dns0.toString() + "\"><br> \
        DNS 1 <INPUT readonly type=\"text\" value=\"" + dns1.toString() + "\"><br> \
        Gateway <INPUT readonly type=\"text\" value=\"" + gateway.toString() + "\"><br> \
        MAC Address <INPUT readonly type=\"text\" value=\"" + WiFi.macAddress() + "\"><br> \
        Receive Signal Strength (RSSI) <INPUT readonly type=\"text\" value=\"" + WiFi.RSSI() + "\"><br> \
        </form>\
         ";
    return html;
  }


private:

  ESP8266WebServer server; 
  MyIOT::DeviceConfig* config;
};
}
#endif
