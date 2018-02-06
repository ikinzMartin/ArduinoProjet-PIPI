#include "main_mega.h"

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(28, 30);

const byte address[2][6] = {"00001", "00010"};

void setup() {
  Serial.begin(9600);

  // setup for relay
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  
  relay_off(RELAY1);
  relay_off(RELAY2);
  relay_off(RELAY3);
  relay_off(RELAY4);

  // setup for NRF
  radio.begin();
  //radio.openWritingPipe(address[1]);
  radio.openReadingPipe(1, address[0]);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  delay(5);
  //send_sig(400);
    Serial.println("ok2");
  if (radio.available()) {
    int sig = 399;
    radio.read(&sig, sizeof(sig));
    Serial.println("ok");
    delay(5);

    switch(sig) {
      case MVT_CAPT:
        Serial.println("j'ai capté un mouvement");
        send_sig(DO_IDENTIFY);
        break;
      case DO_IDENTIFY:
        Serial.println("Authentifie le mouvement");
        break;
      case MVT_IDENTIFYED:
        Serial.println("Mouvement authentifié");
        relay_on(RELAY1);
        break;
      default:;
    }
  }
}

void send_sig(int sig) {
  //radio.stopListening();
  Serial.println("ok");
  //while(!radio.write(&sig, sizeof(sig))) delay(70);
  //radio.startListening();
}

void relay_on(int relay) {
  digitalWrite(relay, LOW);
}

void relay_off(int relay) {
  digitalWrite(relay, HIGH);
}

