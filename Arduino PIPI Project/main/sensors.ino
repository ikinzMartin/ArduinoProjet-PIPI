//Libraries
#include <DHT.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_Sensor.h>

//Constants
#define MQ2PIN A1
#define DHTPIN 34
#define PHOTOPIN A0
DHT dht(DHTPIN, DHT11);
Adafruit_BMP085_Unified bmp;

//Variables
float hum; //Humidity
float temp; //Temperature
float hi; //Heat index
int light; //Light measurement
float pressure; //Preassure measurement
int gas; //Gas sensor measurement

void setupSensors() {
  dht.begin();//Initialize communication with DHT11
  pinMode(PHOTOPIN, INPUT);//Initialize input from photoresistor
  pinMode(MQ2PIN, INPUT);//Initialize input from gas sensor
  bmp.begin();//Initialise pressure sensor

}

void updateSensorValues()
{
  readHumiditySensor();
  readPhotoResistor();
  readPressure();
  readGasSensor();
}

void printData()
{
  Serial.print("Humidity: ");
  Serial.println(hum);
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  Serial.print("Heat Index: ");
  Serial.println(hi);
  Serial.print("Light: ");
  Serial.println(light);
  Serial.print("Pressure: ");
  Serial.println(pressure);
  Serial.print("Gas: ");
  Serial.println(gas);
  Serial.println();
}

void htmlEncodeSensorsData(char *res){
  sprintf(res,"Humidity: %s <br />Temp: %s Celsius<br />Heat index: %s <br />Light: %d<br />Pressure: %s <br />Gas: %d",String(hum).c_str(),String(temp).c_str(),String(hi).c_str(),light,String(pressure).c_str(),gas);
}
void readHumiditySensor()
{
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  hi = dht.computeHeatIndex(temp, hum, false);
}

void readPhotoResistor()
{
  light = analogRead(PHOTOPIN);
}

void readPressure()
{
  bmp.getPressure(& pressure);
}

void readGasSensor()
{
  gas = analogRead(MQ2PIN);
}

