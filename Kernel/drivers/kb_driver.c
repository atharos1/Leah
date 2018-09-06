#include <stdint.h>
#include <drivers/console.h>
#include <drivers/kb_driver.h>
#include <asm/libasm.h>
#include <drivers/kb_layout.h>

#define BUFF_SIZE 0xFF

// Non-Shifted scan codes to ASCII:
static unsigned char asciiNonShift[] = {
NULL, ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BACKSPACE,
TAB, 'q', 'w','e', 'r', 't', 'y', 'u', 'i', 'o', 'p',   '[', ']', ENTER, 0,
'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' ', 0,
KF1, KF2, KF3, KF4, KF5, KF6, KF7, KF8, KF9, KF10, 0, 0,
KHOME, 1, KPGUP,'-', KLEFT, '5', KRIGHT, '+', KEND, 2, KPGDN, KINS, KDEL, 0, 0, 0, KF11, KF12 };

// Shifted scan codes to ASCII:
static unsigned char asciiShift[] = {
NULL, ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', BACKSPACE,
TAB, 'Q', 'W',   'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',   '{', '}', ENTER, 0,
'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|',
'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' ', 0,
KF1,   KF2, KF3, KF4, KF5, KF6, KF7, KF8, KF9, KF10, 0, 0,
KHOME, 1, KPGUP, '-', KLEFT, '5',   KRIGHT, '+', KEND, 2, KPGDN, KINS, KDEL, 0, 0, 0, KF11, KF12 };

static char ctrl=0;
static char lshift=0;
static char rshift=0;
static char blockm=0;

//unsigned char readKey();

static void insert(unsigned char c);
int isAlpha(char c);
typedef struct{
	unsigned char array[BUFF_SIZE];
	int i;
	int j;
	int not_read;
} CIRC_BUFFER;

CIRC_BUFFER buff={{0},0,0,0};

/*
 * Fetches a char from the keyboard.
 * Use only in int_33()
 */
char kb_fetch(){
	unsigned char c=(unsigned char)_readKey();
	unsigned char p;

	switch (c){
		case KRRIGHT_SHIFT:
			rshift=1;
			return 0;
			break;
		case KRRIGHT_SHIFT_BK:
			rshift=0;
			return 0;
			break;
		case KRLEFT_CTRL:
			ctrl=1;
			return 0;
			break;
		case KRLEFT_CTRL_BK:
			ctrl=0;
			return 0;
			break;
		case KRLEFT_SHIFT:
			lshift=1;
			return 0;
			break;
		case KRLEFT_SHIFT_BK:
			lshift=0;
			return 0;
			break;
		case KRCAPS_LOCK:
			blockm=!blockm;
			return 0;
			break;
		default:
			break;
	}

	if(c>0x80)
		return 0;



	if (lshift || rshift) {
		p=asciiShift[c];
		if(blockm && isAlpha(p))
			p=asciiNonShift[c];
		insert(p);
	} else {
		p=asciiNonShift[c];
		if(blockm && isAlpha(p))
			p=asciiShift[c];
		insert(p);
	}
	return 1;
}

static void insert(unsigned char c){
	buff.array[buff.i]=c;
	buff.i++;
	if(buff.i==BUFF_SIZE) buff.i=0;
	buff.not_read++;
	if(buff.not_read==BUFF_SIZE+1){
		buff.not_read=0;
	}
}

/*
 * Returns a char from the buffer.
 * -1 if no chars to read
 */
int8_t getChar(){
	if(buff.not_read==0) return -1;
	unsigned ans = buff.array[buff.j];
	buff.j++;
	if(buff.j==BUFF_SIZE) buff.j=0;
	buff.not_read--;
	return ans;
}

/*
 * Peeks the last char inserted
 * to the buffer.
 * -1 if no chars inserted
 */
unsigned char peekChar(){
	if(buff.not_read==0) return -1;
	if(buff.i==0){
		return buff.array[BUFF_SIZE-1];
	}
	return buff.array[buff.i-1];
}

/*
 * Decreases the read pointer by one and
 * restores the given char.
 */
void ungetc(unsigned char c){
	if(buff.j==0){
		buff.j=BUFF_SIZE-1;
	}else{
		buff.j--;
	}
	buff.not_read++;
	buff.array[buff.j]=c;
}

int isAlpha(char c){
	return (c>=65 && c<=90) || (c>=97 && c<=122) ;
}
