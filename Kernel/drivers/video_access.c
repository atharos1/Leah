#include <stdint.h>

#define VBE_DISPI_INDEX_ID 0
#define VBE_DISPI_INDEX_XRES 1
#define VBE_DISPI_INDEX_YRES 2
#define VBE_DISPI_INDEX_BPP 3
#define VBE_DISPI_INDEX_ENABLE 4
#define VBE_DISPI_INDEX_BANK 5
#define VBE_DISPI_INDEX_VIRT_WIDTH 6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 7
#define VBE_DISPI_INDEX_X_OFFSET 8
#define VBE_DISPI_INDEX_Y_OFFSET 9

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF

uint64_t _out(uint16_t port,uint16_t value);
uint64_t _in(uint16_t port);


void set_video_mode(uint32_t width,uint32_t height,uint8_t bpp){
  //Disable VBE
  _out(VBE_DISPI_IOPORT_INDEX,VBE_DISPI_INDEX_ENABLE);
  _out(VBE_DISPI_IOPORT_DATA,0);

  //Set height
  _out(VBE_DISPI_IOPORT_INDEX,VBE_DISPI_INDEX_VIRT_WIDTH);
  _out(VBE_DISPI_IOPORT_DATA,width);

  //Set height
  _out(VBE_DISPI_IOPORT_INDEX,VBE_DISPI_INDEX_VIRT_HEIGHT);
  _out(VBE_DISPI_IOPORT_DATA,height);

  //Set BPP
  _out(VBE_DISPI_IOPORT_INDEX,VBE_DISPI_INDEX_BPP);
  _out(VBE_DISPI_IOPORT_DATA,bpp);

  //Enable VBE
  _out(VBE_DISPI_IOPORT_INDEX,VBE_DISPI_INDEX_ENABLE);
  _out(VBE_DISPI_IOPORT_DATA,1);
  
}

uint16_t version(){
	_out(VBE_DISPI_IOPORT_INDEX,0);
	return _in(VBE_DISPI_IOPORT_DATA);
}

void set_version(uint16_t version){
	_out(VBE_DISPI_IOPORT_INDEX,0);
	_out(VBE_DISPI_IOPORT_DATA,version);
}