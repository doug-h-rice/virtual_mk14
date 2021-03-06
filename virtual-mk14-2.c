/*   
 * Virtual MK14, an MK14 emulator based on Nascom II emulator.
 * 
 * usage: "./virtual-mk14-2 <file.hex>\n"
 * 
 * <file.hex> is a file of assembled code in intell ihx format.
 * 
 * see:
 * http://ccgi.dougrice.plus.com/cgi-bin/wiki.pl?MK14_Notes
 * 
 * My github: https://github.com/doug-h-rice
 * I have a version that uses SDL called mk14_src_sdl_1.zip at
 * 
 * based on Virtual Nascom, a Nascom II emulator.
 * based on Paul Robson's MK14 simulator for DOS
 * ported to use SDL2.0.6 instead of SDL1.2
 * The SDL1.2 code has been commented out.
 * 
     Copyright (C) 2000,2009,2017  Tommy Thorn
     Copyright (C) 2000,2009,2017  Paul Robson
     Copyright (C) 2017  Doug Rice

     Z80 emulator portition Copyright (C) 1995,1998 Frank D. Cringle.
     SC/MP emulator portition CPU.C Copyright (C) 1998 Paul Robson
     
	 Mash together copyright (C) 2017 Doug Rice 

     This is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
     02111-1307, USA.

  With the SC/MP emulator in place the first thing to get working is the
  screen memory. The "correct" way to simulate screen memory is to
  trap upon writes, but that would be slow.  We do it any just to get
  started.
  * 
  * This version used two display methods.
  * I have added drawing segments by rectangles in one window and
  * 
  * 
  *  
  * and also kept the display using fonts in another window.
  *  
  * 
  * SDL has surfaces and BLITS the display surface with bitmaps from
  * the character rom surface which are indexed by the character.
  * 
  * for the MK14 the display is done buy using three rows of characters.
  * 
  *  _   _   _   _   _   _   _   _      a 
  * |_| |_| |_| |_| |_| |_| |_| |_|    fgb
  * |_|,|_|,|_|,|_|,|_|,|_|,|_|,|_|,   edch
  * 
  * segment names
  *  a
  * fgb
  * edch
  * 
  * bits:  h,g,f,e,d,c,b,a
  * 
  * The status register is displayed as a 4x2 grid on the right
  * The status register:-
  *	7   6  5  4  3  2  1  0
  *	cy/l,OV,SB,SA,IE,F2,F1,F0
  *	Bit Function 	Notes
  *	0 	F0 	Output Line
  *	1 	F1 	Output Line
  *	2 	F2 	Output Line
  *	3 	IE 	Interrupt Enable / HALT
  *	4 	SA 	Input Line + INT,
  *	5 	SB 	Input Line
  *	6 	OV 	Overflow
  *	7 	CY/L Carry / Link bit
  *
  * SA and SB are inputs, and work in needed here
  * 
  * SIN and SIO are an input and output, but not emulated yet.
  * 
  * 
  *  
  * The Keyboard is also a challenge.
  * 
  * =================================
  * Meoory map 0D07 on left, 0d00 is on right
  * =================================
  * 0D00+   7  6  5  4  3  2  1  0
  * =================================
  * bit7    7  6  5  4  3  2  1  0
  * bit6                      9  8
  * bit5    T        Z  M  G
  * bit4    F  E        D  C  B  A
  * =================================
  * 0D00+   8  7  5  4  3  2  1  0
  * =================================
  *
  * G = GO
  * M = MEM
  * T = TERM
  * Z = ABORT
  * Q or  / quites emulator 
  *  
  * G = GO		- run from displayed address
  * M = MEM 	- increment address and allow hex input
  * T = TERM 	- allow hex input
  * Z = ABORT 	- allow address input
  * 
  * Q or  / quites emulator 

  * There is an array of which key is pressed. 
  * This is indexed by two mappings, one for the MK14 hardware 
  * and one for the PC character mappings.
  * 
  * The MK14 had hardware to aid scanning the keyboard.
  * The MK14 tests for a key press by reading a display address.
  * If a key is pressed the bit is low
  * 
  * the host pc's keyboard needs to interface to the simulated hardware
  * This is done by arrays and functions. 

  *
  * To get this to build on Ubuntu and Raspberry Pi I needed to load SDL1.2
  * 
  * To build on Raspberry Pi RASPBIAN STRETCH,I needed to load SDL2.0.6
  * 
  * SDL version
  *
  * Building this code needs SDL2.0.6 
  * code for SDL1.2 is commented out.
  *  
  *Download SDL2.0
  *Simple DirectMedia Layer - Homepage
  *https://www.libsdl.org/
  *Simple DirectMedia Layer is a cross-platform development library designed to provide
  *SDL officially supports Windows, Mac OS X, Linux, iOS, and Android.
‎  *SDL version 2.0.5 (stable) · ‎SDL Wiki ·  ‎License

  *download SDL2.0
  * 
  *install
  *./configure && make  
  *   *sudo make install
  *  
  *    
  * usage: ./virtual-mk14-2 flags file
  *    -v -verbose
  * 	file.ihx is a file containing intel hex format or .nas format
  * 
  * 	It currently saves in NASCOM HEX format
  *   
# Makefile for Virtual MK14 using SDL2
srcdir  = .
CC      = gcc
EXE	= 
CFLAGS  = -g -O2 -D_REENTRANT -I/usr/local/include/SDL2 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -DHAVE_OPENGLES -DHAVE_OPENGLES2 -DHAVE_OPENGL -g
LIBS	=  -lSDL2_test -L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lSDL2

#SDL1.2 version
#virtual-mk14: cpu.o memory.o virtual-mk14.o
#	$(CC) $(CWARN) $(shell sdl-config --libs) $^ -o $@ -lSDL

#SDL2 version
virtual-mk14-2: cpu.o memory.o virtual-mk14-2.o
	$(CC) $(CWARN)  $^ -o $@ -lSDL2

clean:
	rm -f *.o *~ core
  *  
  * 

pi@raspberrypi:~/Desktop/virtual_mk14-master $ make
gcc -g -O2 -D_REENTRANT -I/usr/local/include/SDL2 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -DHAVE_OPENGLES -DHAVE_OPENGLES2 -DHAVE_OPENGL -g   -c -o virtual-mk14-2.o virtual-mk14-2.c
gcc   cpu.o memory.o virtual-mk14-2.o -o virtual-mk14-2 -lSDL2

 *
 * Doug Rice, 2017
 * 
 * Possible changes:-
 * Add:
 *  disassembler to file / console
 *  load ihex from clipboard or file while running.
 *  Status LEDs
 *  add SIO SIN
 *  add SA,SB
 *
 * Code mash up needs a tidy. 
 * 
 * 
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <stdbool.h>
#include <SDL.h>
#include "scmp.h"

/* from simz80.h  */
#include <limits.h>
#include <stdint.h>
#include <string.h>

