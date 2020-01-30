
	LOCALS			;; Enable local labels

        IDEAL                   ;; Use Turbo Assembler's IDEAL mode
	JUMPS

SMALL_MODEL	equ	0   	;: true only if trying to generate near
			    	;; procedure calls, note that all C function
			    	;; prototypes are currently set as far so
			    	;; that the C compiler will link correctly
			    	;; regardless of memory model, so there is
			    	;; no need to generate near calls unless you
			    	;; are trying to write an executable without
			    	;; any segment relocatable information such as
			    	;; a COM file.


	INCLUDE "..\digmid.lib\PROLOGUE.MAC"  ;; common assembly language prologue

SEGMENT  _TEXT BYTE PUBLIC 'CODE'	;; Set up _TEXT segment
        ENDS

	ASSUME	CS: _TEXT, DS: _TEXT, SS: NOTHING, ES: NOTHING

	extrn	_memfree:far	; Application controlled memory allocation.
	extrn	_memalloc:far	; Application controlled memory allocation.


SEGMENT _TEXT

Macro	CPROC	name
	public	_&name
IF	SMALL_MODEL
Proc	_&name	near
ELSE
Proc	_&name	far
ENDIF
	endm



	CPROC	mfopen
	ARG	FNAME:DWORD,RSIZE:DWORD,FTYPE:WORD
; usage: int mfopen(char *filename,    Name of the file to open. +4
;                   long int *size,    Returned size of file     +6
;                    int type);        Open type.                +8
; type --- 0 - open, create new.
; type --- 1 - open, without creating.
; returns file handle.  A file handle of zero is an error condition.
        push    bp
        mov     bp,sp
	push	ds
        push    si
        push    di

	lds	dx,[FNAME]
	mov	ax,[FTYPE]	; get open type requested.
        or      al,al
        jnz     opold
; Here we create the file as requested.
        xor     cx,cx
        mov     ah,03Ch         ; create handle function.
        int     21h             ; perform function
        jc      ferr            ; if carry clear set then return error.
        jnc     fop             ; ALWAYS-> branch to handle return.
opold:  mov     al,2            ; open with read and write access.
        mov     ah,03Dh         ; open handle for read and write access.
        int     21h             ; do file open.
        jc      ferr            ; file not found, return error.
fop:    mov     bx,ax           ; put file handle in BX.
        mov     ah,42h          ; function 42h move file pointer
        mov     al,2            ; offset from end of file
        xor     cx,cx           ; zero bytes from
        xor     dx,dx           ; end of the file
        int     21h             ; perform move file pointer
        jc      ferr            ; file error with a close
	lds	di,[RSIZE]	; pointer to long int
	or	di,di		; Valid pointer?
	jnz	@@OK1
	mov	cx,ds		; Get DS register.
	jcxz	@@SKIP		; if null pointer, skip
@@OK1:	mov	[di],ax 	; save low word of file size
        mov     [di+2],dx       ; save high word of file size
@@SKIP: mov	ah,42h		; function 42h move file pointer
	xor	al,al		; offset from beginning.
        xor     cx,cx           ; zero bytes from beginning
        xor     dx,dx           ; of the file
        int     21h             ; perform move file pointer
        jc      ferr            ; file error with a close
        mov     ax,bx           ; return file handle.
        pop     di
        pop     si
	pop	ds
        pop     bp              ; restore base pointer.
        ret
ferr:   xor     ax,ax           ; handle of zero is an error condition.
        pop     di
        pop     si
	pop	ds
        pop     bp
        ret
	endp

	CPROC	mfclose
	ARG	FHAND:WORD
; usage: int mfclose(int fhand)  Closes this files.
         push   bp
         mov    bp,sp
         mov    ah,3Eh          ; function for close handle.
	 mov	bx,[FHAND]	; get handle into BX
         int    21h             ; perform file close.
         jc     clerr           ; if carry set then a close error.
         mov    ax,1            ; return code of one is success code.
         pop    bp
         ret
clerr:  xor    ax,ax            ; return code of zero is an error on close.
        pop     bp
        ret
	endp

	CPROC	mfpos
	ARG	FHAND:WORD
