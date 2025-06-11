
/*
USB CDC on Boot: Enabled
USb mode: OTG
 * Main File
 */
#include <math.h>
#include "USB.h"
#include "USBMIDI.h"
USBMIDI MIDI;



/**
Sending keypresses as midi CC
*/
#define KEYBOARD_CC_NUM 127
#define KEYBOARD_CC_VALUE_UP 0
#define KEYBOARD_CC_VALUE_DOWN 1
#define KEYBOARD_CC_VALUE_LEFT 2
#define KEYBOARD_CC_VALUE_RIGHT 3


#include <Wire.h>             // Libreria per I2C
#include <Adafruit_GFX.h>     // Libreria grafica
#include <Adafruit_SSD1306.h> // Libreria per il display OLED

#define SCREEN_WIDTH 128 // Larghezza dello schermo in pixel
#define SCREEN_HEIGHT 64 // Altezza dello schermo in pixel

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
/*

SENDER

 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include <WiFiAP.h>
#include <NetworkUdp.h>
#include "Multiplexer.hpp"
// Set these to your desired credentials.
const char *ssid = "yourAP";
const char *password = "yourPassword";

// IP address to send UDP data to:
//  either use the ip address of the server or
//  a network broadcast address
const char *udpAddress = "192.168.4.2";
const int udpPort = 3333;

// Are we currently connected?
boolean connected = false;

// The udp library class
NetworkUDP udp;

#include "Fader.hpp"
#include "Button.hpp"

/// Gesture detection for the preset fader, note: default "middle" value is approx 43
uint8_t threshold_turn_on_hi = 110;
uint8_t threshold_reset_hi = 80;
uint8_t threshold_turn_on_lo = 7;
uint8_t threshold_reset_lo = 30;
uint8_t last_value = 65;
bool gesture_active = false;

uint8_t max_preset = 10;
uint8_t active_preset = 0;
uint8_t next_preset = 0;

void send_midi_cc(uint8_t note, uint8_t value)
{
  uint8_t offset = active_preset * 13; // Note: 13 is the number of MIDI CC senders we currently active
  uint8_t preset_mapped_cc = note + offset; // cc mapped to preset

  Serial.printf("sendMidiCC(%d, %d)\n", preset_mapped_cc, value);
  udp.beginPacket(udpAddress, udpPort);
  udp.write(10); // CC
  udp.write(preset_mapped_cc);
  udp.write(value);
  udp.endPacket();
  MIDI.controlChange(preset_mapped_cc, value); // Usb Midi
}




/**
value is between 0 and 127
**/
void handle_preset_fader(uint8_t _, uint8_t value)
{
  // send_midi_cc(99, value); // TODO: comment this out, its just for debugging

  if (value >= threshold_turn_on_hi && last_value < threshold_turn_on_hi && gesture_active == false) // HI threshold crossing
  {
    // On UP: send a "keypress" message
    send_midi_cc(KEYBOARD_CC_NUM, KEYBOARD_CC_VALUE_UP);

    gesture_active = true;
    next_preset = (next_preset + 1) % max_preset;
    drawNumber(next_preset, 2);
  }
  else if (value <= threshold_reset_hi && last_value > threshold_reset_hi) // HI threshold reset
  {
    // reset preset
    gesture_active = false;
  }
  else if (value <= threshold_turn_on_lo && last_value > threshold_turn_on_lo && gesture_active == false) // DOWN threshold crossing
  {
    // On DOWN: send a "keypress" message
    send_midi_cc(KEYBOARD_CC_NUM, KEYBOARD_CC_VALUE_DOWN);

    gesture_active = true;

    if (next_preset <= 0) {
      next_preset = max_preset - 1;
    } else {
      next_preset = next_preset - 1;
    }

    drawNumber(next_preset, 2);
  }
  else if (value >= threshold_reset_lo && last_value < threshold_reset_lo) // DOWN threshold reset
  {
    // reset preset
    gesture_active = false;
  }

  last_value = value;
}

void handle_confirm_preset(uint8_t _, uint8_t __)
{
  if (next_preset != active_preset)
  {
    active_preset = next_preset;
    //Serial.printf("Active preset changed to %d\n", active_preset);
    drawNumber(active_preset, 3);
  }
}

void ignore_fader(uint8_t _, uint8_t value) {

}

//  Button(int pin, int cc_num, MidiCallback send_midi_cc_fn)
Button b1 = Button(0, 0, send_midi_cc);
Button b2 = Button(1, 1, send_midi_cc);
Button b3 = Button(2, 2, send_midi_cc);
Button b4 = Button(3, 3, send_midi_cc);

Button b5 = Button(4, 99, handle_confirm_preset); // joystick  to wire

//   Fader(int pin, int cc_num, MidiCallback send_midi_cc_fn)
Fader f1 = Fader(0, 4, send_midi_cc);       // pot1
Fader f2 = Fader(1, 5, send_midi_cc);       // pot2
Fader f3 = Fader(2, 6, send_midi_cc);       // pot3
Fader f4 = Fader(3, 7, send_midi_cc);       // fade1
Fader f5 = Fader(4, 8, send_midi_cc);       // fade 2
Fader f6 = Fader(5, 9, send_midi_cc);       // fade 3
Fader f7 = Fader(6, 10, send_midi_cc);       // fade 4
Fader f8 = Fader(7, 11, send_midi_cc);       //
Fader f9 = Fader(8, 12, send_midi_cc);       //
Fader f10 = Fader(9, 13, send_midi_cc);      //
Fader f11 = Fader(10, 14, send_midi_cc);     //
Fader f12 = Fader(11, 15, send_midi_cc); // industrial Joystick x
Fader f13 = Fader(12, 16, send_midi_cc); // industrial Joystick y

Fader f14 = Fader(13, 99, handle_preset_fader); // normal Joystick y
Fader f15 = Fader(14, 99, ignore_fader); // normal Joystick x

void drawNumber(uint8_t x, uint8_t size) {
  display.clearDisplay();      // Pulisce lo schermo
  display.setTextSize(size);      // Imposta la grandezza del testo
  display.setTextColor(WHITE); // Imposta il colore del testo
  display.setCursor(10, 20);   // Imposta la posizione del testo
  display.println(x);    // Testo da visualizzare
  display.display();           // Mostra il testo sul display
}

void setup()
{
  Serial.begin(115200);
  MIDI.begin();
  USB.begin(); // for USB Midi

  pinMode(11, OUTPUT);
  Wire.begin(13, 12);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  drawNumber(active_preset, 3);

  multiplexerSetup();
  // Connect to the WiFi network
  createWifiAp(ssid, password);
  startUdp();
}

int blink = 0;

void loop()
{
  b1.process();
  b2.process();
  b3.process();
  b4.process();
  b5.process();

  f1.process();
  f2.process();
  f3.process();
  f4.process();
  f5.process();
  f6.process();
  f7.process();
  f8.process();
  f9.process();
  f10.process();
  f11.process();
  f12.process();
  f13.process();
  f14.process();
  f15.process();

  delay(10);
  // digitalWrite(11, blink);
  // blink = !blink;
}

void createWifiAp(const char *ssid, const char *pwd)
{
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password))
  {
    log_e("Soft AP creation failed.");
    while (1)
      ;
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void startUdp()
{
  udp.begin(WiFi.localIP(), udpPort);
  connected = true;
}