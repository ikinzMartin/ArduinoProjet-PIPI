/*********************************
   name:PIR sensor
   Desription:Read the PIR value every 2 seconds,when it's value is 1,then PIRCount increase 1,and you can see the LED turn on .
   upload the PIRCount to the devicebit platform every 10 seconds.
 **************************************************/
#include <DeviceBitTcpClient.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <SPI.h>

//Common data types macro definition
#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long

DeviceBitTcpClient *client;
#define LW_USERKEY "2e80e2a876db478c810a4d8b8fe81505" // USEEKEY
#define LW_GATEWAY "01"                               // User gateway Numbers

#define pirPin 36   // PIR attach to PIN36 on SunFounder Mega2560
#define pirLED 40   //PIR led attach to pin 40

int postInterval = 10000;
int pirDelay = 500;
unsigned long starttime;
unsigned long starttime1;

uint8  pirCount = 0;

void setup()
{
  Serial.begin(9600);
  client = new DeviceBitTcpClient(LW_USERKEY, LW_GATEWAY);
  starttime = millis();
  starttime1 = millis();
  pinMode(pirPin, INPUT);
  pinMode(pirLED, OUTPUT);
}

void loop()
{
  if (client)
  {
    pirtime();
    if ((millis() - starttime) > postInterval)
    {
      //PIR
      client->sendSensorValue("PIR", pirCount);
      Serial.print( "PIRCount: ");
      Serial.println( pirCount);
      starttime = millis();
    }
  }
}

void PIR(void)
{
  int pirValue = digitalRead(pirPin);
  if (pirValue == 1)
  {
    pirCount ++;
    digitalWrite(pirLED, HIGH);
    delay(50);
  }
  digitalWrite(pirLED, LOW);
}
void pirtime()
{
  {
    PIR();
    starttime1 = millis();
  }
}