; Usage: long int mfpos(int fhand)
; Reports current file position.
        push    bp
        mov     bp,sp

	mov	bx,[FHAND]	; get file handle.
        mov     ah,42h          ; move file pointer.
        mov     al,1            ; from current pointer position.
        xor     cx,cx           ; offset of zero from current location.
        xor     dx,dx           ; high word.
        int     21h             ; do dos

        pop     bp
        ret
	endp

	CPROC	mfseek
	ARG	FHAND:WORD,FPOSL:WORD,FPOSH:WORD
; usage: mfseek(int fhand,long int fpos)
; sets current file position, returns, in the same location, the actual
; file position.
        push    bp
        mov     bp,sp

	mov	bx,[FHAND]	; get file handle.
        mov     ah,42h          ; move file pointer.
        mov     al,0            ; from beginning of the file.

	mov	dx,[FPOSL]	; get low word of file pos.
	mov	cx,[FPOSH]	; get high word of file pos.
        int     21h             ; do dos
        jc      mfserr          ; move file position error.

        pop     bp
        ret
mfserr:
	xor	ax,ax		; zero out return code.
	xor	dx,dx

        pop     bp
        ret
	endp

	CPROC	mfread
	ARG	READADR:DWORD,SIZEL:WORD,SIZEH:WORD,FHAND:WORD
; usage: int mfread(char far *read,long int size,int fhand);
        push    bp
        mov     bp,sp

        push    si
        push    di
	mov	bx,[FHAND]	 ; get file handle.
        push    ds               ; save data segment

	lds	dx,[READADR]

doit2:  mov    cx,32768          ; block size for reads.
	cmp    [SIZEH],0	 ; is high word non-zero?
        jne    doread            ; yes->do read
	cmp    cx,[SIZEL]	 ; more than block bytes to read?
        jbe    doread            ; go do read
	mov    cx,[SIZEL]	 ; read last portion
doread: mov   ah,3Fh             ; read command
        int    21h               ; do read
        jc     ferr2             ; file error
        cmp    cx,ax             ; read all the bytes in?
        jne    ferr2             ; file error
	sub    [SIZEL],cx	 ; subtract
	sbb    [SIZEH],0	 ; high word
        mov    ax,cx             ; save count bytes written
        and    ax,0Fh            ; leave low nibble
        add    dx,ax             ; add to offset
        shr    cx,1              ; /2
        shr    cx,1              ; /4
        shr    cx,1              ; /8
        shr    cx,1              ; /16
        mov    ax,ds             ; get segment
        add    ax,cx             ; add paragraphs along
        mov    ds,ax             ; place in data segment
	cmp    [SIZEL],0	 ; done all?
        jne    doit2             ; non-zero more to do
	cmp    [SIZEH],0	 ; done all?
	jne    doit2		 ; non-zero more to do
	mov	ax,1		 ; return success code.

        pop     ds               ; get back data segment
        pop     di
        pop     si
        pop     bp
        ret
ferr2:  xor     ax,ax            ; return error code on read
        pop     ds
        pop     di
        pop     si
        pop     bp
        ret
	endp

	CPROC	mfwrite
	ARG	WADR:DWORD,SIZEL:WORD,SIZEH:WORD,FHAND:WORD
; usage: int mfwrite(int seg,int offset,long int size,int fhand)
        push    bp
        mov     bp,sp

        push    si
        push    di
	mov	bx,[FHAND]	; get file handle.
        push    ds              ; save data segment

	lds	dx,[WADR]	; write address.

doit:   mov    cx,32768         ; block size for writes
	cmp    [SIZEH],0	; is high word non-zero?
        jne    dowrite          ; yes->do write
	cmp    cx,[SIZEL]	; more than block bytes to write?
        jbe    dowrite          ; go do write
	mov    cx,[SIZEL]	; write last portion
