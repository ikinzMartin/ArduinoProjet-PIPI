#include <DeviceBitTcpClient.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp280; //Define a variable bmp280 of Adafruit_BMP280 type. Subsequently bmp280 represents Adafruit_BMP280

DeviceBitTcpClient *client;
#define LW_USERKEY "2e80e2a876db478c810a4d8b8fe81505" // USEEKEY
#define LW_GATEWAY "01"
// User gateway Numbers

#define DELAYTIME 1000
int postInterval = 10000;
unsigned long starttime;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("BMP280_I2C"));//print BMP280_I2C on serial monitor

  if (!bmp280.begin()) //if bmp280.begin()==0, it means bmp280 initialization fails.
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);//Infinite loop, no stop until the initialization succeeds.
  }
  client = new DeviceBitTcpClient(LW_USERKEY, LW_GATEWAY);
  starttime = millis();
}

void loop()
{
  if (client)
  {
    if ((millis() - starttime) > postInterval)
    {
      client -> sendSensorValue("Pressure", float (bmp280.readPressure()/100));
      Serial.print(bmp280.readPressure()/100,2);
      Serial.println(" mb");
      starttime = millis();
    }
  }
}

