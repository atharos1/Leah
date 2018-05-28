#include <stdint.h>
#include <drivers/font.h>
#include <drivers/video_vm.h>

unsigned int bgColor = 0x0;
unsigned int fColor = 0xFFFFFF;

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

void drawPixel(unsigned int x, unsigned int y, int color)
{
    char* screen = screenData->framebuffer;
    unsigned where = (x + y*SCREEN_WIDTH) * SCREEN_bPP;
    screen[where] = color & 255;              // BLUE
    screen[where + 1] = (color >> 8) & 255;   // GREEN
    screen[where + 2] = (color >> 16) & 255;  // RED
}

void invertPixel(unsigned int x, unsigned int y) {
	char* screen = screenData->framebuffer;
	unsigned where = (x + y*SCREEN_WIDTH) * SCREEN_bPP;
	screen[where] = 255 - screen[where];          // BLUE
	screen[where + 1] = 255 - screen[where + 1];  // GREEN
	screen[where + 2] = 255 - screen[where + 2];  // RED
}

// void drawChar(char c, unsigned int x, unsigned int y) {
//
//     char * charData = charBitmap(c);
//
//     /*for(int j = 0; j < 16; j++) {
//         for(int i = 0; i < 8; i++) {
//             //if( charData[ (j*8) + i ] == 1 )
//                 drawPixel(x+i, y+j, fColor);
//         }
// 	}*/
//
// 	int shifted = 0;
//
// 	for(int i = 0; i<16;i++) {
// 		for(int j = 0; j<8; j++) {
//
// 			shifted = 1<<(7-j) & charData[ j ];
//
// 			if( shifted > 0 )
// 				drawPixel(8-1-j+x,i+y,fColor);
// 			else
// 				drawPixel(8-1-j+x,i+y,bgColor);
// 		}
// 	}
// }

void drawChar(int x, int y, char character, int fontColor, int backgroundColor) {

	int aux_x = x;
	int aux_y = y;

	char bitIsPresent;	

	char* toDraw = charBitmap(character);

	for(int i = 0; i < CHAR_HEIGHT; i++) {
		for(int j = 0; j < CHAR_WIDTH; j++) {
			bitIsPresent = (1 << (CHAR_WIDTH - j)) & toDraw[i];
			if(bitIsPresent)
				drawPixel(aux_x, aux_y, fontColor);
			else
				drawPixel(aux_x, aux_y, backgroundColor);
			aux_x++;
		}
		aux_x = x;
		aux_y++;
	}
}

void invertChar(int x, int y) {
	int aux_x = x;
	int aux_y = y;

	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 8; j++) {
			invertPixel(aux_x, aux_y);
			aux_x++;
		}
		aux_x = x;
		aux_y++;
	}
}

void clearDisplay() {
	char * pos = screenData->framebuffer;
	for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
		*pos = 0;
		pos++;
		*pos = 0;
		pos++;
		*pos = 0;
		pos++;
	}
}

// void writeStringToScreen(int x, int y, char* string) {
//
// 	int aux_x = x;
// 	char* aux_s = string;
//
// 	for(int i = 0; (*aux_s) != '\0'; i++) {
// 		drawChar(aux_x, y, (*aux_s));
// 		aux_s++;
// 		aux_x += 8;
// 	}
// }