dowrite: mov   ah,40h           ; write command
        int    21h              ; do write
        jc     ferr1            ; file error
        cmp    cx,ax            ; write all the bytes out?
        jne    ferr1            ; file error
	sub    [SIZEL],cx	; subtract
	sbb    [SIZEH],0	; high word
        mov    ax,cx            ; save count bytes written
        and    ax,0Fh           ; leave low nibble
        add    dx,ax            ; add to offset
        shr    cx,1             ; /2
        shr    cx,1             ; /4
        shr    cx,1             ; /8
        shr    cx,1             ; /16
        mov    ax,ds            ; get segment
        add    ax,cx            ; add paragraphs along
        mov    ds,ax            ; place in data segment
	cmp    [SIZEL],0	; done all?
        jne    doit             ; non-zero more to do
	cmp    [SIZEH],0	; done all?
        jne    doit             ; non-zero more to do
        mov     ax,1            ; return success code.
        pop     ds              ; get back data segment
        pop     di
        pop     si
        pop     bp
        ret
ferr1:  xor     ax,ax           ; return error code on write.
        pop     ds
        pop     di
        pop     si
        pop     bp
        ret
	endp


	CPROC	fload
	ARG	FNAME:DWORD,RSIZE:DWORD
; This routine is called as:
; char far *fload(char *filename,long int *size)
; where filename is the name of the file you wish to load
; and size will contain the size of the file in bytes.
; This routine returns the segment in memory of where this file
; was read into.  It uses a dos memory allocate to decide where to
; read the file in.
        push    bp
        mov     bp,sp
        push    di
        push    si
	push	ds
	mov	si,ds		 ; Save DS passed.

	lds	dx,[FNAME]
        xor     al,al            ; access code of zero
        mov     ah,03Dh          ; function 3D file open
        int     21h              ; perform function
	jc	@@ferr0 	 ; if carry set then an error
        mov     bx,ax            ; place file handle in bx
        mov     ah,42h           ; function 42h move file pointer
        mov     al,2             ; offset from end of file
        xor     cx,cx            ; zero bytes from
        xor     dx,dx            ; end of the file
        int     21h              ; perform move file pointer
	jc	@@ferr1 	 ; file error with a close
	lds	di,[RSIZE]
	or	di,di
	jz	@@SKIP
	mov	[di],ax 	 ; save low word of file size
	mov	[di+2],dx	 ; save high word of file size
@@SKIP: mov	ds,si		 ; DS=segment on entry.
	mov	si,bx		 ; Save file handle
	push	dx
	push	ax
	call	_memalloc	 ; Allocate memory for it.
	add	sp,4
	mov	bx,si		 ; get back file handle
	or	ax,ax		 ; Offset portion needs to be zero!
	jnz	@@ok2		 ; Exit if offset portion is non-zero.
	or	dx,dx		 ; Failure to allocate memory?
	jz	@@ferr1 	 ; file error with close
@@ok2:	mov	di,ax		 ; Save offset portion of address.
	mov	si,dx		 ; place segment in si
        mov     ah,42h           ; move file pointer
        xor     al,al            ; to beginning of file
        xor     cx,cx            ; zero begin
        xor     dx,dx            ; zero begin
        int     21h              ; move file pointer to begining
        mov     ds,si            ; get segment of memory allocate
        mov     cx,32768         ; read in 32768 bytes
	mov	dx,di		 ; byte offset zero from allocated segment
readin: mov     ah,3Fh           ; file read handle
        int     21h              ; perform the read
        cmp     ax,cx            ; read in whole amount?
        jne     readon           ; done entire read
        mov     ax,ds            ; get buffer segment
        add     ax,2048          ; add 32768 bytes
        mov     ds,ax            ; place back in data segment
        jmp short readin         ; read in next block
readon: mov	ah,3eh		 ; close handle
        int     21h              ; perform close
	mov	dx,si		 ; get back segment
	mov	ax,di		 ; offset portion
	pop	ds
        pop     si
        pop     di
        pop     bp               ; restore base pointer
        ret
@@ferr1:  mov	  ah,3eh	 ; close handle
        int     21h              ; perform close
@@ferr0:  xor	  ax,ax
	xor	dx,dx		
	pop	ds
        pop     si
        pop     di
        pop     bp
        ret
	endp


;; Load a file into allocated memory, but force it to be on a paragraph
;; boundary.
	CPROC	floadpara
	ARG	FNAME:DWORD,RSIZE:DWORD,SEGRET:DWORD
