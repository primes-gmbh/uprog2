;################################################################################
;#										#
;# UPROG2 universal programmer for linux					#
;#										#
;# copyright (c) 2012-2023 Joerg Wolfram (joerg@jcwolfram.de)			#
;#										#
;#										#
;# This program is free software; you can redistribute it and/or		#
;# modify it under the terms of the GNU General Public License			#
;# as published by the Free Software Foundation; either version 2		#
;# of the License, or (at your option) any later version.			#
;#										#
;# This program is distributed in the hope that it will be useful,		#
;# but WITHOUT ANY WARRANTY; without even the implied warranty of		#
;# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU		#
;# General Public License for more details.					#
;#										#
;# You should have received a copy of the GNU General Public			#
;# License along with this library; if not, write to the			#
;# Free Software Foundation, Inc., 59 Temple Place - Suite 330,			#
;# Boston, MA 02111-1307, USA.							#
;#										#
;################################################################################

;pic offset
;
.equ	PIC3_LADDR	= 0x80
.equ	PIC3_BERASE	= 0x18
.equ	PIC3_PERASE	= 0xF0
.equ	PIC3_READ	= 0xFC
.equ	PIC3_READI	= 0xFE
.equ	PIC3_WRITE	= 0xC0
.equ	PIC3_WRIITEI	= 0xE0
.equ	PIC3_AINC	= 0xF8

.equ	PIC3_CLOCK	= SIG2
.equ	PIC3_DATA	= SIG1


;------------------------------------------------------------------------------
; init and send key (if par4=1)
;------------------------------------------------------------------------------
pic3_init:		ldi	YL,0			;copy table
			ldi	YH,1
			ldi	XL,4			;base register
			ldi	XH,0
			ldi	r16,10
pic3_init_1:		ld	r17,Y+
			st	X+,r17
			dec	r16
			brne	pic3_init_1

			cbi	CTRLPORT,PIC3_DATA	;all low
			cbi	CTRLPORT,PIC3_CLOCK	;all low
			sbi	CTRLDDR,PIC3_DATA		;all to output
			sbi	CTRLDDR,PIC3_CLOCK	;all to output

			call	api_vcc_on
			call	pic_w20ms

			call	api_vpp_on
			call	pic_w20ms

			sbrc	r19,0
			rcall	pic3_okey		;emit key

			jmp	main_loop_ok

			;omit key
pic3_okey:		ldi	XH,0x4d
			ldi	r22,0x43
			ldi	r21,0x48
			ldi	r20,0x50
			rjmp	pic3_wtrans


;------------------------------------------------------------------------------
; erase
;
; par4;
; bit 0		EEPROM
; bit 1		Flash
; bit 2		UID
; bit 3		CONFIG
;------------------------------------------------------------------------------
pic3_bulk_erase:	ldi	XL,PIC3_BERASE
			ldi	r22,0x00
			ldi	r21,0x00
			mov	r20,r19
							
			rcall	pic3_wtrans

			ldi	ZL,12
			ldi	ZH,0
			call	api_wait_ms

			jmp	main_loop_ok


;------------------------------------------------------------------------------
; readf 512B blocks
; par1 = AADRL
; par2 = ADDRH
; par3 = ADDRU
; par4 = blocks
;------------------------------------------------------------------------------
pic3_readf:		clr	YL
			ldi	YH,1
			rcall	pic3_addrout
			mov	r25,r19
			clr	r24
			
pic3_readf_1:		ldi	XL,PIC3_READI
			rcall	pic3_rtrans
			st	Y+,r20
			st	Y+,r21
			sbiw	r24,1
			brne	pic3_readf_1
		
			jmp	main_loop_ok
			

;------------------------------------------------------------------------------
; readf words
; par1 = AADRL
; par2 = ADDRH
; par3 = ADDRU
; par4 = blocks
;------------------------------------------------------------------------------
pic3_readw:		clr	YL
			ldi	YH,1
			rcall	pic3_addrout
			mov	r24,r19
			clr	r25
			rjmp	pic3_readf_1
			

;------------------------------------------------------------------------------
; read bytes
; par1 = AADRL
; par2 = ADDRH
; par3 = ADDRU
; par4 = bytes
;------------------------------------------------------------------------------
pic3_readb:		clr	YL
			ldi	YH,1
			rcall	pic3_addrout
			mov	r24,r19
			clr	r25
			
pic3_readb_1:		ldi	XL,PIC3_READI
			rcall	pic3_rtrans
			st	Y+,r20
			sbiw	r24,1
			brne	pic3_readb_1
		
			jmp	main_loop_ok
	

