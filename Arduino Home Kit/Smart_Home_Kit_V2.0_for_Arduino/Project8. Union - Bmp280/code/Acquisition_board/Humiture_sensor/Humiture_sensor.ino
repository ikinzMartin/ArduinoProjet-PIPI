#include <DeviceBitTcpClient.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <SPI.h>
#include <dht11.h>

DeviceBitTcpClient *client;
#define LW_USERKEY "2e80e2a876db478c810a4d8b8fe81505" // USEEKEY
#define LW_GATEWAY "01"                               // User gateway Numbers

#define DHT11PIN 34  // DHT11 attach to PIN34 on SunFounder Mega2560
DHT11 my_dht11;

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
      float temp_c;
      float humidity;
      if (my_dht11.read(DHT11PIN) == 0)
      { // DHTLIB_OK
        Serial.println("read data OK");
        temp_c = my_dht11.temperature;
        humidity = my_dht11.humidity;
        Serial.print("Temperature "); Serial.print(temp_c);
        Serial.print("Humidity "); Serial.println(humidity);
      }
      client -> appendSensorValue("Temperature", temp_c);
      client -> sendSensorValue("Humidity", humidity);
      starttime = millis();
    }
  }
}
