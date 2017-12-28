/*********************************************************************
**   SPI-compatible                                                 **
**   CS - to digital pin 22                                          **
**   CSN - to digital pin 24  (SS pin)                               **
**   SCK - to digital pin 26 (SCK pin)                              **
**   MOSI - to digital pin 28 (MOSI pin)                            **
**   MISO - to digital pin 30 (MISO pin)                            **
**   IRQ - to digital pin 32 (MISO pin)                             **
*********************************************************************/
#include <SPI.h>
#include <Ethernet.h>
#include <DeviceBitTcpClient.h>
#include <EEPROM.h>
#include <dht11.h>
#include "NRF24L01.h"
#include "rfid1.h"
#include <Wire.h>
#include <SFE_BMP180.h>
#include <avr/interrupt.h>

//Common data types macro definition
#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long

//Library configuration
DHT11 DHT11;//Creates a variable of type DHT11,named DHT11
RFID1 rfid; //Creates a variable of type RFID1,named rfid
uchar serNum[5];
SFE_BMP180 pressure;


#define TX_ADR_WIDTH    5   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  3  // 32 unsigned chars TX payload

unsigned char TX_ADDRESS[TX_ADR_WIDTH]  =
{
  0x34, 0x43, 0x10, 0x10, 0x01
}; // Define a static TX address

unsigned char rx_buf[TX_PLOAD_WIDTH] = {0}; // initialize value
uint8 tx_buf[TX_PLOAD_WIDTH] = {0};

DeviceBitTcpClient *client;
#define LW_USERKEY "00a7fd657fd944129760bcb2b418b1ce" // USEEKEY
#define LW_GATEWAY "01"

int postInterval = 10000;//the delay time between the data upload to the devicebit

unsigned long duration;
unsigned long starttime;//store the time the timer
unsigned long starttime1;

volatile uint8 count1, count2 = 0; //store temporary data

uint8 pirValue = 0;
boolean pirState = false;
uint8 rfidValue = 0;
boolean rfidState = false;
uint8  pirCount = 0;
double T, P;

// pins distribution
#define photocellPin A0 //photoresistor connect to A0
#define mq2Pin A1 //mq2 connect to A1
#define light1Pin 8  //Set the control operation of I/o port
#define light2Pin 9
#define pirPin 36 //pir connect to pin36
#define DHT11PIN 34 //dht11 connect to pin34
#define rfidLED 38 //rfid signal led connnect to pin38
#define pirLED 40  //pir signal led connect to pin40

void setup()
{
  Serial.begin(9600);//set the baudrate of serial monitor  is 9600
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.
    Serial.println("BMP180 init fail\n\n");
  }
  rfid.begin(7, 5, 4, 3, 6, 2);//begin(uchar irqPin, uchar sckPin, uchar mosiPin, uchar misoPin, uchar ssnPin, uchar rst)
  delay(100);
  rfid.init();
  pinMode(pirPin, INPUT);
  pinMode(rfidLED, OUTPUT);
  pinMode(pirLED, OUTPUT);

  pinMode(CE,  OUTPUT);
  pinMode(SCK_PIN, OUTPUT);
  pinMode(CSN, OUTPUT);
  pinMode(MOSI_PIN,  OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(IRQ, INPUT);
  init_io();                        // Initialize IO port
  unsigned char status = SPI_Read(STATUS);
  Serial.print("status = ");
  Serial.println(status, HEX);    // There is read the mode’s status register, the default value should be ‘E’
  Serial.println("*******************TX_Mode Start****************************");

  TX_Mode();
  //TIMER1_init();
  client = new DeviceBitTcpClient(LW_USERKEY, LW_GATEWAY);
  //client = new LeweiTcpClient(LW_USERKEY, LW_GATEWAY,mac,ip,mydns,gw,subnet);
  UserSwitch us1(switchLight1, "light1", 1);
  client->addUserSwitch(us1);
  UserSwitch us2(switchLight2, "light2", 1);
  client->addUserSwitch(us2);
  UserSwitch us3(switchLight3, "light3", 1);
  client->addUserSwitch(us3);
  UserSwitch us4(switchLight4, "light4", 1);
  client->addUserSwitch(us4);
  starttime = millis();
  starttime1 = millis();
  pinMode(light1Pin, OUTPUT);
  pinMode(light2Pin, OUTPUT);
}


