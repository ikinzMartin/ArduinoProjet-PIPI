#define pirPin 11  //PIR attach to
#define ledPin 36  //LED attach to
#define piezoPin 7  //Piezo attach to

#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long

uint8 pirValue = 0;  //store the PIR state

void setup() 
{
    Serial.begin(9600);
    pinMode(pirPin, INPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(piezoPin, OUTPUT);
}

void loop()
{
    pirValue = digitalRead(pirPin);//read the value of PIR
    Serial.println("Pir State: ");
    Serial.println(pirValue);
    digitalWrite(ledPin, pirValue);
    if (pirValue==1){
      analogWrite(piezoPin, 300);
    } 
    else {
      analogWrite(piezoPin, 0);
    }
    ;
    delay(500);
}
