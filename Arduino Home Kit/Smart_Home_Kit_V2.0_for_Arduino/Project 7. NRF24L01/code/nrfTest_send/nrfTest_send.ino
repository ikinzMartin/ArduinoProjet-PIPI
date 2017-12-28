/*********************************************************************
**  NRF24L01-------------Mega2560                                   **
**   GND-----------------GND                                        **
**   VCC-----------------3.3V                                       **
**   CE------------------pin 22                                      **
**   CSN-----------------pin 24  (SS pin)                            **
**   SCK-----------------pin 26 (SCK pin)                           **
**   MOSI----------------pin 28 (MOSI pin)                          **
**   MISO----------------pin 30 (MISO pin)                          **
**   IRQ-----------------pin 32 (IRQ pin)                           **
*********************************************************************/
#include "NRF24L01.h"

// types definition
#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long

//pins configuration
#define sw1Pin 2
#define sw2Pin 3
#define sw3Pin 4
#define sw4Pin 5
#define sw5Pin 6

#define TX_ADR_WIDTH    5   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  3  // 32 unsigned chars TX payload

unsigned char TX_ADDRESS[TX_ADR_WIDTH]  = 
{
  0x34,0x43,0x10,0x10,0x01
}; // Define a static TX address

unsigned char rx_buf[TX_PLOAD_WIDTH] = {0}; // initialize value
uint8 tx_buf[TX_PLOAD_WIDTH] = {0};

void setup()
{
    Serial.begin(9600);
    pinMode(sw1Pin, INPUT);
    pinMode(sw2Pin, INPUT);
    pinMode(sw3Pin, INPUT);
    pinMode(sw4Pin, INPUT);
    pinMode(sw5Pin, INPUT);
    digitalWrite(sw1Pin, HIGH);
    digitalWrite(sw2Pin, HIGH);
    digitalWrite(sw3Pin, HIGH);
    digitalWrite(sw4Pin, HIGH);
    digitalWrite(sw5Pin, HIGH);
    
    pinMode(CE,  OUTPUT);
    pinMode(SCK_PIN, OUTPUT);
    pinMode(CSN, OUTPUT);
    pinMode(MOSI_PIN,  OUTPUT);
    pinMode(MISO_PIN, INPUT);
    pinMode(IRQ, INPUT);
    init_io();                        // Initialize IO port
    unsigned char status=SPI_Read(STATUS);
    Serial.print("status = ");    
    Serial.println(status,HEX);     // There is read the mode’s status register, the default value should be ‘E’
    Serial.println("*******************TX_Mode Start****************************");
    TX_Mode();
}

void loop()
{
    if(digitalRead(sw1Pin) == 0)
    {
        Serial.println(1);
        tx_buf[0] = 0x55;
        tx_buf[1] = 0xAA;
        tx_buf[2] = 0x11;
        unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
        if(status&TX_DS)                                           // if receive data ready (TX_DS) interrupt
        {
          SPI_RW_Reg(FLUSH_TX,0);                                  
          SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);       // write playload to TX_FIFO
        }
        if(status&MAX_RT)                                         // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR                          
        {
          SPI_RW_Reg(FLUSH_TX,0);
          SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);      // disable standy-mode
        }
        SPI_RW_Reg(WRITE_REG+STATUS,status);                     // clear RX_DR or TX_DS or MAX_RT interrupt flag
    }
    if(digitalRead(sw2Pin) == 0)
    {
        Serial.println(2);
        tx_buf[0] = 0x55;
        tx_buf[1] = 0xAA;
        tx_buf[2] = 0x21;
        unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
        if(status&TX_DS)                                           // if receive data ready (TX_DS) interrupt
        {
          SPI_RW_Reg(FLUSH_TX,0);                                  
          SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);       // write playload to TX_FIFO
        }
        if(status&MAX_RT)                                         // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR                          
        {
          SPI_RW_Reg(FLUSH_TX,0);
          SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);      // disable standy-mode
        }
        SPI_RW_Reg(WRITE_REG+STATUS,status);                     // clear RX_DR or TX_DS or MAX_RT interrupt flag
    }
    if(digitalRead(sw3Pin) == 0)
    {
        Serial.println(3);
        tx_buf[0] = 0x55;
        tx_buf[1] = 0xAA;
        tx_buf[2] = 0x31;
        unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
        if(status&TX_DS)                                           // if receive data ready (TX_DS) interrupt
        {
          SPI_RW_Reg(FLUSH_TX,0);                                  
          SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);       // write playload to TX_FIFO
        }
        if(status&MAX_RT)                                         // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR                          
        {
          SPI_RW_Reg(FLUSH_TX,0);
          SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);      // disable standy-mode
        }
        SPI_RW_Reg(WRITE_REG+STATUS,status);                     // clear RX_DR or TX_DS or MAX_RT interrupt flag
    }
    if(digitalRead(sw4Pin) == 0)
    {
        Serial.println(4);
        tx_buf[0] = 0x55;
        tx_buf[1] = 0xAA;
        tx_buf[2] = 0x41;
        unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
        if(status&TX_DS)                                           // if receive data ready (TX_DS) interrupt
        {
          SPI_RW_Reg(FLUSH_TX,0);                                  
          SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);       // write playload to TX_FIFO
        }
        if(status&MAX_RT)                                         // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR                          
        {
          SPI_RW_Reg(FLUSH_TX,0);
          SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);      // disable standy-mode
        }
        SPI_RW_Reg(WRITE_REG+STATUS,status);                     // clear RX_DR or TX_DS or MAX_RT interrupt flag
    }
    if(digitalRead(sw5Pin) == 0)
    {
        tx_buf[0] = 0x55;
        tx_buf[1] = 0xAA;
        tx_buf[2] = 0x50;
        unsigned char status = SPI_Read(STATUS);                   // read register STATUS's value
        if(status&TX_DS)                                           // if receive data ready (TX_DS) interrupt
        {
            SPI_RW_Reg(FLUSH_TX,0);                                  
            SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);       // write playload to TX_FIFO
        }
        if(status&MAX_RT)                                         // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR                          
        {
            SPI_RW_Reg(FLUSH_TX,0);
            SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);      // disable standy-mode
        }
        SPI_RW_Reg(WRITE_REG+STATUS,status);                     // clear RX_DR or TX_DS or MAX_RT interrupt flag
    }
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
 * Function: SPI_RW();
 * 
 * Description:
 * Writes one unsigned char to nRF24L01, and return the unsigned char read
 * from nRF24L01 during write, according to SPI protocol
 **************************************************/
