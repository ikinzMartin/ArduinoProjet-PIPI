#include <DeviceBitTcpClient.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <SPI.h>

DeviceBitTcpClient *client;
#define LW_USERKEY "2e80e2a876db478c810a4d8b8fe81505" // USEEKEY
#define LW_GATEWAY "01"                               // User gateway Numbers

#define mq2Pin A1   // mq2attach to A1 on SunFounder Mega2560
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
      int gasValue = analogRead(mq2Pin);
      Serial.println(gasValue);
      client -> sendSensorValue("Gas", gasValue);
      starttime = millis();
    }
  }
}
