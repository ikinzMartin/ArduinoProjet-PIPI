#ifndef SIG_HANDLER

#define SIG_HANDLER

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "signals.h"

class Sig_handler
{
  public:
  Sig_handler(int ce, int csn);
  void init_handler(int ind_writer, int ind_reader);
  bool is_available();
  signal read_sig();
  void send_sig(signal sig);
  void debugging_mode();

  private:
    const byte _adresses[2][6] = {"00001", "00010"};
    RF24 _radio;
};

#endif
