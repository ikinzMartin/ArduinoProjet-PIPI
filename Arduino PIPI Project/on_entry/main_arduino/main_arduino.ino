#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define pirPin 5  //Pin atatched to PIR pin
RF24 radio(7,8); //ce,csn
const byte addresses[][6] = {"00001","00010"};
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
  radio.openReadingPipe(0,addresses[0]);
  radio.setPALevel(RF24_PA_MIN);
  //radio.startListening();
  //radio.stopListening();

}

void loop() {
//  int pirVal = readPIR();
//  if (pirVal){
//    Serial.print("movement");
//    detectedMovement();
//    }
  
  if (!radio.write(&a, sizeof(a))){
    Serial.println("AAA");}
  Serial.println("BBB");
  radio.write(&b, sizeof(b));
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
  int res = 0;
  while (!res)
  {
    if (radio.write(&a, sizeof(a))){
      res = 1;
      Serial.println("Message sent");}
    delay(100);
    };
  Serial.print("movement alert sent");
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
  if (next==400){
    Serial.print("Please identify yourself.");}
  else{
    Serial.print("Movement is unimportant");}
    }
  
