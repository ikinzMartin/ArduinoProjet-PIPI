/****************************************
name:phototest
function:you can see the resisistance change print in serial monitor
******************************************/
//Email:support@sunfounder.com
//website:www.sunfounder.com
#define photoresistorPin A0  // photoresistor attach to A0 on SUNFOUNDER MEGA2560

int lightPin = 4;

void setup()
{
    Serial.begin(9600);//
}

void loop()
{
  int value=analogRead(photoresistorPin);//read the value of photoresistor
  Serial.print("Photoresistor Value: ");
  Serial.println(value);
  if(value>300){
    setColor(255);
  } else if(value>150){
    setColor(125);
  } else {
    setColor(50);
  }
}

void setColor(int intensity)
{
  analogWrite(lightPin, intensity); 
}
