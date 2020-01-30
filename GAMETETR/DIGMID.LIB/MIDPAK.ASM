;; MIDPAK.ASM			January 5, 1992, John W. Ratciff
;;
;; This piece of source provides C procedure call hooks down into
;; the resident TSR sound driver.  Use the call CheckIn to find out
;; if the sound driver is in memory.  See the C header file MIDPAK.H
;; for prototype information.
;;
;; This file is in the format for Turbo Assembler's IDEAL mode.  The
;; IDEAL mode syntax makes a lot more sense for 8086 than the old
;; MASM format.  MASM has recently been updated to provide some of the
;; functions that Turbo Assembler has had for a number of years.  I prefer
;; to consider Turbo Assembler the standard for 8086 assemblers.
;; IDEAL mode functionality includes true local labels, real data structures,
;; typecasting, automatic argument passing and local memory.
;; Converting any of this code into MASM format is an excercise left for
;; the student.


	LOCALS			;; Enable local labels

        IDEAL                   ;; Use Turbo Assembler's IDEAL mode
	JUMPS

	; Driver load and unload calls.  Requires that the application provide
	; memory allocation functions and access to DOSCALLS.OBJ.

IFNDEF	LOADABLE_DRIVERS		; If not already defined.
LOADABLE_DRIVERS	equ	1	; Set true to enable
ENDIF

SMALL_MODEL	equ	0   ;: True if declaring C procedures as near.
			; It is false here because all procedures are
			; far, so that you can link any memory model
			; to theme. (They are prototyped as well.)


	INCLUDE "..\digmid.lib\PROLOGUE.MAC"          ;; common prologue


SEGMENT  _TEXT BYTE PUBLIC 'CODE'               ;; Set up _TEXT segment
        ENDS

	ASSUME	CS: _TEXT, DS: _TEXT, SS: NOTHING, ES: NOTHING

IF      LOADABLE_DRIVERS
;; These external procedures are required for the Load and Unload sound
;; driver calls.  The need access to memory allocation functions.
;; The application must provide memory allocation through the functions
;; memfree and memalloc.  The user may redirect these either through DOSCALLS
;; which uses the standard DOS memory allocate, or through their C compiler's
;; memory allocation functions.
	extrn	_memfree:far	 ; Application controlled memory allocation.
	extrn	_memalloc:far	; Application controlled memory allocation.
	extrn	_floadpara:far	    ; File load procedure, found in DOSCALLS!
ENDIF

SEGMENT _TEXT

Macro	CPROC	name		; Macro to establish a C callable procedure.
	public	_&name
IF	SMALL_MODEL
Proc	_&name	near
ELSE
Proc	_&name	far
ENDIF
	endm

;;	int  PlaySequence(int seqnum)
CPROC	PlaySequence
	ARG	DATA:WORD
	PENTER	0

	mov	ax,0702h	; Function #3, DigPlay
	mov	bx,[DATA]
	int	66h

	PLEAVE
	ret
	endp


;;	int  SegueSequence(int seqnum,int activate)
CPROC	SegueSequence
	ARG	SEQ:WORD,ACT:WORD
	PENTER	0

        mov     ax,0703h        ;
	mov	bx,[SEQ]
	mov	cx,[ACT]
	int	66h

	PLEAVE
	ret
	endp


;;	int  RegisterXmidi(char far *midi)
CPROC	RegisterXmidi
	ARG	SEQOFF:WORD,SEQSEG:WORD,LENL:WORD,LENH:WORD
	PENTER	0

	push	di
	push	si

	mov	ax,0704h	; Register XMIDI file.
	mov	bx,[SEQOFF]
	mov	cx,[SEQSEG]
	mov	si,[LENL]
	mov	di,[LENH]	; Length of XMIDI data.
	int	66h

	pop	si
	pop	di


	PLEAVE
	ret
	endp


;;	int  MidiStop(void)
CPROC	MidiStop
	mov	ax,0705h	; Stop playing current sequence.
	int	66h
	ret
	endp

;; long int MidPakClock(void)
CPROC	MidPakClock
	mov	ax,712h
	int	66h
	ret
	endp

;; long int MidPakClock(void)
CPROC	MidPakClockAddress
	mov	ax,712h
	int	66h
	mov	ax,bx		; offset portion of address
	mov	dx,cx		; segment portion of address
	ret
	endp

;;	void RemapChannel(sequence,physical)
CPROC	RemapChannel
	ARG	sequence:word,physical:word
	PENTER	0

	mov	ax,0706h	; Stop playing current sequence.
	mov	bx,[sequence]
	mov	cx,[physical]
	int	66h

	PLEAVE
	ret
	endp




;;	int  CheckMidiIn(void);   // Is sound driver available?
CPROC	CheckMidiIn
	call	CheckMidiIn
	ret
	endp

