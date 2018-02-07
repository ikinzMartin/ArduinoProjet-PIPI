#include "Sig_handler.h"
#include "signals.h"

Sig_handler::Sig_handler(int ce, int csn) : _radio(ce, csn)
{
}

void Sig_handler::init_handler(int ind_writer, int ind_reader)
{
  _radio.begin();
  _radio.openWritingPipe(_adresses[ind_writer]);
  _radio.openReadingPipe(1, _adresses[ind_reader]);
  _radio.setPALevel(RF24_PA_MIN);
  _radio.startListening();
}

bool Sig_handler::is_available()
{
  return _radio.available();
}

int Sig_handler::read_sig()
{
  int sig = NULL_SIGNAL;
  _radio.read(&sig, sizeof(sig));
  return sig;
}

void Sig_handler::send_sig(int sig)
{
  _radio.stopListening();
  while (!_radio.write(&sig, sizeof(sig))) delay(61);
  _radio.startListening();
}

void Sig_handler::send_sig(int sig, int delay_time)
{
  _radio.stopListening();
  while (!_radio.write(&sig, sizeof(sig))) delay(delay_time);
  _radio.startListening();
}
