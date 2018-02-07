#include <Sig_handler.h>
#include <signals.h>

#include "main_mega.h"

Sig_handler handler(28, 30);

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

  // setup for the handler
  handler.init_handler(1, 0);
}

void loop() {
  delay(5);
  if (handler.is_available()) {
    delay(5);

    switch(handler.read_sig()) {
      case MOTION_CAPTURED_SIGNAL:
        Serial.println("j'ai capté un mouvement");
        handler.send_sig(DO_IDENTIFYING_SIGNAL);
        break;
      case DO_IDENTIFYING_SIGNAL:
        Serial.println("Authentifie le mouvement");
        break;
      case MOTION_IDENTIFIED_SIGNAL:
        Serial.println("Mouvement authentifié");
        relay_on(RELAY1);
        break;
      default:
        handler.send_sig(NULL_SIGNAL);
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