; This routine is called as:
; char far *fload(char *filename,long int *size)
; where filename is the name of the file you wish to load
; and size will contain the size of the file in bytes.
; This routine returns the segment in memory of where this file
; was read into.  It uses a dos memory allocate to decide where to
; read the file in.
        push    bp
        mov     bp,sp
        push    di
        push    si
	push	ds
	mov	si,ds		 ; Save DS passed.

	lds	dx,[FNAME]
        xor     al,al            ; access code of zero
        mov     ah,03Dh          ; function 3D file open
        int     21h              ; perform function
	jc	@@ferr0 	   ; if carry set then an error
        mov     bx,ax            ; place file handle in bx
        mov     ah,42h           ; function 42h move file pointer
        mov     al,2             ; offset from end of file
        xor     cx,cx            ; zero bytes from
        xor     dx,dx            ; end of the file
        int     21h              ; perform move file pointer
	jc	@@ferr1 	   ; file error with a close
	lds	di,[RSIZE]
	or	di,di
	jz	@@SKIP
	mov	[di],ax 	; save low word of file size
	mov	[di+2],dx	; save high word of file size
@@SKIP: mov	ds,si		; DS=segment on entry.
	mov	si,bx		; Save file handle
	add	ax,16
	adc	dx,0		; Extra 16 bytes.
	push	dx
	push	ax
	call	_memalloc	; Allocate memory for it.
	add	sp,4
	mov	bx,si		; get back file handle
	or	ax,ax		; Offset portion needs to be zero!
	jnz	@@ok2		; Exit if offset portion is non-zero.
	or	dx,dx		; Failure to allocate memory?
	jz	@@ferr1 	; file error with close
@@ok2:	mov	di,ax		; Save offset portion of address.
	mov	si,dx		 ; place segment in si
        mov     ah,42h           ; move file pointer
        xor     al,al            ; to beginning of file
        xor     cx,cx            ; zero begin
        xor     dx,dx            ; zero begin
        int     21h              ; move file pointer to begining
        mov     ds,si            ; get segment of memory allocate
        mov     cx,32768         ; read in 32768 bytes
	mov	dx,di		 ; byte offset zero from allocated segment
;; Get byte offset, but now round it up to the next paragraph boundary.
	add	dx,15		; Round up.
	and	dx,0FFF0h	; Strip off bottom 4 bits.
@@r1:	mov	ah,3Fh		 ; file read handle
        int     21h              ; perform the read
        cmp     ax,cx            ; read in whole amount?
	jne	@@r2		 ; done entire read
        mov     ax,ds            ; get buffer segment
        add     ax,2048          ; add 32768 bytes
        mov     ds,ax            ; place back in data segment
	jmp short @@r1		 ; read in next block
@@r2:	mov	ah,3eh		 ; close handle
        int     21h              ; perform close
	mov	dx,si		 ; get back segment
	mov	ax,di		 ; offset portion
	add	ax,15		; Round up to closest paragraph.
	ShiftR	ax,4		; compute segment portion.
	add	ax,dx		; Plus segment portion of far address.
	lds	bx,[SEGRET]	; Get address to return segment portion of address.
	mov	[ds:bx],ax	; Return segment load.
	mov	dx,si		 ; get back segment
	mov	ax,di		 ; offset portion

	pop	ds
        pop     si
        pop     di
        pop     bp               ; restore base pointer
        ret
@@ferr1:  mov	  ah,3eh	   ; close handle
        int     21h              ; perform close
@@ferr0:  xor	  ax,ax
	xor	dx,dx		;
	pop	ds
        pop     si
        pop     di
        pop     bp
        ret
	endp



	CPROC	keystat
        mov     ah,01h
        int     16h
        jnz     IsKey
        xor     ax,ax
        ret
IsKey:  mov     ax,1
        ret
	endp


	CPROC	getkey
        mov     ah,07h
        int     21h
        xor     ah,ah
	or	al,al
	jnz	@@RET
	mov	ah,07h
	int	21h
	xor	ah,ah
	add	ax,256
@@RET:
        ret
	endp


	CPROC	farcop
	ARG	DEST:DWORD,FROM:DWORD
	push	bp
	mov	bp,sp
	push	es
	push	ds
	push	si
	push	di

	les	di,[DEST]
	lds	si,[FROM]
