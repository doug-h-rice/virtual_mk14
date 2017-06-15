# Makefile for Virtual Nascom

# CC must be an C99 compiler
CC=gcc -std=c99 

# full speed or debugging to taste
OPTIMIZE=-O2
#OPTIMIZE=-g
#WARN=-Wmost -Werror
WARN=-Wall -Wno-parentheses
CFLAGS=$(OPTIMIZE) $(WARN) $(shell sdl-config --cflags)

virtual-mk14: cpu.o memory.o virtual-mk14.o
	$(CC) $(CWARN) $(shell sdl-config --libs) $^ -o $@ -lSDL

virtual-nascom: virtual-nascom.o font.o simz80.o ihex.o font_8x8.o 
	$(CC) $(CWARN) $(shell sdl-config --libs) $^ -o $@ -lSDL

clean:
	rm -f *.o *~ core
