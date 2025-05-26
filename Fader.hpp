//Fader.hpp
 

#include "Multiplexer.hpp"
#include "RunningAverage.hpp"

// Calibration

// scale a number 

int linlin(int input, int in_min, int in_max, int out_min, int out_max) {

    if (input < in_min) input = in_min;

    if (input > in_max) input = in_max;

    return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

}




class Fader {
public:
  // Define the function pointer type
  using MidiCallback = void (*)(uint8_t, uint8_t);

  // Constructor
  Fader(int pin, int cc_num, MidiCallback send_midi_cc_fn)
      : pin(pin), cc_num(cc_num), send_midi_cc_fn(send_midi_cc_fn), mem(-1), runningAvg(48) {
  }

  // Process the fader movement
  void process() {
    setMuxChannel(pin, 2);
    //delay(1);

    runningAvg.addValue(analogRead(MUX_SIG_2));
    int val = runningAvg.getAverage();

    val = linlin(val, 300, 3821, 0, 127);

    if (abs(val-mem)>1) {
      send_midi_cc_fn(cc_num, val);
      mem = val;
    }
  }

private:
  RunningAverage runningAvg;
  int pin;
  int cc_num;
  int mem;
  MidiCallback send_midi_cc_fn;
};
