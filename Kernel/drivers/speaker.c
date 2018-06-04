#include <stdint.h>
#include <interruptions/intHandlers.h>
#include <drivers/speaker.h>

void _beep_start(uint16_t freq);
void _beep_stop();

void beep(uint32_t nFrequence, unsigned char duration) {
  removeFunctionFromTimer(&nosound);
  appendFunctionToTimer(&nosound, duration);
  _beep_start(1193180 / nFrequence);
}

void nosound() {
  removeFunctionFromTimer(&nosound);
  _beep_stop();
}
