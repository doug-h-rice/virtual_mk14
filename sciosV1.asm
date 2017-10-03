; ****************************************************************************
;
;                               The Original SC/MP I/O Monitor.
;
;       Converted to TASM by Colin Phillips (webmaster@mymk14.co.uk)
;       Uses splendid TASM work done by Paul Robson (autismuk@aol.com)
;
; ****************************************************************************

#define High(x)         (((x) >> 8) & 15)
#define Low(x)          ((x) & 255)
#define DatPtr(a)       ((a)+1)
#define DatPtrD(a)	((a)-1)
#define js(x,y)		ldi High(DatPtrD(y))\ xpah x\ ldi Low(DatPtrD(y))\ xpal x\ xppc x

Ram     .equ    0F00h                   ; this is where the standard RAM is
Disp    .equ    0D00h                   ; this is where the display is

; ****************************************************************************
;       Offsets into the data structure , from P2
; ****************************************************************************
dl      .equ    0                       ; Segment for Digit 1
dh      .equ    1                       ; Segment for Digit 2
d3      .equ    2                       ; Segment for Digit 3
d4      .equ    3                       ; Segment for Digit 4
adll    .equ    4                       ; Segment for Digit 5
adlh    .equ    5                       ; Segment for Digit 6
adhl    .equ    6                       ; Segment for Digit 7
adhh    .equ    7                       ; Segment for Digit 8
d9      .equ    8                       ; Segment for Digit 9
cnt     .equ    9                       ; Counter
pushed  .equ    10                      ; Key pushed
char    .equ    11                      ; Char Read
adl     .equ    12                      ; Memory Address (low)
word    .equ    13                      ; Memory Word
adh     .equ    14                      ; Memory Address (High)
ddta    .equ    15                      ; first flag
row     .equ    16                      ; row counter
next    .equ    17                      ; flag for now data
; ****************************************************************************
;       Ram areas used by SCIOS. P3 is saved elsewhere
;       The Data macro is used because of the pre-increment of the PC
;       in the processor isn't supported by TASM.
; ****************************************************************************
p1h     .equ    DatPtr(0FF9h)
p1l     .equ    DatPtr(0FFAh)
p2h     .equ    DatPtr(0FFBh)
p2l     .equ    DatPtr(0FFCh)
a       .equ    DatPtr(0FFDh)
e       .equ    DatPtr(0FFEh)
s       .equ    DatPtr(0FFFh)

; ****************************************************************************
;                               Segment Data
; ****************************************************************************
SA      .equ    1                       ; Segment bit patterns
SB      .equ    2
SC      .equ    4
SD      .equ    8
SE      .equ    16
SF      .equ    32
SG      .equ    64
; ****************************************************************************
;                               Character Segment Maps
; ****************************************************************************
DASH	.equ	SG
KE	.equ	SA+SD+SE+SF+SG
KR	.equ	SE+SG
K0	.equ	SC+SD+SE+SG
; ****************************************************************************
;                       Start of monitor listing
; ****************************************************************************
        .org    1000h                   ; stops TASM complaining...
        nop
        jmp Start			
        				
; ****************************************************************************
;                   Debug exit : restore environment
; ****************************************************************************
GoOut:  ld	adh(2)			; get address
	xpah    3                       ; Save A in P3 (A is Go address High)
        ld      adl(2)
        xpal    3
        ld      @-1(3)                  ; fix go address
        ld      e                       ; restore registers
        xae
        ld      p1l
        xpal    1
        ld      p1h
        xpah    1
        ld      p2l
        xpal    2
        ld      p2h
        xpah    2
        ld      s
        cas
        ld      a
        xppc    3
; ****************************************************************************
;                               Debug Entry Point
; ****************************************************************************
Start:  st      a                       ; Copy all registers to memory
        lde
        st      e
        csa
        st      s
        xpah    1
        st      p1h
        xpal    1
        st      p1l
        ldi     High(Ram)               ; Copy P2, make it point to RAM
        xpah    2                       ; at the same time
        st      p2h
        ldi     Low(Ram)
        xpal    2
        st      p2l
        ld      @1(3)                   ; Bump P3 for return
        xpal    3                       ; save P3
        st      adl(2)
        xpah    3
        st      adh(2)
; ****************************************************************************
;                               Abort
; ****************************************************************************
Abort: 	ldi	0
     	st	d3(2)
     	st	d4(2)
     	nop				; Previously st d9(2) - stop blanking of 
     	nop				; 9th digit to keep page number.
     	ldi	DASH			; set segments to "-"
     	st	dl(2)
     	st	dh(2)
     	st	adll(2)
     	st	adlh(2)
     	st	adhl(2)
     	st	adhh(2)