@@MOV:	lodsb
	stosb
	or	al,al
	jnz	@@MOV

	pop	di
	pop	si
	pop	ds
	pop	es
	pop	bp
	ret
	endp


	CPROC	farcat
	ARG	DEST:DWORD,FROM:DWORD
	push	bp
	mov	bp,sp
	push	es
	push	ds
	push	si
	push	di

	les	di,[DEST]
	xor	ax,ax
	mov	cx,-1
	repnz	scasb
	dec	di
	lds	si,[FROM]
@@MOV2:  lodsb
	stosb
	or	al,al
	jnz	@@MOV2

	pop	di
	pop	si
	pop	ds
	pop	es
	pop	bp
	ret
	endp


	CPROC	fmalloc
	ARG	SIZEL:WORD,SIZEH:WORD
; MEMALLOC(SIZE IN PARAGRAPHS)
; This routine allocates the amount of memory requested in the first
; argument.  If the memory could not be allocated then MEMALLOC returns
; a -1 else it returns the segment of the allocated memory
        push    bp
        mov     bp,sp

	mov	ax,[SIZEL]	; Get low word.
	mov	dx,[SIZEH]	; Get high word, get size, long word.

	sar	dx,1
	rcr	ax,1		; /2

	sar	dx,1
	rcr	ax,1		; /4

	sar	dx,1
	rcr	ax,1		; /8

	sar	dx,1
	rcr	ax,1		; /16

	or	dx,dx
	jnz	@@ERR

	mov	bx,ax		; Paragraphs.
	inc	bx		; Plus 1 to correct for roundoff
        mov     ah,48h          ; request for memory allocate
        int     21h
	jnc	@@mall1 	  ; memory was allocated
@@ERR:	xor	ax,ax		; error return code!
@@mall1:  mov	  dx,ax
	xor	ax,ax		; Far ptr format
	pop	bp
        ret
	endp



	CPROC	ffree
	ARG	FREEL:WORD,FREEH:WORD
; FREEMEM(Segement to free)
; This routine frees previously allocated memory block
; -1 is an error code
        push    bp
        mov     bp,sp
        push    es
	mov	bx,[FREEH]	; Get segment portion of address
        mov     es,bx           ; segment to free
        mov     ah,49h
        int     21h
        jnc     fmem1
        mov     ax,-1
fmem1:  pop     es
        pop     bp
        ret
	ENDP


	CPROC	writeln
	ARG	STRING:DWORD
; called as writeln(char *string)
; prints the string specifed to the standard output device
        push    bp         ; save base pointer
        mov     bp,sp      ; place bp in sp
	push	ds
        push    si
	lds	si,[STRING]
        mov     bl,15      ; print in White
write:  lodsb              ; get character
	or	al,al	   ; EOS?
	jz	writdon    ; done with write
        cmp     al,10      ; is it a line feed?
        jne     nex1       ; go compare next item
        mov     ah,0Eh     ; write out the line feed
        int     10h        ; now write out
        mov     al,13      ; a carriage return
nex1:   mov     ah,0Eh     ; write a character function
        int     10h        ; write the character
        jmp short write    ; keep on writing
writdon: pop    si
	pop	ds
        pop    bp         ; restore base pointer
        ret
	endp

	CPROC	GetTimerInterruptVector
	push	ds
	xor	ax,ax
	mov	ds,ax
	mov	ax,[ds:8*4]
	mov	dx,[ds:8*4+2]
	pop	ds
	ret
	endp

command_reg     equ 43h
channel_0       equ 40h
channel_2       equ 42h         ; speaker's frequency oscillator.

	CPROC	SetTimerInterruptVector
	ARG	ADDRESSL:WORD,ADDRESSH:WORD,DIVISOR:WORD
	PENTER	0

	cli		; Turn off hardware Interrupts while modifiying vectory.
	push	ds
	xor	ax,ax
	mov	ds,ax
	mov	ax,[ADDRESSL]
	mov	[ds:8*4],ax
	mov	ax,[ADDRESSH]
	mov	[ds:8*4+2],ax
	mov	dx,[DIVISOR]	; Get divisor rate.

        mov     al,00110110b
        out     command_reg,al
        jmp     $+2
	mov	ax,dx		; Get rate into AX
        out     channel_0,al
        jmp     $+2
        mov     al,ah
        out     channel_0,al
	sti			; Restart hardware Interrupts

	pop	ds
	PLEAVE
	ret
	endp


