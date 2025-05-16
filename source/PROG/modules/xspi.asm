;------------------------------------------------------------------------------
; init SPI
; param 1 = mode (0..3)
; param 2 = divider
;------------------------------------------------------------------------------
xspi_init:		sbi	PORTB,0
			sbi	PORTB,1
			sbi	DDRB,0
			sbi	DDRB,1
			
			sbi	DDRB,7
			sbi	DDRB,5
		
			andi	r16,0x03	;mode
			lsl	r16
			lsl	r16
			mov	XL,r17
			lsr	XL
			andi	XL,0x02
			or	XL,r16
			ori	XL,0x50
			out	SPCR,XL
			inc	r17
			andi	r17,0x01
			out	SPSR,r17
			ret

;------------------------------------------------------------------------------
; write & radback via SPI and CS(B0)
; param 3/4 = length
;------------------------------------------------------------------------------
xspi_comm0:		rcall	xspi_init
			cbi	PORTB,0
			rcall	_xspi_comm
			sbi	PORTB,0
			jmp	main_loop_ok				

;------------------------------------------------------------------------------
; write & radback via SPI and CS(B1)
; param 3/4 = length
;------------------------------------------------------------------------------
xspi_comm1:		rcall	xspi_init
			cbi	PORTB,1
			rcall	_xspi_comm
			sbi	PORTB,1
			jmp	main_loop_ok				

_xspi_comm:		movw	r24,r18
			andi	r27,0x07
			adiw	r24,1
			andi	r19,0x07		;limit to 2K
			ldi	YL,0
			ldi	YH,1
_xspi_comm1:		sbiw	r24,1
			breq	_xspi_comm_end
			ld	XL,Y
			out	SPDR,XL
_xspi_comm2:		in	XL,SPSR
			andi	XL,0x80
			breq	_xspi_comm2
			in	XL,SPDR
			st	Y+,XL
			rjmp	_xspi_comm1

_xspi_comm_end:		ret

			
			
					