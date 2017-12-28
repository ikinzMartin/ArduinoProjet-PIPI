const int ledPin = 13;
const int analogPin=A0;
const int digitalPin=7;
int Astate=0;
boolean  Dstate=0;
void setup() 
{
  pinMode (digitalPin,INPUT);
  pinMode(ledPin,OUTPUT);
  Serial.begin(9600);
} 
void loop()
{  
 Astate=analogRead(analogPin);//read the value of A0
 Serial.println(Astate);
 Dstate=digitalRead(digitalPin);//read the value of D0
  Serial.println(Dstate);
 if( Dstate==HIGH )
 {
  digitalWrite(ledPin,LOW);
  }
  if( Dstate==LOW)
  {
   digitalWrite(ledPin,HIGH);
  }
  delay(200);
}