CPROC	mdelete
	ARG	FNAME:DWORD
; usage: int mdelete(char far *fname)
; return code 1, deleted.
;	      0, error occured.
	PENTER	0
	push	ds

	lds	dx,[FNAME]
	mov	ah,41h
	int	21h
	jnc	mdok
	xor	ax,ax
	jmp short @@EXT
mdok:	mov	ax,1
@@EXT:
	pop	ds
	PLEAVE
	ret
	endp


	CPROC	farlen
	ARG	STRING:DWORD
	PENTER	0
	push	es
	push	di

	les	di,[STRING]
	xor	ax,ax
	mov	cx,-1
	repnz	scasb
	not	cx
	mov	ax,cx
	dec	ax		; Less one

	pop	di
	pop	es
	PLEAVE
	ret
	endp

	CPROC	ucase
	ARG	STRING:DWORD
	PENTER	0
	PushCREGS

	xor	ax,ax
	lds	si,[STRING]
	push	ds
	pop	es
	mov	di,si			; Dest address is the same.
	mov	cx,-1
	xor	ax,ax
	repnz	scasb
	not	cx
	mov	di,si
@@LOD:  lodsb                           ; Get byte from source.
        cmp     al,'a'                  ;
        jl      @@STO                   ; char < 'A' ... continue to next char
        cmp     al,'z'                  ;
        jg      @@STO                   ; char > 'Z' ... continue to next char
        sub     al,'a'-'A'              ; upper case it
@@STO:  stosb                           ; Store it
	loop	@@LOD

	PopCREGS
	PLEAVE
	ret
	endp

	CPROC	farcompare
	ARG	STR1:DWORD,STR2:DWORD
	PENTER	0
	PushCREGS


	lds	si,[STR1]
	les	di,[STR2]
	push	di
	mov	cx,-1
	xor	ax,ax		; zero byte
	repnz	scasb		; Search for EOS string 2
	not	cx		; Get length.
	pop	di
	rep	cmpsb		; compare strings.
	xor	bx,bx		; Zero bx.
	mov	al,[ds:si-1]	; Get ending byte.
	mov	bl,[es:di-1]	; Ending byte.
	sub	ax,bx		; Return code.

	PopCREGS
	PLEAVE
	ret
	endp

	CPROC	ifexists
	ARG	FNAME:DWORD
	PENTER	0
	push	ds

	lds	dx,[FNAME]
	xor	al,al
	mov	ah,03Dh
	int	21h
	jc	@@NOP
	mov	bx,ax
	mov	ah,3Eh
	int	21h
	mov	ax,1		; File found
@@LEAVE:
	pop	ds
	PLEAVE
	ret
@@NOP:	xor	ax,ax		; File not found.
	jmp short @@LEAVE
	endp

CPROC	tprint
	ARG	XLOC:WORD,YLOC:WORD,LN:WORD,STRING:DWORD,COLOR:WORD
	PENTER	0
	PushCREGS

	lds	si,[STRING]	   ;; Get base address of text to print.
	cmp	[LN],0		;; If length passed is zero, then use string length.
	jne	@@LNPASS	;; use length passed.
	or	si,si		;; Null string?
	jz	@@EXIT		;; yes, exit.
	mov	di,si
	push	ds
	pop	es
	xor	ax,ax
	mov	cx,-1
	repnz	scasb
	neg	cx
	dec	cx		; Less one.
	mov	[LN],cx 	; Length of string.
@@LNPASS:
;; If ylocation above are below clip window, exit.
	mov	ax,[YLOC]		;; Get destination ylocation.
	or	ax,ax			;; < top margin?
	js	@@EXIT			 ;; yes, doesn't fit.
	cmp	ax,25			;; > bottom margin?
	jg	@@EXIT			 ;; yes, doesn't fit.

