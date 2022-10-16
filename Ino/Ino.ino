
#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"

//wifi
#include <WiFi.h>
#include <WiFiUdp.h>


MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

//wifi

const char* ssid = "JAOT_2.4";
const char* password = "juan1234";
WiFiUDP udpDevice;
uint16_t localUdpPort = 44444;
uint32_t previousMillis = 0;

void setup()
{
  Serial.begin(115200);
  //wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
  udpDevice.begin(localUdpPort);
  //

  Serial.println("Initializing...");


}
void networkTask() {


  uint8_t data;
  uint8_t packetSize = udpDevice.parsePacket();
  if (packetSize) {
    data = udpDevice.read();
    if (data == '1') {
      Serial.println("Llego el mensaje coño");
      udpDevice.beginPacket(udpDevice.remoteIP(), udpDevice.remotePort());
      udpDevice.write(byte(1));
      udpDevice .endPacket();
    }
    // send back a reply, to the IP address and port we got the packet from
  }
}
void loop()
{
  networkTask();
}