#if UCHAR_MAX == 255
typedef uint8_t BYTE;
#else
#error Need to find an 8-bit type for BYTE
#endif

#if USHRT_MAX == 65535
typedef uint16_t WORD;
#else
#error Need to find an 16-bit type for WORD
#endif
 
//#define MEMSIZE 64
//BYTE ram[MEMSIZE*1024+1];       /* Z80 memory space */

char *monitor;
char *progname;

int debugCounter = 0;
		
bool  verbose;
unsigned char keym[9];
 
/********************************************************************/
/* 		Prototypes                                              	*/
/********************************************************************/

int setup(int, char **);
int load_both_formats(char *file);
void save_nascom(int start, int end, const char *name);


/* display */
void DrawSeg(SDL_Renderer * renderer, int on,int x, int y , int w, int h);
void DrawSeg7(SDL_Renderer * renderer, int digit ,int x, int y );
void DrawDisplay(SDL_Renderer * renderer, int * digits  );






//#define RAM(a)		ram[(a)&0xffff]

/*****************************************************/

#define SLOW_DELAY  25000
#define FAST_DELAY 900000

static bool go_fast = false;
static int t_sim_delay = SLOW_DELAY;

bool key_pressed;

#define FONT_H_PITCH 16
#define FONT_H       15
#define FONT_W        8
#define FONT_V        0

#define DISPLAY_WIDTH   ( 4*8*10) 
#define DISPLAY_HEIGHT  ( 4*16 )	
#define DISPLAY_X_OFFSET 0
#define DISPLAY_Y_OFFSET 0

extern int LEDStat[DIGITS];
extern void BlockExecute(void);

static SDL_Surface *screen;

/* for SDL1.2 */
SDL_Window   * window;
SDL_Renderer * renderer;

/* for SDL2 */
SDL_Window 	 * window2;
SDL_Renderer * renderer2;


SDL_Surface *surface;
SDL_Surface *image;
SDL_Texture *sdlTexture;
int done;


static struct font {
    SDL_Surface *surf;
    int w, h, h_pitch;
} seg7_font;

static int  seg7dispA[10];
static char seg7segA[  8];
static char seg7hexA[  32]; /* lookup */

/* 7 segment font
  *  _   _   _   _   _   _   _   _      a 
  * |_| |_| |_| |_| |_| |_| |_| |_|    fgb
  * |_|,|_|,|_|,|_|,|_|,|_|,|_|,|_|,   edch
  * 
  * segment names
  *  a
  * fgb
  * edch
  * 
  * bits:  h,g,f,e,d,c,b,a
*/


/*
 * 
 * The MK14 had a 7 segment display with thin segments.
 * define one of these below:
 * S7THIN - thin like the MK14 but not very visable 
 * S7MEDIUM - most visable and readable
 * S7THICK - clumsy
 * 
 */

//#define S7THIN
#define S7MEDIUM
//#define S7THICK

uint8_t seg7_font_raw[16*4] = {	

#ifdef S7MEDIUM
	/* ' ' */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	/* '_' */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7e, 0xff, 0x7e, 0x00, 0x00, 0x00,

	/* '|' */	
    0x18, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c,
    0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x18, 0x00, 0x00,
    
	/* '.' */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x60, 0xf0, 0xf0, 0x60, 0x00, 0x00

#endif

#ifdef S7THIN

	/* ' ' */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	/* '_' */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00,

	/* '|' */	
    0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00,
    
	/* '.' */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x60, 0xf0, 0x60, 0x60, 0x00, 0x00
#endif

#ifdef S7THICK
	/* ' ' */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	/* '_' */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7e, 0xff, 0xff, 0xff, 0x7e, 0x00,

	/* '|' */	
    0x00, 0x00, 0x3c, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 
    0x7e, 0x7e, 0x7E, 0x7e, 0x3c, 0x3c, 0x00, 0x00,
    
	/* '.' */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x60, 0xf0, 0x60, 0x60, 0x00, 0x00

#endif
};

/*
 * defines for 7 segment display segment map. 
 * 
 */
 
#define SA ( 1<<0 )
#define SB ( 1<<1 )
#define SC ( 1<<2 )
#define SD ( 1<<3 )
#define SE ( 1<<4 )
#define SF ( 1<<5 )
#define SG ( 1<<6 )
#define SH ( 1<<7 )

/*
 *=========================================================================== 
 * 
 */

typedef struct _LEDRECT
{
int left,top,right,bottom,sx,cy;
} LEDRECT;

int ConX = 0;        					/* Console horizontal position */
char CurrentKey = ' ';					/* Current key being pressed */
clock_t KeyTime = 0;					/* Time that key was pressed */
void *ScBuffer;							/* Buffer for graphics sccopy */
int  CurrentKeyID = -1;					/* Current key pressed */
int  KeyPressMatrix[32];				/* Matrix of key presses */
LEDRECT rcLED[DIGITS];					/* LED rectangles */

/********************************************************************/
/* 		Initialise Keyboard & Display, Open Windows etc.			*/
/********************************************************************/

void CONInitialise(void)
{
char *p;
int i ;//,x,GDriver,GMode;
p = " Portable MK14 Emulator in 'C' (MSDOS version)";

ConX = 0;CurrentKey = ' ';KeyTime = 0;	/* Initialise everything */
CurrentKeyID = 0;
for (i = 0;i < 32;i++)
	KeyPressMatrix[i] = 0;
										/* Personal prompt */
p = "Mk14 'C' Emulator & DOS Port by Paul Robson.\r";
while (*p != '\0') CONWrite(*p++);
}

/********************************************************************/
/*				Tidy up everything ready for exit					*/
/********************************************************************/

void CONTerminate(void)
{
 /* stub */
}

/********************************************************************/
/*			  Write a character to the information console			*/
/********************************************************************/

void CONWrite(char cOut)
{
 /* stub */
}

/********************************************************************/
/*		Read a Key. Characters 8 (BS) and 13 (CR) als supported		*/
/********************************************************************/
char kbhit( void ){
	bool kp = key_pressed; 
    key_pressed = (1==0);
	return kp;
}


char CONRead(void)
{
  /* stub */	
  char c;
  c='a';
return(c);
}

/********************************************************************/
/*					Check to see if a key is pressed				*/
/********************************************************************/

