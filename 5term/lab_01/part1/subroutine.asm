
           temp.lst		Sourcer	v5.10    15-Sep-22  10:20 am  Page 2

				;                              SUBROUTINE

; подпрограмма sub_2. proc near - ближний переход внутри одного сегмента.
				sub_2		proc	near
; сохраняем аппаратный контекст в стек.
020A:07B9  1E					push	ds
020A:07BA  50					push	ax

; помещаем в ds адрес сегмента данных BIOS, используя буфер ax.
020A:07BB  B8 0040				mov	ax,40h
020A:07BE  8E D8				mov	ds,ax

; команда LAHF сохраняет младший байт регистра флагов
; в AH(старший байт AX) сохраняем, так как TEST меняет флаги.
020A:07C0  9F					lahf				; Load ah from flags

; команда TEST сравнивает между собой два числа, но меняет флаги, а не их. 
; Флаги SF, ZF, PF устанавливаются в соответствии с результатом.
; 2400h = 0010 0100 0000 0000. 10 бит - DF(Direction Flag),
; 13 - IOPL(input/output privilege level).
020A:07C1  F7 06 0314 2400			test	word ptr ds:[314h],2400h	; (0040:0314=3200h)

; если DF != 0 или старший бит IOPL != 0, то переходим на loc_7.
020A:07C7  75 0C				jnz	loc_7			; Jump if not zero

; если DF == 0 и старший бит IOPL == 0, то надо 
; принудительно сбрасывать IF, через CLI не получится.
; FDFFh = 1111 1101 1111 1111. '0' это 9 бит, IF - Interruption flag.
; команда LOCK блокирует локальную шину памяти 
; на время выполнения следующей команды.
; AND - выполняет логическое И между всеми битами 
; и результат записывает в первый операнд.
020A:07C9  F0> 81 26 0314 FDFF	                           lock	and	word ptr ds:[314h],0FDFFh	; (0040:0314=3200h)

020A:07D0			loc_6::
; команда SAHF загружает младший байт флагов из AH.
020A:07D0  9E					sahf				; Store ah into flags

; восстанааливаем аппаратный контекст из стека.
020A:07D1  58					pop	ax
020A:07D2  1F					pop	ds

; переход в loc_8.
020A:07D3  EB 03				jmp	short loc_8		; (07D8)

; если IOPL == 10 или 11.
020A:07D5			loc_7::
; CLI - Clear Interrupt-Enable Flag. Сбрасывает Interrupt flag (IF).
020A:07D5  FA					cli				; Disable interrupts

; восставновление и выход.
020A:07D6  EB F8				jmp	short loc_6		; (07D0)
020A:07D8			loc_8::

; возврат из подпрограммы.
; retn вытаскивает из стека IP, переходит на следующую после call команду.
020A:07D8  C3					retn
				sub_2		endp