Proc	CheckMidiIn near
	push	ds		; Save ds register.
	push	si

	mov	si,66h*4h	; get vector number
        xor     ax,ax           ; zero
        mov     ds,ax           ; point it there
	lds	si,[ds:si]	; get address of interupt vector
        or      si,si           ; zero?
        jz      @@CIOUT         ; exit if zero
        sub     si,6            ; point back to identifier

	cmp	[word si],'IM'  ; Midi driver?
	jne	@@CIOUT
	cmp	[word si+2],'ID'  ; full midi driver identity string?
	jne	@@CIOUT
	mov	ax,1
@@EXT:
	pop	si
	pop	ds
	ret
@@CIOUT: xor	ax,ax		; Zero return code.
	jmp short @@EXT
	endp

CPROC	DigPakAvailable
	mov	ax,701h
	int	66h
	ret
	endp

CPROC	ReportCallbackTrigger
	mov	ax,707h
	int	66h
	ret
	endp

CPROC	ResetCallbackCounter
	mov	ax,708h
	int	66h
	ret
	endp

CPROC	MidiSleep
	mov	ax,709h
	int	66h
	ret
	endp

CPROC	MidiAwake
	mov	ax,70Ah
	int	66h
	ret
	endp

CPROC	ResumePlaying
	mov	ax,70Bh
	int	66h
	ret
	endp

CPROC	SequenceStatus
	mov	ax,70Ch
	int	66h
	ret
	endp

CPROC	RelativeVolume
	mov	ax,70Eh
	int	66h
	ret
	endp

CPROC	SetRelativeVolume
	ARG	VOL:WORD,TIME:WORD
	PENTER	0
	mov	ax,70Fh
	mov	bx,[VOL]
	mov	cx,[TIME]
	int	66h
	PLEAVE
	ret
	endp

CPROC	RegisterXmidiFile
	ARG	FNAMEL:WORD,FNAMEH:WORD
	PENTER	0

	mov	ax,70Dh
	mov	bx,[FNAMEL]
	mov	cx,[FNAMEH]
	int	66h

	PLEAVE
	ret
	endp

CPROC	PollMidPak
	mov	ax,0711h
	int	66h
	ret
	endp

CPROC	TriggerCountAddress
	mov	ax,713h
	int	66h
	ret
	endp

CPROC	EventIDAddress
	mov	ax,714h
	int	66h
	ret
	endp

CPROC	ReportSequenceNumber
	mov	ax,716h
	int	66h
	ret
	endp



IF	LOADABLE_DRIVERS
LABEL	InstallDriver	DWORD
InstOff dw	0200h	; Offset of first jump.
InstSeg dw	?	; Segment of audio driver.
LABEL	DeInstallDriver DWORD
DeInstOff dw	0203h	; Offset of deinstall jump
DeInstSeg dw	?	; Segment of audio driver.

CPROC	DeInitMP
	ARG	MIDSEG:WORD
	PENTER	0

	mov	ax,[MIDSEG]
	sub	ax,10h
	mov	[cs:DeInstSeg],ax
	call	[cs:DeInstallDriver]	   ; Do indirect call to deinstall the driver.

	PLEAVE
	ret
	endp

CPROC	InitMP
	ARG	MIDSEG:WORD,ADV:WORD,ADL:WORD,ADH:WORD
	PENTER	0
	PushCREGS

	mov	ax,[MIDSEG] ; Save segment loaded at.
	mov	es,ax		; Into ES
	sub	ax,10h		; Less 10 paragraphs for the org 100h
;; Check Identity string 'MIDPAK' if not located starting 3 bytes into
;; the file loaded, then this is not a compatible digitized sound driver.
	mov	cx,ax
	mov	ax,9999 	; Not valid driver.
	cmp	[byte es:3],'M' ; M in MIDPAK?
	jne	@@OUT
	cmp	[byte es:4],'I' ; I in MIDPAK?
	jne	@@OUT
	cmp	[byte es:5],'D' ; D in MIDPAK?
	jne	@@OUT
	cmp	[byte es:6],'P' ; P in MIDPAK?
	jne	@@OUT
	cmp	[byte es:7],'A' ; A in MIDPAK?
	jne	@@OUT
	cmp	[byte es:8],'K' ; K in MIDPAK?
	jne	@@OUT
	mov	[cs:InstSeg],cx ; set jmp segment address.
	call	[cs:InstallDriver]	; Install the driver.
	or	ax,ax		; Installed ok?
	jnz	@@OUT
	mov	bx,[ADV]       ; Segment portion of .ADV file
	xor	cx,cx		; offset portion must be ZERO!
	mov	dx,[ADH]	; Segment portion of .AD file.
	mov	si,[ADL]	; Offset portion of .AD file.
	mov	ax,710h
	int	66h
@@OUT:
	PopCREGS
	PLEAVE
	ret
	endp

ENDIF



	ends
	end