void loop()
{
  if (client)
  {
    if ((millis() - starttime) > postInterval)
    {
      //humiture
      dhtPreprocess();
      client -> appendSensorValue("Humidity", DHT11.humidity);
      Serial.print("Humidity: ");
      Serial.println(DHT11.humidity);
      client -> appendSensorValue("Temperature", DHT11.temperature);
      Serial.print("Temperature: ");
      Serial.println(DHT11.temperature);
      //illumination intensity
      client -> appendSensorValue("Light", analogRead(photocellPin));
      Serial.print("Light: ");
      Serial.println(analogRead(photocellPin));
      //gas sensor
      client -> appendSensorValue("Gas", analogRead(mq2Pin));
      Serial.print("Gas: ");
      Serial.println(analogRead(mq2Pin));
      //bmp180
      char status;
      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);
        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          // Print out the measurement:
          client -> appendSensorValue("Pressure", float (P / 100));
          Serial.print("pressure: ");
          Serial.print(P, 2);
          Serial.println(" mb ");
        }
      }
      //PIR
      pirtime();
      client->appendSensorValue("PIR", pirValue);
      Serial.print("PIR: ");
      Serial.println(pirValue);
      //RFID
      client->sendSensorValue("RFID", rfidValue);
      Serial.print("RFID: ");
      Serial.println(rfidValue);
      Serial.println("repeat lwc->send  ...Completed.");
      starttime = millis();
    }
  }
}

void stateChange1()
{
  Serial.println(digitalRead(light1Pin));
  if (digitalRead(light1Pin) == HIGH)
  {
    switchLight1("0");
  }
  else
  {
    switchLight1("1");
  }
}
void stateChange2()
{
  if (digitalRead(light2Pin) == HIGH)
  {
    switchLight2("0");
  }
  else
  {
    switchLight2("1");
  }
}

void switchLight1(char* p1)
{
  Serial.println("switch1 test");
  //stateChange1();
  if (String(p1).equals("0"))
  {
    digitalWrite(light1Pin, HIGH);
    tx_buf[0] = 0x55;
    tx_buf[1] = 0xAA;
    tx_buf[2] = 0x11;
    unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
    if (status & TX_DS)                                        // if receive data ready (TX_DS) interrupt
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     // write playload to TX_FIFO
    }
    if (status & MAX_RT)                                      // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);    // disable standy-mode
    }
    SPI_RW_Reg(WRITE_REG + STATUS, status);                  // clear RX_DR or TX_DS or MAX_RT interrupt flag
    //delay(1000);
  }
  else
  {
    digitalWrite(light1Pin, LOW);
    tx_buf[0] = 0x55;
    tx_buf[1] = 0xAA;
    tx_buf[2] = 0x10;
    unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
    if (status & TX_DS)                                        // if receive data ready (TX_DS) interrupt
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     // write playload to TX_FIFO
    }
    if (status & MAX_RT)                                      // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);    // disable standy-mode
    }
    SPI_RW_Reg(WRITE_REG + STATUS, status);                  // clear RX_DR or TX_DS or MAX_RT interrupt flag
    //delay(1000);
  }
  client->checkFreeMem();
}

void switchLight2(char* p1)
{
  Serial.println("switch2 test");
  //  stateChange2();
  if (String(p1).equals("0"))
  {
    digitalWrite(light2Pin, HIGH);
    tx_buf[0] = 0x55;
    tx_buf[1] = 0xAA;
    tx_buf[2] = 0x21;
    unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
    if (status & TX_DS)                                        // if receive data ready (TX_DS) interrupt
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     // write playload to TX_FIFO
    }
    if (status & MAX_RT)                                      // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);    // disable standy-mode
    }
    SPI_RW_Reg(WRITE_REG + STATUS, status);                  // clear RX_DR or TX_DS or MAX_RT interrupt flag
    //delay(1000);
  }
  else
  {
    digitalWrite(light2Pin, LOW);
    tx_buf[0] = 0x55;
    tx_buf[1] = 0xAA;
    tx_buf[2] = 0x20;
    unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
    if (status & TX_DS)                                        // if receive data ready (TX_DS) interrupt
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     // write playload to TX_FIFO
    }
    if (status & MAX_RT)                                      // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);    // disable standy-mode
    }
    SPI_RW_Reg(WRITE_REG + STATUS, status);                  // clear RX_DR or TX_DS or MAX_RT interrupt flag
    //delay(1000);
  }
  client->checkFreeMem();
}

