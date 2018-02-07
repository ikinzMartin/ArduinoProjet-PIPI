#include "Sig_handler.h"

/**
    CS-11 Asn 2
    Sig_handler.cpp
    Purpose: an handler for signals sents on arduino by an NRF24L01.
    This lib is based on RF24.

    @author Alexandre Hulsken
    @version 0.1 07/02/18
*/


/**
    Constructor.
    Creates a new instance of this driver. Before using, you must initialized this handler in the setup().

    @param ce The pin attached to Chip Enable on the RF module
    @param csn The pin attached to Chip Select
*/
Sig_handler::Sig_handler(int ce, int csn) : _radio(ce, csn)
{
}


/**
    Initialized the RF module used in this handler

    @param ind_writer the indice for the address to the writer
    @param ind_reader the indice for the address to the reader
    @return void
*/
void Sig_handler::init_handler(int ind_writer, int ind_reader)
{
  _radio.begin();
  _radio.openWritingPipe(_adresses[ind_writer]);
  _radio.openReadingPipe(1, _adresses[ind_reader]);
  _radio.setPALevel(RF24_PA_MIN);
  _radio.startListening();
}

/**
    Test whether there is a signal available to be read

    @return True if there is a payload available, false if none is
*/
bool Sig_handler::is_available()
{
  return _radio.available();
}

/**
    Return the last signal received

    @return The last signal received
*/
signal Sig_handler::read_sig()
{
  signal sig = NULL_SIGNAL;
  _radio.flush_rx();
  _radio.read(&sig, sizeof(sig));
  return sig;
}

/**
    Send a signal to an another NRF24L01

    @param sig the signal to send
    @return void
*/
void Sig_handler::send_sig(signal sig)
{
  _radio.stopListening();

  do _radio.flush_tx();
  while (!_radio.write(&sig, sizeof(sig)));

  _radio.startListening();
}

/**
    Print a giant block of debugging information to stdout.

    @Warning Does nothing if stdout is not defined. See fdevopen in stdio.h 
    @return void
*/
void Sig_handler::debugging_mode()
{
  _radio.printDetails();
}
