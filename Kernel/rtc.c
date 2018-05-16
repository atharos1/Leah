#include <stdint.h>

uint64_t RTC(uint64_t mode);

typedef struct {
    uint8_t sec,min,hour,day,month;
	uint32_t year;
} time;

/*time * getRTC() {
    time * t = malloc
}*/