void switchLight3(char* p1)
{
  Serial.println("switch1 test");
  //  stateChange1();
  if (String(p1).equals("0"))
  {
    //digitalWrite(light3Pin,HIGH);
    tx_buf[0] = 0x55;
    tx_buf[1] = 0xAA;
    tx_buf[2] = 0x31;
    unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
    if (status & TX_DS)                                        // if receive data ready (TX_DS) interrupt
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     // write playload to TX_FIFO
    }
    if (status & MAX_RT)                                      // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);    // disable standy-mode
    }
    SPI_RW_Reg(WRITE_REG + STATUS, status);                  // clear RX_DR or TX_DS or MAX_RT interrupt flag
  }
  else
  {
    //digitalWrite(light3Pin,LOW);
    tx_buf[0] = 0x55;
    tx_buf[1] = 0xAA;
    tx_buf[2] = 0x30;
    unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
    if (status & TX_DS)                                        // if receive data ready (TX_DS) interrupt
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     // write playload to TX_FIFO
    }
    if (status & MAX_RT)                                      // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);    // disable standy-mode
    }
    SPI_RW_Reg(WRITE_REG + STATUS, status);                  // clear RX_DR or TX_DS or MAX_RT interrupt flag
  }
  client->checkFreeMem();
}

void switchLight4(char* p1)
{
  Serial.println("switch2 test");
  //  stateChange2();
  if (String(p1).equals("0"))
  {
    //digitalWrite(light4Pin,HIGH);
    tx_buf[0] = 0x55;
    tx_buf[1] = 0xAA;
    tx_buf[2] = 0x41;
    unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
    if (status & TX_DS)                                        // if receive data ready (TX_DS) interrupt
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     // write playload to TX_FIFO
    }
    if (status & MAX_RT)                                      // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);    // disable standy-mode
    }
    SPI_RW_Reg(WRITE_REG + STATUS, status);                  // clear RX_DR or TX_DS or MAX_RT interrupt flag
  }
  else
  {
    //digitalWrite(light4Pin,LOW);
    tx_buf[0] = 0x55;
    tx_buf[1] = 0xAA;
    tx_buf[2] = 0x40;
    unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
    if (status & TX_DS)                                        // if receive data ready (TX_DS) interrupt
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     // write playload to TX_FIFO
    }
    if (status & MAX_RT)                                      // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR
    {
      SPI_RW_Reg(FLUSH_TX, 0);
      SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);    // disable standy-mode
    }
    SPI_RW_Reg(WRITE_REG + STATUS, status);                  // clear RX_DR or TX_DS or MAX_RT interrupt flag
  }
  client->checkFreeMem();
}

//**************************************************
// Function: init_io();
// Description:
// flash led one time,chip enable(ready to TX or RX Mode),
// Spi disable,Spi clock line init high
//**************************************************
void init_io(void)
{
  digitalWrite(IRQ, 0);
  digitalWrite(CE, 0);			// chip enable
  digitalWrite(CSN, 1);                 // Spi disable
}

/**************************************************
   Function: SPI_RW();

   Description:
   Writes one unsigned char to nRF24L01, and return the unsigned char read
   from nRF24L01 during write, according to SPI protocol
 **************************************************/
unsigned char SPI_RW(unsigned char Byte)
{
  unsigned char i;
  for (i = 0; i < 8; i++)               // output 8-bit
  {
    if (Byte & 0x80)
    {
      digitalWrite(MOSI_PIN, 1);
    }
    else
    {
      digitalWrite(MOSI_PIN, 0);
    }
    digitalWrite(SCK_PIN, 1);
    Byte <<= 1;                         // shift next bit into MSB..
    if (digitalRead(MISO_PIN) == 1)
    {
      Byte |= 1;       	                // capture current MISO bit
    }
    digitalWrite(SCK_PIN, 0);
  }
  return (Byte);           	       // return read unsigned char
}
/**************************************************/

