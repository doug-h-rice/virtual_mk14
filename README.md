# virtual_mk14

This code emulates a Science Of Cambride MK14.

It is a port of Paul Robson's DOS code and Thommy Thorn's code for the NASCOM 2

You need SDL1.2 installed to build this code.

The MK14 was a very simple microprocessor trainer board. 

It used the National SC/MP microprocessor.

I brought one about 40 years ago.

At the time it was probably the first programable computer I had access to.

This project is a chance to experiment with simulating a microprocessor, display and keyboard.

Paul Robson had a DOS version, and a website from 1998. A copy is at: 

http://mymk14.co.uk/paulRobson/emulator.htm

I have some notes and a JavaScript Version at:
http://ccgi.dougrice.plus.com/cgi-bin/wiki.pl?MK14_Notes

==================================================

Virtual MK14  an MK14 emulator version 
Copyright (C) 2000,2009,2017  Paul Robson + Tommy Thorn + Doug Rice.
http://github.com/tommythorn/virtual-nascom.git

https://github.com/doug-h-rice/virtual_mk14/edit/master/README.md

Uses software from Paul Robson and Tommy Thorn.

Virtual MK14 comes with ABSOLUTELY NO WARRANTY; 

./virtual-mk14 <file.hex>\n"

The emulator dumps the memory state in `memorydump.nas`\n"
upon exit so one might resume execution later on.\n"

Status with  F0,F1,F2,IE is displayed on right in 2x4 grid.\n"

"The following keys are supported:\n"

* END- saves 0fXX to memorysave.nas 
* F1 - saves 0fXX to memorysave.nas 
* F2 - MEM 
* F3 - GO 
* F4 - exits the emulator
* F5 - toggles between stupidly fast and "normal" speed
* F9 - resets the emulated MK14
===========
Keys to use
===========
		0-9,A-F Hexadecimal Keys
		T       Term	set mem  
		M       Mem	increment address 
		G       Go		run from current address
		Z       Abort	set addr 
		R       Reset SC/MP
		/       Quit emulator


mymk14.co.uk supplied the listing of the original ---- -- rom. SCIOS_V1

Start the program with -m to load the ---- -- monitor instead of the more useable monitor ROM.

There is a bug in Paul's original emulator, when executing jumps with displacement of 0x80.

The ---- -- monitor needs the JMP 80 bug to be fixed.

When the displacement is 0x80 instructions use E instead. 

jmps with displacemnt of 0x80 do not use E instead.

This bug has been fixed.

To Run:-
./virtual-mk14      test5.hex
./virtual-mk14 -m   test5.hex

Start without -m to use the revised 0000 00 monitor

most programs start 0F20, e.g. type: 

The keystrokes below should SET status Flag outputs:
Z 0f20 T C4 M 07 M 07 M 3f Z 0F20 G 		

The keystrokes below should work out AA xor 55 and store it in the next location.
  Z 0f20 T C4 M AA M E4 M 55 M C8 M 02 M 3f Z 0F20 G 

		    
Start with -m to use the original ---- -- monitor

most programs start 0F20, e.g. type: 

The keystrokes below should SET status Flag outputs:
  Z M 0f20 T C4 T M T 07 T M T 07 T M T 3f T Z G 0F20 T 

The keystrokes below should work out AA xor 55 and store it in the next location.
  Z M 0f20 T C4 T M T AA T M T E4 T M T 55 T M T C8 T M T 02 T M T 3f T G 0F20 T 

The new monitor uses less keystrokes, which on the MK14 keyboard was welmome.
