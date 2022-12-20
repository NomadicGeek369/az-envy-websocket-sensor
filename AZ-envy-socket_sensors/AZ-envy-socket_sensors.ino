#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <stdio.h>
#include "secrets.h" // Remove this line if you havn't created one yourself
#include <SHT3x.h>
#include <MQ2.h>

// Network settings
const char* ssid = SSID; // Your wifi name like "myWifiNetwork"
const char* password = PASSWORD; // Your password to the wifi network like "password123"
const char* websocket_server_host = "192.168.0.150"; // The webservers IP address
const uint16_t websocket_server_port = 8887; // The port which is used to identify this device

// Sensor settings
int mq2pin = A0;
float lpg, co, smoke;
SHT3x TempHumSensor;
MQ2 mq2(mq2pin);

using namespace websockets;
WebsocketsClient client;

void setup() 
{
  WiFi.begin(ssid, password);
  Serial.begin(115200);

  while (WiFi.status() != WL_CONNECTED) { Serial.println("Connecting network!"); delay(500); }

  delay(5000);
  TempHumSensor.Begin();
  mq2.begin();
}

void loop() 
{
  while(!client.connect(websocket_server_host, websocket_server_port, "/")) { Serial.println("Connecting client!"); delay(500); }

  TempHumSensor.UpdateData();
  float lpg = mq2.readLPG();
  float co = mq2.readCO();
  float smoke = mq2.readSmoke();
  float hum = TempHumSensor.GetRelHumidity();
  float temp = TempHumSensor.GetTemperature();
  
  String output = "temp=" + String(temp, 2) + ",hum=" + String(hum, 2) + ",lpg="+ String(lpg, 2) + ",co="+ String(co, 2) + ",smoke="+ String(smoke, 2);
  Serial.println(output);

  client.send(output);

  delay(300);
}