;; If xlocation > right margin? yes->exit.
	mov	ax,[XLOC]		;; Get xlocation passed?
	cmp	ax,79			;; > right margin?
	jg	@@EXIT			;; doesn't fit, exit.
	or	ax,ax
	jns	@@SKP1			;; no, start postion is ok.
	neg	ax			;; Make it positive.
	or	si,si			;; Null string?
	jz	@@REN
	add	si,ax			;; Number of pixels to skip.
@@REN:	sub	[LN],ax 		;; Differnece, from total length.
	js	@@EXIT
	jz	@@EXIT			 ;; If length zerod out, get out.
	mov	[XLOC],0		;; Set it.
@@SKP1:
	add	ax,[LN] 		;; Add total length.
	dec	ax			;; Less one to ending pixel position.
	cmp	ax,79			;; Past right margin?
	jle	@@SKIP			;; no->it fits fine.
	sub	ax,79			;; Amount past right margin.
	sub	[LN],ax 		;; Lower total length.
	js	@@EXIT			;; if negitze, no go.
@@SKIP:
	mov	ax,0B800h
	mov	es,ax
	mov	ax,160
	mul	[YLOC]
	mov	di,ax
	mov	ax,[XLOC]
	shl	ax,1
	add	di,ax

	mov	ax,[COLOR]
	mov	ah,al			;; Into AH.
	mov	cx,[LN] 		;; Length.
	or	si,si
	jz	@@SPACE 		;; Fill rest with spaces
@@SND:	lodsb				;; Get character to send.
	or	al,al			;; Is it eos?
	jz	@@SPACE 		;; fill rest with spaces.
	stosw
	loop	@@SND
	jmp	@@EXIT
@@SPACE:mov	al,20h			;; Space character.
@@FILL: rep	stosw			;; Fill rest with spaces.


@@EXIT:
	PopCREGS
	PLEAVE
	ret
	endp

CPROC	tcolor
	ARG	XLOC:WORD,YLOC:WORD,LEN:WORD,COLOR:WORD
	PENTER	0
	PushCREGS

	mov	ax,160
	mul	[YLOC]
	mov	di,ax
	mov	ax,[XLOC]
	shl	ax,1
	add	di,ax
	mov	cx,[LEN]
	mov	ax,0B800h
	mov	es,ax
	mov	ax,[COLOR]
	inc	di		; Up to attribute byte
@@SND:	stosb
	inc	di
	loop	@@SND

	PopCREGS
	PLEAVE
	ret
	endp

CPROC	TextCursor
	ARG	XLOC:WORD,YLOC:WORD
	PENTER	0

	mov	ax,[YLOC]
	mov	dx,[XLOC]
	mov	dh,al
	mov	ah,2
	mov	bh,0
	int	10h

	PLEAVE
	ret
	endp


	CPROC	farmov
	ARG	DEST:DWORD,FROM:DWORD,MLEN:WORD
	push	bp
	mov	bp,sp
	push	es
	push	ds
	push	si
	push	di

	les	di,[DEST]
	lds	si,[FROM]
	mov	cx,[MLEN]
	rep	movsb

	pop	di
	pop	si
	pop	ds
	pop	es
	pop	bp
	ret
	endp

CPROC	AddFar
	ARG	BASE:DWORD,LOCL:WORD,LOCH:WORD
	PENTER	0

	mov	ax,[word BASE+2]	; Get segment portion of address.
	xor	dx,dx

	LongShiftL	4	; One nibble up.
	add	ax,[word BASE]
	adc	dx,0		; Now have 20 bit result.

	add	ax,[LOCL]
	adc	dx,[LOCH]
	push	ax
	LongShiftR	4	; Into segment notation.
	mov	dx,ax		; Segment portion in DX
	pop	ax
	and	ax,0Fh		; Offset portion, normalized.

	PLEAVE
	ret
	endp

CPROC	MakeFP
	ARG	OFFS:WORD,SEGM:WORD
	PENTER	0

	mov	ax,[OFFS]
	mov	dx,[SEGM]

	PLEAVE
	ret
	endp

CPROC	MakeSeg
	ARG	ADR:DWORD
	PENTER	0

	mov	ax,[word ADR+2]

	PLEAVE
	ret
	endp


	ENDS
	END