int CONKeyPressed(int KeyID)
{

while (kbhit())							/* new Key press ? */
	{
	KeyPressMatrix[CurrentKeyID] = 0;
	
	//CurrentKey = getch();				/* Get which key it was */
	CurrentKey = toupper(CurrentKey);
	//KeyTime = clock();					/* This is when we got it... */

	CurrentKeyID = -1;					/* Find out which key it is */
	if (isdigit(CurrentKey))
			CurrentKeyID = CurrentKey-'0';
	if (CurrentKey >= 'A' && CurrentKey <= 'F')
			CurrentKeyID = CurrentKey-'A'+10;
	if (CurrentKey == 'T')		CurrentKeyID = KEY_TERM;
	if (CurrentKey == 'M')		CurrentKeyID = KEY_MEM;
	if (CurrentKey == 'Z')		CurrentKeyID = KEY_ABORT;
	if (CurrentKey == 'G')		CurrentKeyID = KEY_GO;
	if (CurrentKey == '/')		CurrentKeyID = KEY_BREAK;
	if (CurrentKey == 'Q')		CurrentKeyID = KEY_BREAK;
	if (CurrentKey == 'T')		CurrentKeyID = KEY_TERM;
	if (CurrentKey == 'R')		CurrentKeyID = KEY_RESET;
	if (CurrentKeyID >= 0)		KeyPressMatrix[CurrentKeyID] = 1;
	else
		CurrentKeyID = 0;
	}
	
#ifdef comment
/* DOS does not has a keydown and key up, so use a times press */

if (CurrentKey != ' ')					/* auto release after 2 ticks */
	{
	if (clock() > KeyTime+4)
		{
		CurrentKey = ' ';
		KeyPressMatrix[CurrentKeyID] = 0;
		}
	}
#endif	

  return(KeyPressMatrix[KeyID] != 0);
}

/********************************************************************/
/*		   Synchronise the emulated and real CPU Speeds				*/
/********************************************************************/

clock_t	LastClock;
long CycCount = 0L;

void CONSynchronise(long AddCycles)
{
  CycCount = CycCount+AddCycles;
}

/********************************************************************/
/*				Draw a LED pattern on the screen					*/
/********************************************************************/

void CONDrawLED(int Digit,int BitPattern)
{
	/* stub */
}


void RenderItem(struct font *font, int idx, int x, int y)
{
    auto SDL_Rect dest = { DISPLAY_X_OFFSET + x, DISPLAY_Y_OFFSET + y,
                           font->w, font->h };
    SDL_Rect clip = { 0, idx * font->h_pitch, font->w, font->h };
    SDL_BlitSurface(  font->surf, &clip, screen, &dest );
}




/********************************************************************/
/*				Keyboard functionality					*/
/********************************************************************/

typedef enum { CONT = 0, RESET = 1, DONE = -1 } sim_action_t;
static sim_action_t action = CONT;

//sd1   void handle_key_event_raw(SDL_keysym keysym, bool keydown);
void handle_key_event_raw(SDL_Keysym keysym, int keydown);

/* use function pointer to corrct routine */
//sd1 void (*handle_key_event)(SDL_keysym, bool) = handle_key_event_raw;
void (*handle_key_event)(SDL_Keysym, int ) = handle_key_event_raw;

//sd1 void handle_app_control(SDL_Keysym keysym, bool keydown)
void handle_app_control(SDL_Keysym keysym, int  keydown)
{
	/* force out of reset */
	action = CONT;

    if (!keydown) {
	
      switch (keysym.sym) {
        case SDLK_F2:
            printf("key pressed f2 UP\n");
			CurrentKeyID = KEY_MEM;
			KeyPressMatrix[CurrentKeyID] = 0;
		    CurrentKey = ' ';
		    KeyPressMatrix[CurrentKeyID] = 0;
            break;
        case SDLK_F3:
            printf("key pressed f3 UP\n");
			CurrentKeyID = KEY_GO;
			KeyPressMatrix[CurrentKeyID] = 0;
		    CurrentKey = ' ';
		    KeyPressMatrix[CurrentKeyID] = 0;
            break;
        default:
            ;
        }
    }    

	
    if (keydown){
        //printf("key pressed");

        switch (keysym.sym) {
        case SDLK_END: {
            printf("END key - saving memory in ram.nas\n");
            save_nascom(0xf00, 0xfff, "ram.nas");
            if (verbose) printf("mem dumped\n");
            break;
        }

        case SDLK_F1: 
            printf("F1 key - saving memory in ram.nas\n");
            save_nascom(0xf00, 0xfff, "ram.nas");
            if (verbose) printf("mem dumped\n");
            break;
			break;

        case SDLK_F2:
            printf("key pressed F2 DOWN\n");
		    CurrentKey = 'M';
			CurrentKeyID = KEY_MEM;
			KeyPressMatrix[CurrentKeyID] = 1;
            break;

        case SDLK_F3:
            printf("key pressed F3 DOWN\n");
		    CurrentKey = 'G';
			CurrentKeyID = KEY_GO;
			KeyPressMatrix[CurrentKeyID] = 1;
            break;


        case SDLK_F4:
            printf("key pressed F4 DOWN\n");
            action = DONE;
            CurrentKeyID = KEY_BREAK;
			KeyPressMatrix[CurrentKeyID] = 1;
            break;

        case SDLK_F5:
            go_fast = !go_fast;
            printf("Switch to %s\n", go_fast ? "fast" : "slow");

            t_sim_delay = go_fast ? FAST_DELAY : SLOW_DELAY;
            break;

        case SDLK_F9:
            action = RESET;
            break;

        default:
            ;
        }
    }    
}

//sd1 void handle_key_event_raw(SDL_keysym keysym, bool keydown)
void handle_key_event_raw(SDL_Keysym keysym, int keydown)
{
    if (keysym.sym < 128) {
      CurrentKey = toupper(keysym.sym);
    }
  
  	KeyPressMatrix[CurrentKeyID] = 0;
	CurrentKey = keysym.sym;				/* Get which key it was */
	CurrentKey = toupper(CurrentKey);


	CurrentKeyID = -1;					/* Find out which key it is */
	if (isdigit(CurrentKey))			CurrentKeyID = CurrentKey-'0';
	if (CurrentKey >= 'A' && CurrentKey <= 'F')			CurrentKeyID = CurrentKey-'A'+10;
	if (CurrentKey == 'T')			CurrentKeyID = KEY_TERM;
	if (CurrentKey == 'M')			CurrentKeyID = KEY_MEM;
	if (CurrentKey == 'Z')			CurrentKeyID = KEY_ABORT;
	if (CurrentKey == 'G')			CurrentKeyID = KEY_GO;
	if (CurrentKey == '/')			CurrentKeyID = KEY_BREAK;
	if (CurrentKey == 'T')			CurrentKeyID = KEY_TERM;
	if (CurrentKey == 'R')			CurrentKeyID = KEY_RESET;

   if (keydown) {
	  if (CurrentKeyID >= 0)		KeyPressMatrix[CurrentKeyID] = 1;

	  else {		CurrentKeyID = 0;    } 
      switch (keysym.sym) {
        default:
          handle_app_control(keysym, keydown);
      }
   }   
    
   if (!keydown) {
      if (CurrentKeyID >= 0)	{	
		/*  
		 * Enable trace on key press
		 * 
	    printf( "key------------- %d --------\n", keysym.sym  );
		debugCounter = 100;
		* */
		CurrentKey = ' ';
		KeyPressMatrix[CurrentKeyID] = 0;
      }
    }           
}