Wait:	js(3,Kybd)			; display & read keyboard
	jmp	Wck			; command return
	jmp	Abort			; return for number
Wck:	xri	07			; check if mem
	jz	Mem
	xri	01			; check if Go
	jnz	Abort
; ****************************************************************************
;                              Go was pushed
;				goto user programme
; ****************************************************************************
Go:	ldi	-1			; set first flag
	st 	ddta(2)
	ldi	DASH			; set data to dash
	st	dl(2)
	st	dh(2)
Gol:	ldi 	Low(Dispa)-1		; fix address seg
	xpal	3
	xppc	3			; do display and keyboard
	jmp	Gock			; command return
	ldi	Low(adr-1)		; set address
	xpal	3
	xppc	3			
	jmp	Gol			; not done		
Gock:	xri	03			; check for term
	jz 	GoOut			; error if no term
; ****************************************************************************
;                              Incorrect sequence
;				display error, wait for new input
; ****************************************************************************
Error:	ldi	KE			; fill with error        
        st	adhh(2)
        ldi	KR
        st	adhl(2)
        st	adlh(2)
        st	d4(2)
        ldi	K0
        st	adll(2)
        ldi	0
        st	d3(2)
        st	dh(2)
        st	dl(2)
        jmp	Wait
; ****************************************************************************
;                              Memory transactions
; ****************************************************************************
Dtack:	ld	next(2)			; check if data field
	jnz	Data			; address done
Memdn:	ld	adh(2)			; put word in mem
	xpah	1
	ld	adl(2)
	xpal	1
	ld	word(2)
	st	(1)
	jmp	Mem
Memck:	xri	06			; check for go
	jz	Error			; can not go now
	xri	05			; check for term
	jz	Dtack			; check if done
	ild	adl(2)			; update address low
	jnz	Mem			; check if update hi
	ild	adh(2)
;	Mem key pushed
Mem:	ldi	-1			; set first flag
	st	next(2)			; set flag for address now
	st	ddta(2)
Meml:	ld	adh(2)
	xpah	1			; set P1 for mem address
	ld	adl(2)
	xpal	1
	ld	(1)
	st	word(2)			; save mem data
	ldi	Low(dispd)-1		; fix data seg
	xpal	3
	xppc	3			; go to DISPD set seg for data
	jmp	Memck			; command return
	ldi	Low(adr)-1		; make address
	xpal	3
	xppc	3
	jmp	Meml			; get next char
Data:	ldi	-1			; set first flag
	st	ddta(2)
	ld	adh(2)			; set P1 to memory address
	xpah	1
	ld	adl(2)
	xpal	1
	ld	(1)			; read data word
	st	word(2)			; save for display
Datal:	ldi	Low(dispd)-1		; fix data seg
	xpal	3
	xppc	3			; fix data seg-go to dispd
	jmp	Memck			; char return
	ldi	4			; set counter for number of shifts
	st	cnt(2)
	ild	ddta(2)			; check if first
	jnz	Dnfst
	ldi	0			; zero word if first
	st	word(2)
	st	next(2)			; set flag for address done
Dnfst:	ccl
	ld	word(2)			; shift left
	add	word(2)
	st	word(2)
	dld	cnt(2)			; check for 4 shifts
	jnz	Dnfst
	ld	word(2)			; add new data
	ore
	st	word(2)
	jmp	Datal
	
; ****************************************************************************
;                    Hex number to seven segment table
; ****************************************************************************
CRom:   .db     SA+SB+SC+SD+SE+SF
        .db     SB+SC
        .db     SA+SB+SD+SE+SG
        .db     SA+SB+SC+SD+SG
        .db     SB+SC+SF+SG
        .db     SA+SC+SD+SF+SG
        .db     SA+SC+SD+SE+SF+SG
        .db     SA+SB+SC
        .db     SA+SB+SC+SD+SE+SF+SG
        .db     SA+SB+SC+SF+SG
        .db     SA+SB+SC+SE+SF+SG
        .db     SC+SD+SE+SF+SG
        .db     SA+SD+SE+SF
        .db     SB+SC+SD+SE+SG
        .db     SA+SD+SE+SF+SG
        .db     SA+SE+SF+SG
        
; ****************************************************************************
;                    Make 4 digit address
; ****************************************************************************
; shift address left one digit then
; add new low hex digit
; hex digit in E register
; P2 points to RAM
adr:	ldi	4			; set number of shifts
	st	cnt(2)
	ild	ddta(2)			; check if first
	jnz	Notfst			; jmp if no.
	ldi	0			; zero address
	st	adh(2)
	st	adl(2)
