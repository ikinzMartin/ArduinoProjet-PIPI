#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

#define pirPin 5  //Pin atatched to PIR pin
RF24 radio(8,9); //ce,csn
const byte addresses[][6] = {"0Node","1Node"};
int msg;
int a = 400;
int b = 402;
int current;

void setup() 
{
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  radio.begin();
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
//  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
//  int pirVal = readPIR();
//  if (pirVal){
//    Serial.print("movement");
//    detectedMovement();
//  }
  delay(1000);
  detectedMovement();
  while(true);
}

//Reads the value given by the Human Body Infrared sensor(PIR) and returns the read value
float readPIR()
{
  float pirValue = digitalRead(pirPin);//read the value of PIR
  return pirValue;
  }

void sendMessage(int msg)
{
  radio.stopListening();
  Serial.println("Message sent");
  while (!radio.write(&msg, sizeof(msg))) {}
  Serial.println("Acknowledged");
  radio.startListening();
}

int recieveMessage()
{
  while(!radio.available());
  int msg;
  radio.read(&msg, sizeof(msg));
  return msg;
  }
  
void detectedMovement()
{
  sendMessage(400);
  int next = recieveMessage();
  if (next==401){
    Serial.print("Please identify yourself.");}
  else{
    Serial.print("Movement is unimportant");}
    }
  
