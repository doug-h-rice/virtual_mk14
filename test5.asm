;
; Dabble with SC/MP. 
;
; Author doug rice
;
; usage: ./rcasm test4.asm -l -h -t
;
; use simulator:-
;
; This code puts a constant onto the hex display
;
;

start: 

8060{
;
; Try and write someting to the display at 0d00h
;
;	
Disp:	equ 0d00H
ChRom:	equ 010bH



	org	0f20H
	jmp	loop2
t:	db	0
cnt1:	DB	0
cnt2:	DB	0
keys:	DB	0
lastkeys:	DB	0
t2:	db	0

;  
;  * The Keyboard is also a challenge.
;  * 
;  * =================================
;  * 0D00+   8  7  5  4  3  2  1  0
;  * =================================
;  * bit7    7  6  5  4  3  2  1  0
;  * bit6                      9  8
;  * bit5    T        Z  M  G
;  * bit4    F  E        D  C  B  A
;  * =================================
;  * 0D00+   8  7  5  4  3  2  1  0
;  * =================================
;  * 0D00 is on the right



loop2:
	; reload display pointer.
	LDI	Disp.0
	XPAL	1
	LDI	Disp.1
	XPAH	1

	; read switches and put onto the display
	ldr	keys
	str	lastkeys

	; read switch
	LD	0(1)
	str	keys

	xri	0ffh
	andr	lastkeys
;	xorr	lastkeys
	; 1's represent keys pressed.
	jz	loop2a

	; not zro but which bit

	str	cnt2

	
	xri	080h
	jz	loop2b
	
	dldr	cnt1
	str	cnt1
	jmp	loop2a
	

loop2b:
	ildr	cnt1
	str	cnt1


	
loop2a:	
	; store pressed key to display
	ldr	cnt2
	st	@+1(1)
	dly	10

	; display blank
	ldi	0 
	st	@+1(1)
	dly	10

	; store counter bits to display
	ldr	cnt1
	st	@+1(1)
	dly	10



	; set pointer o Character rom @ 0x010b 
	LDI	ChRom.1
	XPAH	2
	
	; load cnt in xpal and use offset 
	ldr 	cnt1
	ani	0Fh
	XPAL	2
	ld	ChRom.0(2)
	; store onto display
	st	@+1(1)
	dly	10

	; set pointer o Character rom @ 0x010b 
	LDI	ChRom.0
	XPAL	2
	LDI	ChRom.1
	XPAH	2
	

	; display blank
	ldi	0 
	st	@+1(1)
	dly	10


	; store onto display
	ldr	0
	st	@+1(1)
	dly	10

	;nop
	;ldi	055h
	; increment accumulator and store to display

	; display '5'
	ldi	005h
	xae
	;
	; use value in E to lot up 7Seg 
	ld	-128(2)
	; store onto display
	st	@+1(1)
	dly	10

	jmp	loop2


	; display '2'
	ldi	002h
	xae
	
	; use value in E to lot up 7Seg 
	ld	-128(2)
	; store onto display
	st	@+1(1)

	dly	10
	jmp	loop2




	ldi	008h
	xae
	
	; use value in E to lot up 7Seg 
	ld	-128(2)
	; store onto display
	st	@+1(1)

	ldi	000h
	xae
	; use value in E to lot up 7Seg 
	ld	-128(2)
	; store onto display
	st	@+1(1)
	

loop3:	
	jmp	loop2

Stop:
	xppc 3
	jmp stop



}


