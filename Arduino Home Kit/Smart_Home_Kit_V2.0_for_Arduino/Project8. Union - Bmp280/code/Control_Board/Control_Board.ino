/*********************************************************************
**   SPI-compatible                                                 **
**   CE - to digital pin 8                                          **
**   CSN - to digital pin 9  (SS pin)                               **
**   SCK - to digital pin 10 (SCK pin)                              **
**   MOSI - to digital pin 11 (MOSI pin)                            **
**   MISO - to digital pin 12 (MISO pin)                            **
**   IRQ - to digital pin 13 (IRQ pin)                             **
*********************************************************************/

#include "NRF24L01.h"

#define TX_ADR_WIDTH    5   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  3  // 32 unsigned chars TX payload

#define SW1_ON_Command     0x11   
#define SW1_OFF_Command    0x10   
#define SW2_ON_Command     0x21   
#define SW2_OFF_Command    0x20   
#define SW3_ON_Command     0x31   
#define SW3_OFF_Command    0x30   
#define SW4_ON_Command     0x41   
#define SW4_OFF_Command    0x40  
#define SWS_OFF_Command    0x50  

#define SW1 6// IN1 attach to pin6 of nano
#define SW2 5//IN2 attach to pin5 of nano
#define SW3 4//IN3 attach to pin4 of nano
#define SW4 3//IN4 attach to pin5 of nano

unsigned char TX_ADDRESS[TX_ADR_WIDTH]  = 
{
  0x34,0x43,0x10,0x10,0x01
}; // Define a static TX address

unsigned char rx_buf[TX_PLOAD_WIDTH];
unsigned char tx_buf[TX_PLOAD_WIDTH];

void setup()
{
    pinMode(CE,  OUTPUT);
    pinMode(SCK_PIN, OUTPUT);
    pinMode(CSN, OUTPUT);
    pinMode(MOSI_PIN,  OUTPUT);
    pinMode(MISO_PIN, INPUT);
    pinMode(IRQ, INPUT);
    Serial.begin(9600);
    init_io();                        // Initialize IO port
    unsigned char status=SPI_Read(STATUS);
    Serial.print("status = ");
    Serial.println(status,HEX);      // There is read the mode’s status register, the default value should be ‘E’  
    Serial.println("*****************RX_Mode start******************************R");
    RX_Mode();
    
    pinMode(SW1, OUTPUT); 
    pinMode(SW2, OUTPUT); 
    pinMode(SW3, OUTPUT); 
    pinMode(SW4, OUTPUT); 
    
    digitalWrite(SW1, HIGH);
    digitalWrite(SW2, HIGH);
    digitalWrite(SW3, HIGH);
    digitalWrite(SW4, HIGH);
}

void loop()
{
    unsigned char status = SPI_Read(STATUS);                         // read register STATUS's value
    if(status&RX_DR)
    {
        Serial.println(1);
        SPI_Read_Buf(RD_RX_PLOAD, rx_buf, TX_PLOAD_WIDTH);             // read playload to rx_buf
        SPI_RW_Reg(FLUSH_RX,0);       // clear RX_FIFO
        if(rx_buf[0] == 0x55)
        {
            if(rx_buf[1] == 0xAA)
            {
                switch(rx_buf[2])
                {
                    case SW1_ON_Command:  
                        digitalWrite(SW1, LOW);        
                        break;
                    case SW1_OFF_Command:  
                        digitalWrite(SW1, HIGH);        
                        break;
                    case SW2_ON_Command:  
                        digitalWrite(SW2, LOW);        
                        break;
                    case SW2_OFF_Command:
                        digitalWrite(SW2, HIGH);        
                        break;
                    case SW3_ON_Command:  
                        digitalWrite(SW3, LOW);        
                        break;
                    case SW3_OFF_Command:  
                        digitalWrite(SW3, HIGH);        
                        break;
                    case SW4_ON_Command:  
                        digitalWrite(SW4, LOW);        
                        break;
                    case SW4_OFF_Command:  
                        digitalWrite(SW4, HIGH);        
                        break;
                    case SWS_OFF_Command:
                        digitalWrite(SW1, HIGH);
                        digitalWrite(SW2, HIGH);
                        digitalWrite(SW3, HIGH);
                        digitalWrite(SW4, HIGH);
                        break;
                }
            }
            
        }
    }
    SPI_RW_Reg(WRITE_REG+STATUS,status);                             // clear RX_DR or TX_DS or MAX_RT interrupt flag
    for(int i = 0; i < 3; i++)
    {
        rx_buf[i] = 0;
    }
    delay(1000);
}

//**************************************************
// Function: init_io();
// Description:
// flash led one time,chip enable(ready to TX or RX Mode),
// Spi disable,Spi clock line init high
//**************************************************
void init_io(void)
{
  digitalWrite(IRQ, 0);  //interrupt signal。In the process of the wireless communication MCU mainly through IRQ with NRF24L01 communication
  digitalWrite(CE, 0);	 // chip enable .Mode control chip. Under the condition of the CSN to be low, CE synergy NRF24L0A CONFIG registers to NRF24L01 state
  digitalWrite(CSN, 1);  // Spi disable .Chip line selection, CSN, work for low level chip	
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
      digitalWrite(MOSI_PIN, 1);    // output 'unsigned char', MSB to MOSI
    }
    else
    {
      digitalWrite(MOSI_PIN, 0);
    }
    digitalWrite(SCK_PIN, 1);                      // Set SCK high..
    Byte <<= 1;                         // shift next bit into MSB..
    if(digitalRead(MISO_PIN) == 1)
    {
      Byte |= 1;       	                // capture current MISO bit
    }
    digitalWrite(SCK_PIN, 0);         	// ..then set SCK low again
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

  digitalWrite(CSN, 0);                   // Set CSN low, init SPI tranaction
  status = SPI_RW(reg);             // Select register to write to and read status unsigned char
  for(i=0;i<bytes; i++)             // then write all unsigned char in buffer(*pBuf)
  {
    SPI_RW(*pBuf++);
  }
  digitalWrite(CSN, 1);                  // Set CSN high again
  return(status);                  // return nRF24L01 status unsigned char
}
/**************************************************/

/**************************************************
 * Function: RX_Mode();
 * 
 * Description:
 * This function initializes one nRF24L01 device to
 * RX Mode, set RX address, writes RX payload width,
 * select RF channel, datarate & LNA HCURR.
 * After init, CE is toggled high, which means that
 * this device is now ready to receive a datapacket.
/**************************************************/
void RX_Mode(void)
{
  digitalWrite(CE, 0);
  SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // Use the same address on the RX device as the TX device
  SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
  SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // Select RF channel 40
  SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH); // Select same RX payload width as TX Payload width
  SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);     // Set PWR_UP bit, enable CRC(2 unsigned chars) & Prim:RX. RX_DR enabled..
  digitalWrite(CE, 1);                             // Set CE pin high to enable RX device
  //  This device is now ready to receive one packet of 16 unsigned chars payload from a TX device sending to address
  //  '3443101001', with auto acknowledgment, retransmit count of 10, RF channel 40 and datarate = 2Mbps.
}
/**************************************************/