;------------------------------------------------------------------------------
; progf 512B blocks, par4=blocks
; par1 = AADRL
; par2 = ADDRH
; par3 = ADDRU
; par4 = blocks
;------------------------------------------------------------------------------
pic3_progf:		clr	YL
			ldi	YH,1
			rcall	pic3_addrout
			mov	r25,r19
			clr	r24
	
pic3_progf_1:		ldi	XL,PIC3_WRIITEI
			ld	r20,Y+
			ld	r21,Y+
			clr	r22
			rcall	pic3_wtrans
			
			ldi	ZL,128
			ldi	ZH,1
pic3_progf_2:		sbiw	ZL,1
			brne	pic3_progf_2			

			sbiw	r24,1
			brne	pic3_progf_1
		
			jmp	main_loop_ok
			

;------------------------------------------------------------------------------
; prog words
; par1 = AADRL
; par2 = ADDRH
; par3 = ADDRU
; par4 = words
;------------------------------------------------------------------------------
pic3_progw:		clr	YL
			ldi	YH,1
			rcall	pic3_addrout
			mov	r24,r19
			clr	r25
			rjmp	pic3_progf_1

;------------------------------------------------------------------------------
; prog bytes
; par1 = AADRL
; par2 = ADDRH
; par3 = ADDRU
; par4 = bytes
;------------------------------------------------------------------------------
pic3_progb:		clr	YL
			ldi	YH,1
			rcall	pic3_addrout
			mov	r24,r19
			clr	r25
	
pic3_progb_1:		ldi	XL,PIC3_WRIITEI
			ld	r20,Y+
			clr	r21
			clr	r22
			rcall	pic3_wtrans
			
			ldi	ZL,12
			ldi	ZH,0
			call	api_wait_ms

			sbiw	r24,1
			brne	pic3_progb_1
		
			jmp	main_loop_ok
			

;------------------------------------------------------------------------------
; write transfer
; r20	LSB
; r21
; r22	MSB
; XL	CMD
;------------------------------------------------------------------------------
pic3_wtrans:		ldi	r23,8

pic3_wtrans1:		sbi	CTRLPORT,PIC3_CLOCK		;clk=1
			sbrc	XL,7
			sbi	CTRLPORT,PIC3_DATA
			sbrs	XL,7
			cbi	CTRLPORT,PIC3_DATA

			cbi	CTRLPORT,DS_CLOCK		;clk=0
			
			lsl	XL
			dec	r23
			brne	pic3_wtrans1

			rcall	pic3_dly
	
			ldi	r23,24

pic3_wtrans2:		lsl	r20
			rol	r21
			rol	r22
	
			sbi	CTRLPORT,PIC3_CLOCK		;clk=1
			sbrc	r22,7
			sbi	CTRLPORT,PIC3_DATA
			sbrs	r22,7
			cbi	CTRLPORT,PIC3_DATA

			cbi	CTRLPORT,DS_CLOCK		;clk=0
			
			dec	r23
			brne	pic3_wtrans2
	
pic3_wtrans3:		rcall	pic3_dly
			ret


;------------------------------------------------------------------------------
; read transfer
; XL	CMD
;
; r20	LSB
; r21
; r22	MSB
;------------------------------------------------------------------------------
pic3_rtrans:		ldi	r23,8

pic3_rtrans1:		sbi	CTRLPORT,PIC3_CLOCK		;clk=1
			sbrc	XL,7
			sbi	CTRLPORT,PIC3_DATA
			sbrs	XL,7
			cbi	CTRLPORT,PIC3_DATA

			cbi	CTRLPORT,PIC3_CLOCK		;clk=0
			
			lsl	XL
			dec	r23
			brne	pic3_rtrans1

			cbi	CTRLDDR,PIC3_DATA

			rcall	pic3_dly

			ldi	r23,24

pic3_rtrans2:		sbi	CTRLPORT,PIC3_CLOCK		;clk=1	
			lsl	r20
			rol	r21
			rol	r22
			cbi	CTRLPORT,PIC3_CLOCK		;clk=0
			sbic	CTRLPIN,PIC3_DATA
			inc	r20
			
			dec	r23
			brne	pic3_rtrans2
	
			lsr	r22
			ror	r21
			ror	r20

			rcall	pic3_dly
			sbi	CTRLDDR,PIC3_DATA
			
		
pic3_rtrans3:		ret

pic3_dly:		rcall	pic3_dly1
			rcall	pic3_dly1
pic3_dly1:		ret


;------------------------------------------------------------------------------
; set addr
;------------------------------------------------------------------------------
pic3_addrout:		ldi	XL,PIC3_LADDR
			movw	r20,r16
			mov	r22,r18
			rjmp	pic3_wtrans
			