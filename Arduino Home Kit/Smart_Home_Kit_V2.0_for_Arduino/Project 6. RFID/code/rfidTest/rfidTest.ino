#include "rfid1.h"

RFID1 rfid;

uchar serNum[5];

void setup()
{
  Serial.begin(9600);
  rfid.begin(7, 5, 4, 3, 6, 2);
  delay(100);
  rfid.init();
}
void loop()
{
  uchar status;
  uchar str[MAX_LEN];
  status = rfid.request(PICC_REQIDL, str);
  if (status != MI_OK)
  {
    return;
  }
  
  rfid.showCardType(str);
  status = rfid.anticoll(str);
  
  if (status == MI_OK)
  {
    Serial.print("The card's number is: ");
    memcpy(serNum, str, 5);
    rfid.showCardID(serNum);
    Serial.println();
    
    // Check people associated with card ID
    uchar* id = serNum;
     if (id[0] == 0x5B && id[1] == 0x80 && id[2] == 0xDA && id[3] == 0x0B)
     {
      Serial.println("Hello Martin!");
      Serial.println();
    } 
    else if(id[0]==0xEB && id[1]==0xCD && id[2]==0xD6 && id[3]==0x0B) 
    {
      Serial.println("Hello Iva!");
      Serial.println();
    }
    else if(id[0] == 0x36 && id[1] == 0xAF && id[2] == 0xC4 && id[3] == 0x93)
    {
      Serial.println("Hello mister banana!");
      Serial.println();
    }
    else
    {
      Serial.println("Shoo beast");
      Serial.println();
    }
  }
  delay(500); 
  rfid.halt(); //command the card into sleep mode 
}
