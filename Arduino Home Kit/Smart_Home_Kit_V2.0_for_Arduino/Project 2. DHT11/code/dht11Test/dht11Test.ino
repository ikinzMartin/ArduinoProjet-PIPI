#include <dht.h>


dht DHT11;

#define DHT11PIN 10  // DHT11 attach to digital pin 10 on controller board

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    // READ DATA
    Serial.print("DHT11,\t");
    //read the value returned from sensor
    int chk = DHT11.read11(DHT11PIN);
    switch (chk)
    {
      case DHTLIB_OK:  
  		Serial.print("OK \t"); 
  		break;
      case DHTLIB_ERROR_CHECKSUM: 
  		Serial.print("Checksum error,\t"); 
  		break;
      case DHTLIB_ERROR_TIMEOUT: 
  		Serial.print("Time out error,\t"); 
  		break;
      default: 
  		Serial.print("Unknown error,\t"); 
  		break;
    }
    Serial.println();
   // DISPLAY DATA
   Serial.print("Tem: ");
   Serial.print(DHT11.temperature); //print the temperature on serial monitor
   Serial.println(" C");
   Serial.print("Hum: ");
   Serial.print(DHT11.humidity); //print the humidity on serial monitor
   Serial.println(" %"); 
   Serial.println();
   delay(500); //wait a while 
}