/*
 * 
 * This function loads *.nas, *.ihx, *.hex format files 
 * If an error is found it reports an error and returns.
 * 
 * it is more robust than the originals
 * 
 * */  
int load_both_formats(char *file) {
   uint hex_read, hex_len, hex_addr, hex_cmd ;
   uint hex_count, hex_data, hex_check ;
   
   /*0F58 00 00 00 00 00 00 00 00 00*/
   int a, b1, b2, b3, b4, b5, b6, b7, b8, b9;
   char c10, c11;
   long last,now;

  
   FILE *stream = fopen( file,"rb");
   printf("\n === loading: %s ", file );

   if (!stream) {
	   stream=0;
       printf("error loading file: %s", file );
	   return (1==0);
   }


   /* use ftell() to check for read errors */
   last=0;
   now=0;
   while ( !feof( stream ) ) {
/*
 * 
 * .ihx format and .hex format
:180FDC00F6C40135C20031C100CA00C20131C100CA01C4FFCA0F925D84
:00000001FF
 * 
 *
:len addr 00 xx xx xx .. check                                        
:18  0FDC 00 F6C40135C20031C100CA00C20131C100CA01C4FFCA0F925D84
:18  0FDC 00 F6C40135C20031C100CA00C20131C100CA01C4FFCA0F925D84
:0E  100E 00 6C6C6F20646F75670A00DDE5DD21 F4 
 *  
 */
 	   	   
   /*:0E 100E 00 6C6C6F20646F75670A00DDE5DD21 F4 */
//     printf( "\nftell: %ld\n", ftell(stream) );
     
     hex_read = fscanf(stream,":%2x%4x%2x",&hex_len,&hex_addr,&hex_cmd);

     if ( hex_read ){
       printf( "\n%x  %2d, %2X, %x  : ", hex_read, hex_len, hex_addr, hex_cmd );
       for( hex_count= 0 ; hex_count < hex_len ; hex_count++ ){
         /* limit address */ 
		 hex_addr = hex_addr & 0xFFF;
		   
	     hex_read = fscanf(stream, "%2x",&hex_data ); 	  
         printf(" %02X", hex_data );
         Memory[ hex_addr ] = hex_data ;
         hex_addr ++;
       }    
       hex_read = fscanf(stream, "%2x\n",&hex_check);
     }     
   /* *.nas format   addr datax8 00 BS BS  e.g. */
   /*0F58 00 00 00 00 00 00 00 00 00*/
    hex_read = fscanf(stream, "%x %x %x %x %x %x %x %x %x %x%c%c\n",
	     &a , &b1, &b2, &b3, &b4, &b5, &b6, &b7, &b8, &b9, &c10, &c11 );
	       
	if ( hex_read == 12 ) {
	  printf("\n%d  [%04x]   %02x %02x %02x %02x  %02x %02x %02x %02x  %02x {%d %d}",	
	     hex_read,         a,    b1,  b2, b3,    b4,   b5,  b6,  b7,  b8,   b9, (int)c10, (int)c11 );
//	  if ( a > 0 ){ 
	  if ( c10 == c11 ){ 
		a = ( a & 0xFFF );   
		Memory[a]   = b1;
		Memory[a+1] = b2;
		Memory[a+2] = b3;
		Memory[a+3] = b4;
		Memory[a+4] = b5;
		Memory[a+5] = b6;
		Memory[a+6] = b7;
		Memory[a+7] = b8;
      }
    }

	/* 
	 * 
	 * if the fscanf's have not matched,
	 * the position reported by ftell() does not change 
	 * 
	 */

	/* check for error reading data */
	 
    now=ftell( stream );
    if (last == now){
		printf("\n format error @ %ld\n",now);
		break;
	};
    last = now;
   }
   fclose(stream);
   stream=0;
   return (1==1)  ;
}

void save_nascom(int start, int end, const char *name)
{
    FILE *f = fopen(name, "w+");

    if (!f) {
        perror(name);
        return;
    }
    if( f ){
//    for (uint8_t *p = ram + start; start < end; p += 8, start += 8)
      for (uint8_t *p = Memory + start; start < end; p += 8, start += 8)
        fprintf(f, "%04X %02X %02X %02X %02X %02X %02X %02X %02X %02X%c%c\r\n",
                start, *p, p[1], p[2], p[3], p[4], p[5], p[6], p[7], 0, 8, 8);

      fclose(f);
	}
}

void ui_serve_input(void)
{
    SDL_Event event;

    if (SDL_PollEvent(&event)) {

		//printf("SDL_PollEvent\n");
        
        switch (event.type) {
        case SDL_MOUSEMOTION:
            /*printf("Mouse moved by %d,%d to (%d,%d)\n",
              event.motion.xrel, event.motion.yrel,
              event.motion.x, event.motion.y);*/
            break;
        case SDL_MOUSEBUTTONDOWN:
            /*printf("Mouse button %d pressed at (%d,%d)\n",
              event.button.button, event.button.x, event.button.y);*/
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            handle_key_event(event.key.keysym, event.type == SDL_KEYDOWN);
            break;
        case SDL_QUIT:
			action = DONE;
			CurrentKeyID = KEY_BREAK;       
            printf("Quit\n");
            return;
        default:
            //printf("Unknown event: %d\n", event.type);
            break;
        }
        key_pressed = ( 1==1 );
    }
}

/**********************************************
 * prototypes
 * ********************************************/


