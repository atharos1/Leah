#ifndef VIDEO_ACCESS_H
#define VIDEO_ACCESS_H

#include <stdint.h>

void set_video_mode(uint32_t width,uint32_t height,uint8_t bpp);
uint16_t version();
void set_version(uint16_t version);
#endif
