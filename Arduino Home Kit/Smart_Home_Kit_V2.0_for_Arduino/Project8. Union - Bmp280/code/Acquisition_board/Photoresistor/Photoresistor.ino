#include <DeviceBitTcpClient.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <SPI.h>

DeviceBitTcpClient *client;
#define LW_USERKEY "2e80e2a876db478c810a4d8b8fe81505" // USEEKEY
#define LW_GATEWAY "01"                               // User gateway Numbers

#define photocellPin 0  // photocell attach to A0 on SUNFOUNDER MEGA2560

int postInterval = 10000;
unsigned long starttime;

void setup()
{
  Serial.begin(9600);
  client = new DeviceBitTcpClient(LW_USERKEY, LW_GATEWAY);
  starttime = millis();
}

void loop()
{
  if (client)
  {
    if ((millis() - starttime) > postInterval)
    {
      Serial.println("client is OK ");
      //illumination intensity
      int lightValue = analogRead(photocellPin);
      Serial.println(lightValue);
      client -> sendSensorValue("Light", lightValue);
      starttime = millis();
    }
  }
}
