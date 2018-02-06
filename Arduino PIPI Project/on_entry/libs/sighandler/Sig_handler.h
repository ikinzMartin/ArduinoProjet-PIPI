#ifndef SIG_HANDLER

#define SIG_HANDLER

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

class Sig_handler
{
  public:
  Sig_handler(int ce, int csn);
  void init_handler(int ind_writer, int ind_reader);
  bool is_available();
  int read_sig();
  void send_sig(int sig);
  void send_sig(int sig, int delay_time);

  private:
    const byte _adresses[2][6] = {"00001", "00010"};
    RF24 _radio;
};

#endif
