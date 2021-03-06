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