Notfst:	ccl				; clear link
	ld	adl(2)			; shift address left 4 times
	add	adl(2)
	st	adl(2)			; save it
	ld	adh(2)			; now shift high
	add	adh(2)
	st	adh(2)
	dld	cnt(2)			; check if shifted 4 times
	jnz	Notfst			; jmp if not done
	ld	adl(2)			; now add new number
	ore
	st	adl(2)			; number is now updated
	xppc	3
	
; ****************************************************************************
;                    Data to Segments
; ****************************************************************************
; convert hex data to segments
; P2 points to RAM
; drops through to hex address conversion
dispd:	ldi	High(CRom)		; set address of table
	xpah	1
	ldi	Low(CRom)
	xpal	1
	ld	word(2)			; get memory word
	ani	$0F
	xae
	ld	-128(1)			; get segment disp
	st	dl(2)			; save at data low
	ld	word(2)			; fix hi
	sr				; shift hi to low
	sr
	sr
	sr
	xae
	ld	-128(1)			; get segments
	st	dh(2)			; save in data hi
	
; ****************************************************************************
;                    Address to Segments
; ****************************************************************************
; convert hex address to segments
; P2 points to RAM
; drops through to keyboard and display
Dispa:	scl
	ldi	High(CRom)		; set address of table
	xpah	1
	ldi	Low(CRom)
	xpal	1
Loopd:	ld	adl(2)			; get address
	ani	$0F
	xae
	ld	-128(1)			; get segments
	st	adll(2)			; save seg of adr ll
	ld	adl(2)
	sr				; shift hi digit to low
	sr
	sr
	sr
	xae
	ld	-128(1)			; get segments
	st	adlh(2)
	csa				; check if done
	ani	$80
	jz	Done
	ccl				; clear flag
	ldi	0
	st	d4(2)			; zero digit 4
	ld	@2(2)			; fix P2 for next loop
	jmp	Loopd
Done:	ld	@-2(2)			; fix P2
; ****************************************************************************
;                    Display and Keyboard Input
; ****************************************************************************
; call xppc 3
; jmp command in A GO=6, MEM=7, TERM=3
; in E GO=22, MEM=23, TERM=27
; number return hex number in E reg
; abort key goes to abort
; all registers are used
; P2 must point to RAM, address must be XXX0
; to re-execute routine do xppc p3
Kybd:	ldi	0			; zero char
	st	char(2)
	ldi	High(Disp)		; set display address
	xpah	1
Off:	ldi	-1			; set row/digit address
	st	row(2)			; save row counter
	ldi	10			; set row count
	st	cnt(2)
	ldi	0
	st	pushed(2)		; zero keyboard input
	xpal	1			; set disp address low
Loop:	ild	row(2)			; update row address
	xae
	ld	-128(2)			; get segment
	st	-128(1)			; send it
	dly	0			; delay for display
	ld	-128(1)			; get keyboard input
	xri	$ff			; check if pushed
	jnz	Key			; jump if pushed
Back:	dld	cnt(2)			; check if done
	jnz	Loop			; no if jump
	ld	pushed(2)		; check if key
	jz	Ckmore
	ld	char(2)			; was there a char?
	jnz	Off			; yes, wait for release
	ld	pushed(2)		; no set char
	st	char(2)
	jmp	Off
Ckmore:	ld	char(2)			; check if there was a char
	jz	Off			; no, keep looking
; ****************************************************************************
;                    Command key processing
; ****************************************************************************
	xae				; save char
	lde				; get char
	ani	$20			; check for command
	jnz	Cmnd			; jump if command
	ldi	$80			; find number
	ane
	jnz	Lt7			; 0 to 7
	ldi	$40
	ane
	jnz	N89			; 8 or 9
	ldi	$0F
	ane
	adi	$07			; make offset to table
	xae				; put offset away
	ld	-128(0)			; get number
Keyrtn:	xae				; save in E
	ld	@2(3)			; fix return
	xppc	3			; return
	jmp	Kybd			; allows xppc P3 to return
	
	.db	$0A
	.db	$0B
	.db 	$0C
	.db	$0D
	.db	$00
	.db	$00
	.db	$0E
	.db	$0F
	
Lt7:	xre				; keep low digit
	jmp	Keyrtn
N89:	xre				; get low
	adi	$08			; make digit 8 or 9
	jmp	Keyrtn
Cmnd:	xre
	xri	$04			; check if abort
	jz	Abrt			; abort
	xppc	3			; in E 23=MEM, 22=GO, 27=TERM
					; in A 7=MEM, 6=GO, 3=TERM
	jmp	Kybd			; allows just a xppc p3 to return
Key:	ore				; make char
	st	pushed(2)		; save char
	jmp	Back
Abrt:	ldi	High(Abort)
	xpah	3
	ldi	Low(Abort)-1
	xpal	3
	xppc	3			; go to abort
	
	.END
	
	
        