void init_seg7(void){
	/* test: set up 7 seg mapping */
	//int count;

	seg7hexA[0]  =  SA+SB+SC+SD+SE+SF ;
	seg7hexA[1]  =  SB+SC ;
	seg7hexA[2]  =  SA+SB+SD+SE+SG ;
	seg7hexA[3]  =  SA+SB+SC+SD+SG ;
	seg7hexA[4]  =  SB+SC+SF+SG ;
	seg7hexA[5]  =  SA+SC+SD+SF+SG ;
	seg7hexA[6]  =  SA+SC+SD+SE+SF+SG ;
	seg7hexA[7]  =  SA+SB+SC ;
	seg7hexA[8]  =  SA+SB+SC+SD+SE+SF+SG ;
	seg7hexA[9]  =  SA+SB+SC+SF+SG ;
	seg7hexA[10] =  SA+SB+SC+SE+SF+SG ;
	seg7hexA[11] =  SC+SD+SE+SF+SG ;
	seg7hexA[12] =  SA+SD+SE+SF ;
	seg7hexA[13] =  SB+SC+SD+SE+SG ;
	seg7hexA[14] =  SA+SD+SE+SF+SG ;
	seg7hexA[15] =  SA+SE+SF+SG ;

/*
	for ( count = 0 ; count < 10 ; count++ ){
      seg7dispA[count] = seg7hexA[count];
    }
*/
	
}
 
void ui_display_refresh_seg7( void );

void ui_display_refresh_seg7(void)
{
    int x;
	/* set up 7 segment display */
	/*
	 *    a
	 *  f g b
	 *  e d c
	 *  
0251   110B             ; ****************************************************************************
0252   110B             ;                               Segment Data
0253   110B             ; ****************************************************************************
0254   110B             SA      .equ    1                       ; Segment bit patterns
0255   110B             SB      .equ    2
0256   110B             SC      .equ    4
0257   110B             SD      .equ    8
0258   110B             SE      .equ    16
0259   110B             SF      .equ    32
0260   110B             SG      .equ    64
0261   110B             ; ****************************************************************************
0262   110B             ;                    Hex number to seven segment table
0263   110B             ; ****************************************************************************
0264   110B 3F          CRom:   .db     SA+SB+SC+SD+SE+SF
0265   110C 06                  .db     SB+SC
0266   110D 5B                  .db     SA+SB+SD+SE+SG
0267   110E 4F                  .db     SA+SB+SC+SD+SG
0268   110F 66                  .db     SB+SC+SF+SG
0269   1110 6D                  .db     SA+SC+SD+SF+SG
0270   1111 7D                  .db     SA+SC+SD+SE+SF+SG
0271   1112 07                  .db     SA+SB+SC
0272   1113 7F                  .db     SA+SB+SC+SD+SE+SF+SG
0273   1114 67                  .db     SA+SB+SC+SF+SG
0274   1115 77                  .db     SA+SB+SC+SE+SF+SG
0275   1116 7C                  .db     SC+SD+SE+SF+SG
0276   1117 39                  .db     SA+SD+SE+SF
0277   1118 5E                  .db     SB+SC+SD+SE+SG
0278   1119 79                  .db     SA+SD+SE+SF+SG
0279   111A 71                  .db     SA+SE+SF+SG

	 *    a
	 *  f g b
	 *  e d c

	 */
	
	for (x = 0; x < 8; ++x){
      //seg7dispA[ x ] = ram[0x0BCA+x];
      //seg7dispA[ x ] = Memory[0xd00+x];
      
      /* MK14 display 0xd08 on left down to 0xd00 on right */
	  seg7dispA[ x ] = LEDStat[7-x];

      /* mask of bit and set character */     
      seg7segA[0] = ( seg7dispA[ x ] & SA ) ? 1 : 0 ;  // '_' 
      seg7segA[1] = ( seg7dispA[ x ] & SB ) ? 2 : 0 ;  // '|' 
      seg7segA[2] = ( seg7dispA[ x ] & SC ) ? 2 : 0 ;  // '|' 
      seg7segA[3] = ( seg7dispA[ x ] & SD ) ? 1 : 0 ;  // '_' 
      seg7segA[4] = ( seg7dispA[ x ] & SE ) ? 2 : 0 ;  // '|' 
      seg7segA[5] = ( seg7dispA[ x ] & SF ) ? 2 : 0 ;  // '|' 
      seg7segA[6] = ( seg7dispA[ x ] & SG ) ? 1 : 0 ;  // '_' 
      seg7segA[7] = ( seg7dispA[ x ] & SH ) ? 3 : 0 ;  // '.' 

	  /* print characters to screen */
	  /* top row*/
	  RenderItem(&seg7_font, ( 0            ), (x*4+1) * FONT_W, (FONT_V+0) * FONT_H+4);
	  RenderItem(&seg7_font, ( seg7segA[0]  ), (x*4+2) * FONT_W, (FONT_V+0) * FONT_H+4);
	  RenderItem(&seg7_font, ( 0            ), (x*4+3) * FONT_W, (FONT_V+0) * FONT_H+4);
	  RenderItem(&seg7_font, ( 0            ), (x*4+4) * FONT_W, (FONT_V+0) * FONT_H+4);

	  /* middle row*/
	  RenderItem(&seg7_font, ( seg7segA[5]  ), (x*4+1) * FONT_W, (FONT_V+1) * FONT_H);
	  RenderItem(&seg7_font, ( seg7segA[6]  ), (x*4+2) * FONT_W, (FONT_V+1) * FONT_H+2);
	  RenderItem(&seg7_font, ( seg7segA[1]  ), (x*4+3) * FONT_W, (FONT_V+1) * FONT_H);
	  RenderItem(&seg7_font, ( 0            ), (x*4+4) * FONT_W, (FONT_V+1) * FONT_H);

	  /* bottom row*/
	  RenderItem(&seg7_font, ( seg7segA[4]  ), (x*4+1) * FONT_W, (FONT_V+2) * FONT_H);
	  RenderItem(&seg7_font, ( seg7segA[3]  ), (x*4+2) * FONT_W, (FONT_V+2) * FONT_H+2);
	  RenderItem(&seg7_font, ( seg7segA[2]  ), (x*4+3) * FONT_W, (FONT_V+2) * FONT_H);
	  RenderItem(&seg7_font, ( seg7segA[7]  ), (x*4+4) * FONT_W, (FONT_V+2) * FONT_H);
	}
	
	/* Status outputs*/
	x=8;

	/* Status FLAG outputs connected to LEDs */
	/* This needs futher work SA and SA are inputs,
	 * SIN,SIO needs to be displayed, but not in CPU.C yet 
	 * Status shown in 4x2 grid
	 * 
The status register:-
  7   6  5  4  3  2  1  0
cy/l,OV,SB,SA,IE,F2,F1,F0
Bit Function 	Notes
0 	F0 	Output Line
1 	F1 	Output Line
2 	F2 	Output Line
3 	IE 	Interrupt Enable / HALT
4 	SA 	Input Line + INT,
5 	SB 	Input Line
6 	OV 	Overflow
7 	CY/L Carry / Link bit
	 */
	/* F0 to F3 */ 
	RenderItem(&seg7_font, ( ( Stat & 1 ) ? 1 : 3  ),  (x*4+4) * FONT_W, (FONT_V+0) * FONT_H);
	RenderItem(&seg7_font, ( ( Stat & 2 ) ? 1 : 3  ),  (x*4+4) * FONT_W, (FONT_V+1) * FONT_H);
	RenderItem(&seg7_font, ( ( Stat & 4 ) ? 1 : 3  ),  (x*4+4) * FONT_W, (FONT_V+2) * FONT_H);
    /* halt */ 
	RenderItem(&seg7_font, ( ( Stat & 8 ) ? 1 : 3  ),  (x*4+4) * FONT_W, (FONT_V+3) * FONT_H);


	/* flags */
	RenderItem(&seg7_font, ( ( Stat & 0x10 ) ? 1 : 3  ),  (x*4+6) * FONT_W, (FONT_V+0) * FONT_H);
	RenderItem(&seg7_font, ( ( Stat & 0x20 ) ? 1 : 3  ),  (x*4+6) * FONT_W, (FONT_V+1) * FONT_H);
	RenderItem(&seg7_font, ( ( Stat & 0x40 ) ? 1 : 3  ),  (x*4+6) * FONT_W, (FONT_V+2) * FONT_H);
	RenderItem(&seg7_font, ( ( Stat & 0x80 ) ? 1 : 3  ),  (x*4+6) * FONT_W, (FONT_V+3) * FONT_H);
}