/**************************************************
   Function: SPI_RW_Reg();

   Description:
   Writes value 'value' to register 'reg'
  /**************************************************/
unsigned char SPI_RW_Reg(unsigned char reg, unsigned char value)
{
  unsigned char status;

  digitalWrite(CSN, 0);                   // CSN low, init SPI transaction
  status = SPI_RW(reg);                   // select register
  SPI_RW(value);                          // ..and write value to it..
  digitalWrite(CSN, 1);                   // CSN high again

  return (status);                  // return nRF24L01 status unsigned char
}
/**************************************************/

/**************************************************
   Function: SPI_Read();

   Description:
   Read one unsigned char from nRF24L01 register, 'reg'
  /**************************************************/
unsigned char SPI_Read(unsigned char reg)
{
  unsigned char reg_val;

  digitalWrite(CSN, 0);           // CSN low, initialize SPI communication...
  SPI_RW(reg);                   // Select register to read from..
  reg_val = SPI_RW(0);           // ..then read register value
  digitalWrite(CSN, 1);          // CSN high, terminate SPI communication

  return (reg_val);              // return register value
}
/**************************************************/

/**************************************************
   Function: SPI_Read_Buf();

   Description:
   Reads 'unsigned chars' #of unsigned chars from register 'reg'
   Typically used to read RX payload, Rx/Tx address
  /**************************************************/
unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
  unsigned char status, i;

  digitalWrite(CSN, 0);                  // Set CSN low, init SPI tranaction
  status = SPI_RW(reg);       	    // Select register to write to and read status unsigned char

  for (i = 0; i < bytes; i++)
  {
    pBuf[i] = SPI_RW(0);    // Perform SPI_RW to read unsigned char from nRF24L01
  }

  digitalWrite(CSN, 1);                   // Set CSN high again

  return (status);                 // return nRF24L01 status unsigned char
}
/**************************************************/

/**************************************************
   Function: SPI_Write_Buf();

   Description:
   Writes contents of buffer '*pBuf' to nRF24L01
   Typically used to write TX payload, Rx/Tx address
  /**************************************************/
unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
  unsigned char status, i;

  digitalWrite(CSN, 0);                  // Set CSN low, init SPI tranaction
  status = SPI_RW(reg);             // Select register to write to and read status unsigned char
  for (i = 0; i < bytes; i++)       // then write all unsigned char in buffer(*pBuf)
  {
    SPI_RW(*pBuf++);
  }
  digitalWrite(CSN, 1);                   // Set CSN high again
  return (status);                 // return nRF24L01 status unsigned char
}
/**************************************************/

/**************************************************
   Function: TX_Mode();

   Description:
   This function initializes one nRF24L01 device to
   TX mode, set TX address, set RX address for auto.ack,
   fill TX payload, select RF channel, datarate & TX pwr.
   PWR_UP is set, CRC(2 unsigned chars) is enabled, & PRIM:TX.

   ToDo: One high pulse(>10us) on CE will now send this
   packet and expext an acknowledgment from the RX device.
 **************************************************/
void TX_Mode(void)
{
  digitalWrite(CE, 0);

  SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
  SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack

  SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
  SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...
  SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // Select RF channel 40
  SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);     // Set PWR_UP bit, enable CRC(2 unsigned chars) & Prim:TX. MAX_RT & TX_DS enabled..
  SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);

  digitalWrite(CE, 1);
}

//Temperature and humidity sensor preprocessor
void dhtPreprocess(void)
{
  int chk = DHT11.read(DHT11PIN);
  switch (chk)
  {
    case DHTLIB_OK:
      break;
    case DHTLIB_ERROR_CHECKSUM:
      break;
    case DHTLIB_ERROR_TIMEOUT:
      break;
    default:
      break;
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

    if (id[0] == 0x36 && id[1] == 0xAF && id[2] == 0xC4 && id[3] == 0x93)
    {
      return 1;

    }
  }
  return 0;
  rfid.halt(); //command the card into sleep mode
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
    RFID();
    starttime1 = millis();
  }
}



