#include <stdint.h>
#include <interruptions/intHandlers.h>

void _beep_start(uint16_t freq);
void _beep_stop();

unsigned char beepDuration = 0;


void beep(uint32_t nFrequence, unsigned char duration) {
  beepDuration = duration;
  _beep_start(1193180 / nFrequence);
}

void nosound() {
  beepDuration = 0;
  _beep_stop();
}

void refresh() {
  if (beepDuration > 0) {
    beepDuration--;
    if(beepDuration == 0)
      _beep_stop();
  }
}

void setSpeaker() {
  appendFunctionToTimer(&refresh, 1);
}
