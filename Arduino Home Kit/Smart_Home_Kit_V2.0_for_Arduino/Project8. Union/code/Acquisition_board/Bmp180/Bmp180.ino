#include <SPI.h>
#include <Ethernet.h>
#include <DeviceBitTcpClient.h>
#include <EEPROM.h>
#include <SFE_BMP180.h>
#include <Wire.h>

DeviceBitTcpClient *client;
#define LW_USERKEY "2e80e2a876db478c810a4d8b8fe81505" // USEEKEY
#define LW_GATEWAY "01"

int postInterval = 10000;
unsigned long starttime;
SFE_BMP180 pressure;

void setup()
{
  Serial.begin(9600); //initialize the serial monitor
  client = new DeviceBitTcpClient(LW_USERKEY, LW_GATEWAY);
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.
    Serial.println("BMP180 init fail\n\n");
    while (1); // Pause forever.
  }
  starttime = millis();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (client)
  {
    if ((millis() - starttime) > postInterval)
    {
      char status;
      double T, P, p0, a;
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.
      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.
        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          // Print out the measurement:
          client -> sendSensorValue("Pressure", float (P/100));
          Serial.print("pressure: ");
          Serial.print(P/100, 2);
          Serial.println(" mb ");
        }
      }
      starttime = millis();
    }
  }
}




