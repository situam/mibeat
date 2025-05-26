/*
 *  Multiplexer.hpp
 */

#pragma once

// Multiplexer 1 (digitale)
#define S0_1 6
#define S1_1 7
#define S2_1 8
#define S3_1 9
#define MUX_SIG_1 10 // Lettura digitale

// Multiplexer 2 (analogico)
#define S0_2 4
#define S1_2 1
#define S2_2 2
#define S3_2 3
#define MUX_SIG_2 5 // Lettura analogica

void multiplexerSetup()
{

  // Set multiplexer control pins as outputs
  pinMode(S0_1, OUTPUT);
  pinMode(S1_1, OUTPUT);
  pinMode(S2_1, OUTPUT);
  pinMode(S3_1, OUTPUT);

  // Pullup digital button pin
  pinMode(MUX_SIG_1, INPUT_PULLUP);

  // Set multiplexer control pins as outputs
  pinMode(S0_2, OUTPUT);
  pinMode(S1_2, OUTPUT);
  pinMode(S2_2, OUTPUT);
  pinMode(S3_2, OUTPUT);

  // Set analog pin to input
  pinMode(MUX_SIG_2, INPUT);
}

// Funzione per selezionare il canale di un multiplexer specifico
void setMuxChannel(int channel, int mux)
{
  if (mux == 1)
  { // Multiplexer 1 (digitale)
    digitalWrite(S0_1, bitRead(channel, 0));
    digitalWrite(S1_1, bitRead(channel, 1));
    digitalWrite(S2_1, bitRead(channel, 2));
    digitalWrite(S3_1, bitRead(channel, 3));
  }
  else
  { // Multiplexer 2
    digitalWrite(S0_2, bitRead(channel, 0));
    digitalWrite(S1_2, bitRead(channel, 1));
    digitalWrite(S2_2, bitRead(channel, 2));
    digitalWrite(S3_2, bitRead(channel, 3));
  }
}
