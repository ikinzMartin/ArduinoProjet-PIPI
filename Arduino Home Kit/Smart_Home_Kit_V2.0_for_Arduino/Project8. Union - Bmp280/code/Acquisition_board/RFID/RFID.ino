#include <DeviceBitTcpClient.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <SPI.h>
#include "rfid1.h"

//Common data types macro definition
#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long

DeviceBitTcpClient *client;
#define LW_USERKEY "2e80e2a876db478c810a4d8b8fe81505" // USEEKEY
#define LW_GATEWAY "01"

uchar serNum[5];
RFID1 rfid;
uint8 rfidValue = 0;
#define rfidLED 38  // RFID  signal LED attach to PIN38 on SunFounder Mega2560

int postInterval = 10000;
int rfidDelay = 500;
unsigned long starttime;
unsigned long starttime1;
volatile uint8 count1 = 0;

void setup()
{
  Serial.begin(9600);
  client = new DeviceBitTcpClient(LW_USERKEY, LW_GATEWAY);
  starttime = millis();
  starttime1 = millis();
  rfid.begin(7, 5, 4, 3, 6, 2);////begin(uchar irqPin, uchar sckPin, uchar mosiPin, uchar misoPin, uchar ssnPin, uchar rst)
  delay(100);
  rfid.init();
  pinMode(rfidLED, OUTPUT);
}

void loop()
{
  if (client)
  {
    rfidtime();
    if ((millis() - starttime) > postInterval)
    {
      //RFID
      client->sendSensorValue("RFID", rfidValue);
      Serial.println (rfidValue);
      starttime = millis();
    }
  }
}
void RFID(void)
{
  for (int i = 0; i <= 10; i++)
  {
    count1 ++;
    if (count1 == 5)
    {
      count1 = 0;
      if (readRfid())
      {
        rfidValue ++;
        digitalWrite(rfidLED, HIGH);
      }
       
    }
  }
  digitalWrite(rfidLED, LOW);
}

uint8 readRfid(void)
{
  uint8 status;
  uint8 str[MAX_LEN];
  status = rfid.request(PICC_REQIDL, str);
  if (status != MI_OK)
  {
    return 0;
  }
  status = rfid.anticoll(str);

  if (status == MI_OK)
  {
    memcpy(serNum, str, 5);

    // Check people associated with card ID
    uchar* id = serNum;

    if (id[0] == 0x04 && id[1] == 0x4B && id[2] == 0xBC && id[3] == 0xA7)
    {
      return 1;
      
    }
  }
  return 0;
  rfid.halt(); //command the card into sleep mode
}
void rfidtime()
{
  if ((millis() - starttime1) > rfidDelay)
  {
    RFID();
    starttime1 = millis();
  }
}


