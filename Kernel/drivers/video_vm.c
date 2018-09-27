#include <stdint.h>
#include "include/font.h"
#include "include/video_vm.h"
#include "include/console.h"

unsigned int bgColor = 0x0;
unsigned int fColor = 0xFFFFFF;

unsigned int SCREEN_WIDTH = 1024;
unsigned int SCREEN_HEIGHT = 768;
unsigned int SCREEN_bPP = 3;

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

struct vbe_mode_info_structure * screenData = (void*)0x5C00;

void init_VM_Driver() {
	SCREEN_bPP = screenData->bpp / 8;
	SCREEN_HEIGHT = screenData->height;
	SCREEN_WIDTH = screenData->width;
}

void drawPixel(unsigned int x, unsigned int y, int color)
{
    char* screen = screenData->framebuffer; 
    unsigned where = (x + y*SCREEN_WIDTH) * SCREEN_bPP;
    screen[where] = color & 255;              // BLUE
    screen[where + 1] = (color >> 8) & 255;   // GREEN
    screen[where + 2] = (color >> 16) & 255;  // RED
}

void drawRectangle(unsigned int x, unsigned int y, int b, int h, int color) {
    for(int i = 0; i < b; i++)
		for(int j = 0; j < h; j++)
			drawPixel(x + i, y + j, color);
}

void drawSquare(unsigned int x, unsigned int y, int l, int color) {
    drawRectangle(x, y, l, l, color);
}

void scrollUp(int cant, unsigned int backgroundColor){
	uint64_t* screen = screenData->framebuffer;

	int i = 0;
	int j = (cant * SCREEN_WIDTH * SCREEN_bPP)/8;
	while (j < (SCREEN_HEIGHT * SCREEN_WIDTH * SCREEN_bPP)/8) {
		screen[i] = screen[j];
		i++;
		j++;
	}

	for (int i = SCREEN_HEIGHT - cant; i < SCREEN_HEIGHT; i++)
		for (int j = 0; j < SCREEN_WIDTH * SCREEN_bPP; j++)
			drawPixel(j, i, backgroundColor);
}

void drawChar(int x, int y, char character, int fontSize, int fontColor, int backgroundColor) {

	int aux_x = x;
	int aux_y = y;

	char bitIsPresent;

	unsigned char* toDraw = charBitmap(character);

	for(int i = 0; i < CHAR_HEIGHT; i++) {
		for(int j = 0; j < CHAR_WIDTH; j++) {
			bitIsPresent = (1 << (CHAR_WIDTH - j)) & toDraw[i];

			if(bitIsPresent)
				drawSquare(aux_x, aux_y, fontSize, fontColor);
			else
				drawSquare(aux_x, aux_y, fontSize, backgroundColor);

			aux_x+=fontSize;
		}
		aux_x = x;
		aux_y+=fontSize;
	}
}

void clearDisplay(unsigned int backgroundColor) {
	char * pos = screenData->framebuffer;
	for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_bPP; i+=SCREEN_bPP) {
		pos[i] = backgroundColor & 255;              // BLUE
    	pos[i + 1] = (backgroundColor >> 8) & 255;   // GREEN
    	pos[i + 2] = (backgroundColor >> 16) & 255;  // RED
	}
}
