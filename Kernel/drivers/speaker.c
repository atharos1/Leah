#include <stdint.h>
#include <drivers/timer.h>
#include <drivers/speaker.h>

void _beep_start(uint16_t freq);
void _beep_stop();

void beep(uint32_t nFrequence, unsigned char duration) {
  timer_removeFunction(&nosound);
  timer_appendFunction(&nosound, duration);
  _beep_start(1193180 / nFrequence);
}

void nosound() {
  timer_removeFunction(&nosound);
  _beep_stop();
}