void
DrawChessBoard(SDL_Renderer * renderer)
{
    int row = 0,column = 0,x = 0;
    SDL_Rect rect, darea;

    /* Get the Size of drawing surface */
    SDL_RenderGetViewport(renderer, &darea);

    for( ; row < 8; row++)
    {
        column = row%2;
        x = column;
        for( ; column < 4+(row%2); column++)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

            rect.w = darea.w/8;
            rect.h = darea.h/8;
            rect.x = x * rect.w;
            rect.y = row * rect.h;
            x = x + 2;
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

/**********************************************
 * prototypes
 * ********************************************/

#ifdef comment
void init_seg7(void){
	/* test: set up 7 seg mapping */
	int count;

	seg7hexA[0]  =  SA+SB+SC+SD+SE+SF ;
	seg7hexA[1]  =  SB+SC ;
	seg7hexA[2]  =  SA+SB+SD+SE+SG ;
	seg7hexA[3]  =  SA+SB+SC+SD+SG ;
	seg7hexA[4]  =  SB+SC+SF+SG ;
	seg7hexA[5]  =  SA+SC+SD+SF+SG ;
	seg7hexA[6]  =  SA+SC+SD+SE+SF+SG ;
	seg7hexA[7]  =  SA+SB+SC ;
	seg7hexA[8]  =  SA+SB+SC+SD+SE+SF+SG ;
	seg7hexA[9]  =  SA+SB+SC+SF+SG ;
	seg7hexA[10] =  SA+SB+SC+SE+SF+SG ;
	seg7hexA[11] =  SC+SD+SE+SF+SG ;
	seg7hexA[12] =  SA+SD+SE+SF ;
	seg7hexA[13] =  SB+SC+SD+SE+SG ;
	seg7hexA[14] =  SA+SD+SE+SF+SG ;
	seg7hexA[15] =  SA+SE+SF+SG ;

	for ( count = 0 ; count < DIGITS ; count++ ){
      seg7dispA[count] = seg7hexA[count];
    }
	
}
#endif 

void init_seg7a(void){
	/* test: set up 7 seg mapping */
	int count;

	for ( count = 0 ; count < DIGITS ; count++ ){
      seg7dispA[count] = 0;
    }
	
}

/*
There are 8 DIGITS with 8 segments.

	 set up 7 segment display 
	
	 *    a
	 *  f g b
	 *  e d c
	 *  



*/

#define FONT_HH       15
#define FONT_HV       2
#define FONT_VH       2
#define FONT_VV       15

#define FONT_Y1		( 0  ) 
#define FONT_Y2		( FONT_VV + FONT_VH)
#define FONT_Y3		( FONT_VV *2 + FONT_VH*2)

#define FONT_X1		( 0 )
#define FONT_X2		( FONT_HH + FONT_VH )



void DrawSeg(SDL_Renderer * renderer, int on,int x, int y , int w, int h){
    SDL_Rect rect ;

if ( on != 0 ){ 
    SDL_SetRenderDrawColor(renderer, 0xCf, 0 , 0,  0xFF);
} else {
    SDL_SetRenderDrawColor(renderer, 0x1F, 0,  0,  0xFF);
} 

    rect.w = w;
    rect.h = h;
    rect.x = x ;
//    rect.y = y +  DISPLAY_HEIGHT ;  /* shift height */
    rect.y = y  ;  /* shift height */

    SDL_RenderFillRect(renderer, &rect);

}	
	
void DrawSeg7(SDL_Renderer * renderer, int digit ,int x, int y ){
/*
 *    There are 8 DIGITS with 8 segments.
	 set up 7 segment display 
	
	 *    a
	 *  f g b
	 *  e d c
	 *  
*/

  /* - */
  DrawSeg(renderer, ( digit & SA ), x + FONT_X1 + FONT_VH,  y + FONT_Y1 , FONT_HH, FONT_HV ); /* a */
  DrawSeg(renderer, ( digit & SG ), x + FONT_X1 + FONT_VH,  y + FONT_Y2 , FONT_HH, FONT_HV ); /* g */
  DrawSeg(renderer, ( digit & SD ), x + FONT_X1 + FONT_VH,  y + FONT_Y3 , FONT_HH, FONT_HV ); /* d */

  /* | */
  DrawSeg(renderer, ( digit & SB ), x + FONT_X2 ,  y + FONT_Y1 + FONT_VH, FONT_VH, FONT_VV); /* b */
  DrawSeg(renderer, ( digit & SC ), x + FONT_X2 ,  y + FONT_Y2 + FONT_VH, FONT_VH, FONT_VV); /* c */

  DrawSeg(renderer, ( digit & SF ), x + FONT_X1 ,  y + FONT_Y1 + FONT_VH, FONT_VH, FONT_VV); /* f */
  DrawSeg(renderer, ( digit & SE ), x + FONT_X1 ,  y + FONT_Y2 + FONT_VH, FONT_VH, FONT_VV); /* e */

  /* . */
  DrawSeg(renderer, ( digit & SH ), x + FONT_X2 + FONT_VH,  y + FONT_Y3 + FONT_VH , FONT_HV,FONT_HV); /* decimal point */

}	

void DrawDisplay(SDL_Renderer * renderer, int * digits  ){

  int x ;
  for ( x = 0 ; x < DIGITS ; x ++ ){
	//printf(" %02X \n",  digits[x] );
    DrawSeg7( renderer, digits[x] , x * ( FONT_HH *2 )+FONT_HV, FONT_HV  );
  }
  
  	/* Status outputs*/
	x=8;

	/* Status FLAG outputs connected to LEDs */
	/* This needs futher work SA and SA are inputs,
	 * SIN,SIO needs to be displayed, but not in CPU.C yet 
	 * Status shown in 4x2 grid
	 * 
The status register:-
  7   6  5  4  3  2  1  0
cy/l,OV,SB,SA,IE,F2,F1,F0
Bit Function 	Notes
0 	F0 	Output Line
1 	F1 	Output Line
2 	F2 	Output Line
3 	IE 	Interrupt Enable / HALT
4 	SA 	Input Line + INT,
5 	SB 	Input Line
6 	OV 	Overflow
7 	CY/L Carry / Link bit
	 */

    DrawSeg( renderer, digits[x] , x * ( FONT_HH *2 )+FONT_HV, FONT_HV , FONT_HH, FONT_HV  );

	/* F0 to F3 */ 
	DrawSeg( renderer,  Stat & 1 ,  (x*4+4) * FONT_W, (FONT_V+0) * FONT_H  , FONT_HV, FONT_HV );
	DrawSeg( renderer,  Stat & 2 ,  (x*4+4) * FONT_W, (FONT_V+1) * FONT_H  , FONT_HV, FONT_HV );
	DrawSeg( renderer,  Stat & 4 ,  (x*4+4) * FONT_W, (FONT_V+2) * FONT_H  , FONT_HV, FONT_HV );
    /* halt */ 
	DrawSeg( renderer,  Stat & 8 ,  (x*4+4) * FONT_W, (FONT_V+3) * FONT_H  , FONT_HV, FONT_HV );


	/* flags */
	DrawSeg( renderer,  Stat & 0x10 ,  (x*4+6) * FONT_W, (FONT_V+0) * FONT_H, FONT_HV, FONT_HV );
	DrawSeg( renderer,  Stat & 0x20 ,  (x*4+6) * FONT_W, (FONT_V+1) * FONT_H, FONT_HV, FONT_HV );
	DrawSeg( renderer,  Stat & 0x40 ,  (x*4+6) * FONT_W, (FONT_V+2) * FONT_H, FONT_HV, FONT_HV );
	DrawSeg( renderer,  Stat & 0x80 ,  (x*4+6) * FONT_W, (FONT_V+3) * FONT_H, FONT_HV, FONT_HV );
}


int sim_delay(void)
{
    ui_display_refresh_seg7();

    if (!go_fast)
        SDL_Delay(200);

    return action;
}

static void usage(void)
{
    fprintf(stderr,
 
 "This is Virtual MK14  Usage: %s {flags} files\n"
 "           -v              be verbose\n"
 "           -m 		     use original ROMs \n"
 "           -?              this help\n"
 
            ,progname);
    exit (1);
}


void Render__Font(struct font *font, int idx, int x, int y)
{
    auto SDL_Rect dest = { DISPLAY_X_OFFSET + x, DISPLAY_Y_OFFSET + y,
                           font->w, font->h };


    /* set up rect for teh character of interest */
    SDL_Rect clip = { 0, idx * font->h_pitch, font->w, font->h };
    screen = SDL_GetWindowSurface( window );

/*
 * int SDL_BlitSurface( SDL_Surface*    src,
                    const SDL_Rect* srcrect,
                    SDL_Surface*    dst,
                    SDL_Rect*       dstrect)

int SDL_SetSurfaceBlendMode(SDL_Surface*  surface,
                            SDL_BlendMode blendMode)


*
*/
 
//    SDL_BlitSurface(  font->surf, &clip, screen, &dest);
	SDL_SetSurfaceBlendMode(font->surf, SDL_BLENDMODE_BLEND );

    SDL_BlitSurface(  font->surf, &clip, screen, &dest );

}







int mysetup(int argc, char **argv);

int main(int argc, char **argv)
{
    int c;
    int loop_count;
    int version =0;

    if (mysetup(argc, argv))
        return 1;

    progname = argv[0];

   
    while ((c = getopt(argc, argv, "vm?")) != EOF)
        switch (c) {
        case 'v':
            verbose = 1;
            break;
        case 'm':
            version = 1;
            break;
        case '?':
            usage();
            break;
            
        }

    puts("Virtual MK14  an MK14 emulator version \n"
         "Copyright (C) 2000,2009,2017  Paul Robson + Tommy Thorn + Doug Rice.\n"
         "http://github.com/tommythorn/virtual-nascom.git\n"
         "Uses software from Paul Robson "
         "Virtual MK14 comes with ABSOLUTELY NO WARRANTY; for details\n"
         "see the file \"COPYING\" in the distribution directory.\n"
         "\n"
         "./virtual-mk14 <file.hex>\n"
         "On exit the emulator save the memory state in `ram_.nas`\n"
         "so it can be loaded later on.\n"
         "Status with  F0,F1,F2,IE is displayed on right in 2x4 grid.\n"
         "\n"
         "The following keys are supported:\n"
         "\n"
         "* END- saves 0fXX to ram.nas \n"
         "* F1 - saves 0fXX to ram.nas \n"
         "* F2 - MEM \n"
         "* F3 - GO \n"
         "* F4 - exits the emulator and save to ram_.nas \n"
         "* F5 - toggles between stupidly fast and \"normal\" speed\n"
         "* F9 - resets the emulated Nascom\n"
		"===========\n"
		"Keys to use\n"
		"===========\n"
		"0-9,A-F Hexadecimal Keys\n"
		"T       Term	set mem  \n"
		"M       Mem	increment address \n"
		"G       Go		run from current address\n"
		"Z       Abort	set addr \n"
		"R       Reset\n"
		"/       Quit\n"
         "\n");

  if ( version !=1 ){
	  puts(
		" start without -m to use the revised 0000 00 monitor\n"
		"most programs start 0F20, e.g. type: \n"
		" Z 0f20 T C4 M 07 M 07 M 3f Z 0F20 G \n"
		" Should SET status Flag outputs\n"
		" Z 0f20 T C4 M AA M E4 M 55 M C8 M 02 M 3f Z 0F20 G \n"
		" Should work out AA XOR 55  with -m to useand store it. \n"
      ); } else {
	  puts(
		    
		" start with -m to use the original ---- -- monitor\n"
		"most programs start 0F20, e.g. type: \n"
		" Z M 0f20 T C4 T M T 07 T M T 07 T M T 3f T Z G 0F20 T \n"
		" Should SET status Flag outputs\n"
		" Z M 0f20 T C4 T M T AA T M T E4 T M T 55 T M T C8 T M T 02 T M T 3f T G 0F20 T \n"
		" Should work out AA XOR 55 and store it. \n"
 

         "\n");
      };
      
          
    CONInitialise();
	InitialiseDisplay();
	LoadROM( version );
	ResetCPU();

    for (; optind < argc; optind++){
		/* load hex data */
        load_both_formats(argv[optind]);
    }

    loop_count = 0;
    key_pressed = false;
    while ( (CurrentKeyID != KEY_BREAK) ){
      sim_delay( );
      /* run screen and keyboard  timeslice */
      //ui_display_refresh_seg7();


      ui_serve_input();
      
//sd1      SDL_Flip(screen);
//      SDL_Flip(screen);
//      SDL_UpdateWindowSurface(surface);

      screen  = SDL_GetWindowSurface(window);
//      surface = SDL_GetWindowSurface(window2);
      renderer = SDL_CreateSoftwareRenderer(screen);
//      renderer2 = SDL_CreateSoftwareRenderer(surface);


      SDL_DestroyRenderer(renderer);
//      SDL_DestroyRenderer(renderer2);

      screen  = SDL_GetWindowSurface(window);
//      surface = SDL_GetWindowSurface(window2);

      renderer = SDL_CreateSoftwareRenderer(screen);
//      renderer2 = SDL_CreateSoftwareRenderer(surface);



          /* Clear the rendering surface with the specified color */
      SDL_SetRenderDrawColor(renderer2, 0x0F, 0x0F, 0x0F, 0xFF);
      SDL_RenderClear(renderer2);
	  //DrawChessBoard(renderer);
 	  DrawDisplay( renderer2 , seg7dispA  ); /* draw 7 seg display */
	  SDL_RenderPresent(	renderer2	);    


    /* Got everything on rendering surface,
       now Update the drawing image on window screen */
	  /* update display */	
      SDL_UpdateWindowSurface( window2 );


// using SDL1.2
//      screen = SDL_GetWindowSurface(window);
//      SDL_SetRenderDrawColor(renderer, 0x0F, 0x0F, 0x0F, 0xFF);
//      SDL_RenderClear(renderer);

      ui_display_refresh_seg7();
      SDL_UpdateWindowSurface(window);


      /* run microprocessor timeslice */
      Execute(2048);
      Latency();	// polls keyboard
      if ( CurrentKeyID == KEY_RESET ){
        	ResetCPU();
      }
      
	  loop_count += 1;


	}  
    CONTerminate();
    save_nascom(0xf00, 0xfff, "ram_.nas");

    exit(0);
}



int mysetup(int argc, char **argv)
{
	
	int count;
	
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return 1;
    }

    atexit(SDL_Quit);

	//https://wiki.libsdl.org/MigrationGuide

#ifdef SD1
    screen = SDL_SetVideoMode(DISPLAY_WIDTH, DISPLAY_HEIGHT, 8, SDL_SWSURFACE);

    if (screen == NULL) {
        fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
        return 1;
    }

    /* Set the window caption */
    SDL_WM_SetCaption("MK14", "MK14");


#endif

    /* Create window and renderer for given surface */
//    window = SDL_CreateWindow("MK14 using SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);
    window = SDL_CreateWindow("MK14 using SDL2 and font", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_RESIZABLE);
    if(!window)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window creation fail : %s\n",SDL_GetError());
        return 1;
    }

    window2 = SDL_CreateWindow("MK14 using SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_RESIZABLE);
    if(!window2)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window creation fail : %s\n",SDL_GetError());
        return 1;
    }


//    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION, "MK14","welcome", NULL );

    screen  = SDL_GetWindowSurface(window );
    surface = SDL_GetWindowSurface(window2);

    renderer  = SDL_CreateSoftwareRenderer( screen  );
    renderer2 = SDL_CreateSoftwareRenderer( surface );

//    renderer = SDL_CreateRenderer( window , -1, SDL_RENDERER_PRESENTVSYNC ); /* BUILDS BUT CAUSES SEGMENTATION FAULT */
    

    if(!renderer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Render creation for surface fail : %s\n",SDL_GetError());
        return 1;
    }
    
    
    if(!renderer2)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Render creation for surface fail : %s\n",SDL_GetError());
        return 1;
    }
    
    
	SDL_SetRenderDrawColor(	renderer2, 0xf, 0xf, 0xff, 255);
	SDL_RenderClear(		renderer2	);
	/* do something */
	
	
	sdlTexture = SDL_CreateTexture(renderer2,
       SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,640, 480);
	/* create surface in cpu memory */
	SDL_SetRenderDrawColor(	renderer2, 0xff, 0xff, 0x0f, 255);




	/* render GPU stored memory */
    SDL_RenderCopy(renderer2, sdlTexture, NULL, NULL);
	SDL_RenderPresent(		renderer2	);    

    /* put on screen */
    SDL_UpdateWindowSurface( window2 );  
