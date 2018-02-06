void setup() {
  Serial.begin(9600);
  setupSensors();
  setupShield();
}

void loop() {
  updateSensorValues();
  printData();
  listenForEthernatClients();
  delay(1000);
}
