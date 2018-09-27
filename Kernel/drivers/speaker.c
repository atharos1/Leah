#include <stdint.h>
#include "include/timer.h"
#include "include/speaker.h"
#include "asm/libasm.h"


void beep(uint32_t nFrequence, unsigned char duration) {
  timer_removeFunction(&nosound);
  timer_appendFunction(&nosound, duration);
  _beep_start(1193180 / nFrequence);
}

void nosound() {
  timer_removeFunction(&nosound);
  _beep_stop();
}