//    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION, "MK14","welcome", NULL );


	/* invert font - Cannot find SDL function */
    for ( count =0 ; count < sizeof(seg7_font_raw); count ++ ){
		seg7_font_raw[ count ] = ~seg7_font_raw[ count ];
	}

	/* for SDL2 the masks do not seem to work */
    seg7_font.surf =
        SDL_CreateRGBSurfaceFrom(
                seg7_font_raw,               
                8 /* width */,
             8*16 /* height */,
                1 /* depth */,
                1 /* pitch */,
                1 /* Rmask */,
                0 /* Gmask */,
                0 /* Bmask */,
                0 /* Amask */);
    seg7_font.w = FONT_W;
    seg7_font.h = FONT_H;
    seg7_font.h_pitch = FONT_H_PITCH;

    if (!seg7_font.surf) {
        perror("Couldn't load the font\n");
        return 1;
    }
/* how do we convert from black and white to colour? */
//sd1    seg7_font.surf   = SDL_DisplayFormat(seg7_font.surf);

	/* convert font from 1 bit deep to 32 bit deep in two stages */
    //seg7_font.surf   = SDL_ConvertSurfaceFormat( seg7_font.surf,SDL_PIXELFORMAT_INDEX1MSB,0 );   
    seg7_font.surf   = SDL_ConvertSurfaceFormat( seg7_font.surf,SDL_PIXELFORMAT_ABGR8888,0 );
    /* have a green font */
	SDL_SetSurfaceColorMod(seg7_font.surf, 255, 128 , 0 );
	
    return 0;
}




