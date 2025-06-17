/*
 * Button.hpp
 */

#include "Multiplexer.hpp"

class Button
{
public:
  // Define the function pointer type
  using MidiCallback = void (*)(uint8_t, uint8_t);

  // Constructor
  Button(int pin, int cc_num, MidiCallback send_midi_cc_fn)
      : pin(pin), cc_num(cc_num), send_midi_cc_fn(send_midi_cc_fn), mem(-1), toggle(0)
  {
  }

  // Process the fader movement
  void process()
  {
    static int hold_count = 0;

    setMuxChannel(pin, 1);
    // delay(1);

    int val = digitalRead(MUX_SIG_1);
    Serial.printf("%d, ", val);

    if (val != mem)
    {
      if (val == 1)
      { // only handle button when pushed

        hold_count++;
        if (hold_count > 10)
        { // after 10 cycles, send a "hold" message
          hold_count = 0;

          if (toggle == 127)
          {
            toggle = 0;
          }
          else
          {
            toggle = 127;
          }

          send_midi_cc_fn(cc_num, toggle);
        }
      }
      else {
        hold_count = 0; // reset hold count when button is released
      }

      mem = val;
    }
  }

  int toggle;

private:
  int pin;
  int cc_num;
  int mem;
  MidiCallback send_midi_cc_fn;
};