unsigned char SPI_RW(unsigned char Byte)
{
  unsigned char i;
  for(i=0;i<8;i++)                      // output 8-bit
  {
    if(Byte&0x80)
    {
      digitalWrite(MOSI_PIN, 1);
    }
    else
    {
      digitalWrite(MOSI_PIN, 0);
    }
    digitalWrite(SCK_PIN, 1);
    Byte <<= 1;                         // shift next bit into MSB..
    if(digitalRead(MISO_PIN) == 1)
    {
      Byte |= 1;       	                // capture current MISO bit
    }
    digitalWrite(SCK_PIN, 0);
  }
  return(Byte);           	        // return read unsigned char
}
/**************************************************/

/**************************************************
 * Function: SPI_RW_Reg();
 * 
 * Description:
 * Writes value 'value' to register 'reg'
/**************************************************/
unsigned char SPI_RW_Reg(unsigned char reg, unsigned char value)
{
  unsigned char status;

  digitalWrite(CSN, 0);                   // CSN low, init SPI transaction
  status = SPI_RW(reg);                   // select register
  SPI_RW(value);                          // ..and write value to it..
  digitalWrite(CSN, 1);                   // CSN high again

  return(status);                   // return nRF24L01 status unsigned char
}
/**************************************************/

/**************************************************
 * Function: SPI_Read();
 * 
 * Description:
 * Read one unsigned char from nRF24L01 register, 'reg'
/**************************************************/
unsigned char SPI_Read(unsigned char reg)
{
  unsigned char reg_val;

  digitalWrite(CSN, 0);           // CSN low, initialize SPI communication...
  SPI_RW(reg);                   // Select register to read from..
  reg_val = SPI_RW(0);           // ..then read register value
  digitalWrite(CSN, 1);          // CSN high, terminate SPI communication
  
  return(reg_val);               // return register value
}
/**************************************************/

/**************************************************
 * Function: SPI_Read_Buf();
 * 
 * Description:
 * Reads 'unsigned chars' #of unsigned chars from register 'reg'
 * Typically used to read RX payload, Rx/Tx address
/**************************************************/
unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
  unsigned char status,i;

  digitalWrite(CSN, 0);                  // Set CSN low, init SPI tranaction
  status = SPI_RW(reg);       	    // Select register to write to and read status unsigned char

  for(i=0;i<bytes;i++)
  {
    pBuf[i] = SPI_RW(0);    // Perform SPI_RW to read unsigned char from nRF24L01
  }

  digitalWrite(CSN, 1);                   // Set CSN high again

  return(status);                  // return nRF24L01 status unsigned char
}
/**************************************************/

/**************************************************
 * Function: SPI_Write_Buf();
 * 
 * Description:
 * Writes contents of buffer '*pBuf' to nRF24L01
 * Typically used to write TX payload, Rx/Tx address
/**************************************************/
unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
  unsigned char status,i;

  digitalWrite(CSN, 0);                  // Set CSN low, init SPI tranaction
  status = SPI_RW(reg);             // Select register to write to and read status unsigned char
  for(i=0;i<bytes; i++)             // then write all unsigned char in buffer(*pBuf)
  {
    SPI_RW(*pBuf++);
  }
  digitalWrite(CSN, 1);                   // Set CSN high again
  return(status);                  // return nRF24L01 status unsigned char
}
/**************************************************/

/**************************************************
 * Function: TX_Mode();
 * 
 * Description:
 * This function initializes one nRF24L01 device to
 * TX mode, set TX address, set RX address for auto.ack,
 * fill TX payload, select RF channel, datarate & TX pwr.
 * PWR_UP is set, CRC(2 unsigned chars) is enabled, & PRIM:TX.
 * 
 * ToDo: One high pulse(>10us) on CE will now send this
 * packet and expext an acknowledgment from the RX device.
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
  SPI_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);

  digitalWrite(CE, 1);
}
