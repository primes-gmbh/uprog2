;################################################################################
;#										#
;# UPROG2 universal programmer for linux					#
;#										#
;# copyright (c) 2012-2016 Joerg Wolfram (joerg@jcwolfram.de)			#
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
.equ		JPPC_TMS	= SIG1
.equ		JPPC_TCK	= SIG2
.equ		JPPC_TDI	= SIG3
.equ		JPPC_TDO	= SIG4
.equ		JPPC_JCOMP	= SIG5
.equ		JPPC_RESET	= SIG6

.equ		jppc_buffer	= 0x0a00

.equ		JPPC_ONCE_RD_JID	= 0x202
.equ		JPPC_ONCE_WRX		= 0x191		;exit
.equ		JPPC_ONCE_RD_DUMMY	= 0x211

.equ		JPPC_ONCE_RD_DBCR0	= 0x231
.equ		JPPC_ONCE_WR_DBCR0	= 0x031

.equ		JPPC_ONCE_RD_DBCR1	= 0x232
.equ		JPPC_ONCE_WR_DBCR1	= 0x032

.equ		JPPC_ONCE_RD_DBCR2	= 0x233
.equ		JPPC_ONCE_WR_DBCR2	= 0x033

.equ		JPPC_ONCE_RD_OCR	= 0x212
.equ		JPPC_ONCE_WR_OCR	= 0x012
.equ		JPPC_ONCE_NEXUS		= 0x07C		;nexus 3 access

.equ		JPPC_ONCE_WR_CPUSCR	= 0x010		;cpu scan register

.equ		JPPC_NEXUS_WR_ADDR	= 0x13
.equ		JPPC_NEXUS_RD_ADDR	= 0x12
.equ		JPPC_NEXUS_WR_DATA	= 0x15
.equ		JPPC_NEXUS_RD_DATA	= 0x14

.equ		JPPC_NEXUS_WR_RWCS	= 0x0F
.equ		JPPC_NEXUS_RD_RWCS	= 0x0E
.equ		JPPC_NEXUS_RD_DSTAT	= 0x60


.macro		PPCJTAG_CLOCK	
			sbi	CTRLPORT,JPPC_TCK	;2
			nop				;1 filling
			push	r0
			cbi	CTRLPORT,JPPC_TCK	;2
			pop	r0
			nop
.endmacro

jtag_delay:		ret


jtag_init_io:		out	CTRLPORT,const_0
			sbi	CTRLDDR,JPPC_TMS
			sbi	CTRLDDR,JPPC_TCK
			sbi	CTRLDDR,JPPC_TDI
			cbi	CTRLDDR,JPPC_TDO
			sbi	CTRLDDR,JPPC_RESET	;set reset LO
			ret

;-------------------------------------------------------------------------------
; init jtag and read device ID
;-------------------------------------------------------------------------------
ppcjtag_init2:		out	CTRLPORT,const_0
			sbi	CTRLDDR,JPPC_TMS
			sbi	CTRLDDR,JPPC_TCK
			sbi	CTRLDDR,JPPC_TDI
			cbi	CTRLDDR,JPPC_TDO
			sbi	CTRLDDR,JPPC_JCOMP
			sbi	CTRLPORT,JPPC_JCOMP
			sbi	CTRLDDR,JPPC_RESET	;set reset LO NEW!!!
			call	api_vcc_on
			rjmp	jppc_init_0

;-------------------------------------------------------------------------------
; init jtag and read device ID
;-------------------------------------------------------------------------------
ppcjtag_init:		out	CTRLPORT,const_0
			sbi	CTRLDDR,JPPC_TMS
			sbi	CTRLDDR,JPPC_TCK
			sbi	CTRLDDR,JPPC_TDI
			cbi	CTRLDDR,JPPC_TDO
			sbi	CTRLDDR,JPPC_RESET	;set reset LO
			call	api_vcc_on
jppc_init_0:		ldi	ZL,100
			ldi	ZH,0
			call	api_wait_ms
			sbi	CTRLPORT,JPPC_TMS	;->reset
			ldi	r24,16
			ldi	r25,0
jppc_init_1:		PPCJTAG_CLOCK
			sbiw	r24,1
			brne	jppc_init_1
			cbi	CTRLPORT,JPPC_TMS	;->run-test-idle
			ldi	r24,16
			ldi	r25,0
jppc_init_2:		PPCJTAG_CLOCK
			sbiw	r24,1
			brne	jppc_init_2
	
			ldi	ZL,0		
jppc_init_3:		dec	ZL
			brne	jppc_init_3
			
			mov	r24,r19			;5 bits
			set				;IR shift
			ldi	r16,0x01
			rcall	jppc_shift		
	
			clr	r16
			clr	r17
			clr	r18
			clr	r19
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
			sts	0x100,r20
			sts	0x101,r21
			sts	0x102,r22
			sts	0x103,r23
			
			jmp	main_loop_ok

;-------------------------------------------------------------------------------
; exit jtag 
;-------------------------------------------------------------------------------
ppcjtag_exit:		sbi	CTRLPORT,JPPC_TMS	;->reset
			ldi	r24,100
			ldi	r25,0
jppc_exit_1:		PPCJTAG_CLOCK
			sbiw	r24,1
			brne	jppc_exit_1

			out	CTRLPORT,const_0
			out	CTRLDDR,const_0
			call	api_vcc_off
			jmp	main_loop_ok

;-------------------------------------------------------------------------------
; read JTAG ID via ONCE  
;-------------------------------------------------------------------------------
ppcjtag_read_jid:	mov	r24,r19			;n bits
			set				;IR shift
;			ldi	r16,0x11		;ACCESS_AUX_TAP_ONCE
			rcall	jppc_shift		
			
ppcjtag_read_jid_2:	ldi	r16,LOW(JPPC_ONCE_RD_JID)
			ldi	r17,HIGH(JPPC_ONCE_RD_JID)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift		
			
ppcjtag_get32:
ppcjtag_read_jid_2a:	clr	r16
			clr	r17
			clr	r18
			clr	r19
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
			sts	0x100,r20
			sts	0x101,r21
			sts	0x102,r22
			sts	0x103,r23

			jmp	main_loop_ok

;-------------------------------------------------------------------------------
; enter external debug mode  
;-------------------------------------------------------------------------------
ppcjtag_enter_dbg:	ldi	r16,LOW(JPPC_ONCE_WR_OCR)
			ldi	r17,HIGH(JPPC_ONCE_WR_OCR)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift		
			
			ldi	r16,0x05
			ldi	r17,0x00
			ldi	r18,0x00
			ldi	r19,0x00
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			jmp	main_loop_ok

;-------------------------------------------------------------------------------
; enter external debug mode  (set FDB)
;-------------------------------------------------------------------------------
ppcjtag_enter_dbg2:	ldi	r16,LOW(JPPC_ONCE_WR_DBCR0)
			ldi	r17,HIGH(JPPC_ONCE_WR_DBCR0)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift		
			
			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0x00
			ldi	r19,0x80
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			sbi	CTRLPORT,JPPC_RESET	;set reset HI

			jmp	main_loop_ok

;-------------------------------------------------------------------------------
; enter external debug mode  
;-------------------------------------------------------------------------------
ppcjtag_enter_dbg4:	cbi	CTRLPORT,JPPC_RESET	;set reset HI
			call	api_resetptr
			
			ldi	r16,LOW(JPPC_ONCE_WR_OCR)
			ldi	r17,HIGH(JPPC_ONCE_WR_OCR)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift		
			
			ldi	r16,0x05
			ldi	r17,0x80
			ldi	r18,0x80
			ldi	r19,0x00
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

;			jmp	main_loop_ok


;			sbi	CTRLPORT,JPPC_RESET	;set reset HI

			ldi	ZL,50
			ldi	ZH,0
			call	api_wait_ms

			ldi	r16,LOW(JPPC_ONCE_RD_OCR)
			ldi	r17,HIGH(JPPC_ONCE_RD_OCR)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0x00
			ldi	r19,0x00
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			mov	XL,r20
			call	api_buf_bwrite		;DATAH
			mov	XL,r21
			call	api_buf_bwrite
			mov	XL,r22
			call	api_buf_bwrite
			mov	XL,r23
			call	api_buf_bwrite		;DATAL

			jmp	main_loop_ok



;-------------------------------------------------------------------------------
; read OnCE status  
;-------------------------------------------------------------------------------
ppcjtag_once_status:	ldi	r16,LOW(JPPC_ONCE_RD_DUMMY)
			ldi	r17,HIGH(JPPC_ONCE_RD_DUMMY)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift		
			sts	0x100,r20		;status
			sts	0x101,r21
			jmp	main_loop_ok


;-------------------------------------------------------------------------------
; reset CPU  
;-------------------------------------------------------------------------------
ppcjtag_reset:		ldi	r16,LOW(JPPC_ONCE_WR_DBCR0)
			ldi	r17,HIGH(JPPC_ONCE_WR_DBCR0)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift		
			sts	0x104,r20		;status
			sts	0x105,r21
			
			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0x00
			ldi	r19,0xA0		;force reset CPU 0
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			jmp	main_loop_ok

;-------------------------------------------------------------------------------
; set PC 
;-------------------------------------------------------------------------------
ppjtag_set_pc:		call	api_resetptr
			ldi	r16,LOW(JPPC_ONCE_WR_CPUSCR)
			ldi	r17,HIGH(JPPC_ONCE_WR_CPUSCR)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift		
		
			ldi	r24,192			;bits to do
			rcall	jppc_mshift
			jmp	main_loop_ok


;-------------------------------------------------------------------------------
; exit external debug mode  
;-------------------------------------------------------------------------------
ppcjtag_exit_dbg:	ldi	r16,LOW(JPPC_ONCE_WR_DBCR0)
			ldi	r17,HIGH(JPPC_ONCE_WR_DBCR0)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift		
			sts	0x104,r20		;status
			sts	0x105,r21
			
			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0x00
			ldi	r19,0x00
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,LOW(JPPC_ONCE_WR_OCR)
			ldi	r17,HIGH(JPPC_ONCE_WR_OCR)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift		
			sts	0x104,r20		;status
			sts	0x105,r21
			
			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0x00
			ldi	r19,0x00
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

;			jmp	main_loop_ok

			ldi	r16,LOW(JPPC_ONCE_WRX)
			ldi	r17,HIGH(JPPC_ONCE_WRX)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift		
			sts	0x104,r20		;status
			sts	0x105,r21
			
			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0x00
			ldi	r19,0x00
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		


			jmp	main_loop_ok

;-------------------------------------------------------------------------------
; enter OnCE from main JTAG loop
;-------------------------------------------------------------------------------
ppcjtag_enter_once:	ldi	r24,5			;5 bits
			set				;IR shift
			ldi	r16,0x11		;ACCESS_AUX_TAP_ONCE
			rcall	jppc_shift		
			jmp	main_loop_ok
			


;-------------------------------------------------------------------------------
; nexus write longword
;-------------------------------------------------------------------------------	
ppcjtag_nexus_write:	call	api_resetptr
			mov	r6,r19			;Addr LSB
			mov	r7,r18
			mov	r8,r17
			mov	r9,r16			;addr MSB

			;this is for debg only and should return 0x80000000		
			ldi	r16,JPPC_NEXUS_RD_DSTAT
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift		

			clr	r16
			clr	r17
			clr	r18
			clr	r19
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
			
			rcall	nexus_set_address
			
			ldi	r16,JPPC_NEXUS_WR_RWCS
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0xC0		;highest prioity
			ldi	r19,0xD0		;32 bits, write access, start
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			rcall	nexus_write_data32r
			rcall	nexus_clear_rwcs
			
			jmp	main_loop_ok

;-------------------------------------------------------------------------------
; nexus read longword
;-------------------------------------------------------------------------------	
ppcjtag_nexus_pread:	ldi	ZL,0
			ldi	ZH,1
			call	api_wait_ms

ppcjtag_nexus_read:	call	api_resetptr
			mov	r6,r19			;Addr LSB
			mov	r7,r18
			mov	r8,r17
			mov	r9,r16			;addr MSB

			rcall	nexus_set_address
			
			ldi	r16,JPPC_NEXUS_WR_RWCS
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0xC0		;C0 = highest priority
			ldi	r19,0x90		;32 bit transfer, read
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
	
			rcall	nexus_read_data32
			rcall	nexus_clear_rwcs
					
			jmp	main_loop_ok
	

;-------------------------------------------------------------------------------
; nexus write block (r16-r19 = address)
;-------------------------------------------------------------------------------	
ppcjtag_nexus_wblock:	call	api_resetptr
			mov	r6,r19			;Addr LSB
			mov	r7,r18
			mov	r8,r17
			mov	r9,r16			;addr MSB
			ldi	ZL,0			;512 longs
			ldi	ZH,2
			
			
ppcjtag_nexus_wbk_1:	push	ZL
			push	ZH
			
			ldi	r16,JPPC_NEXUS_WR_ADDR
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		
		
			movw	r16,r6			;restore address
			movw	r18,r8
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
			
			ldi	r16,JPPC_NEXUS_WR_RWCS
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0xC0		;C0 = highest priority
			ldi	r19,0xD0		;32 bit transfer
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,JPPC_NEXUS_WR_DATA
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift				
			
			call	api_buf_bread		;DATAL
			mov	r16,XL
			call	api_buf_bread
			mov	r17,XL
			call	api_buf_bread
			mov	r18,XL
			call	api_buf_bread		;DATAH
			mov	r19,XL
			
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			rcall	ppjtag_wait_ready2

			movw	ZL,r6
			adiw	ZL,4
			movw	r6,ZL

			pop	ZH
			pop	ZL
			sbiw	ZL,1
			brne	ppcjtag_nexus_wbk_1

			ldi	r16,JPPC_NEXUS_WR_RWCS
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00		;C0 = highest
			ldi	r18,0x00
			ldi	r19,0x00		;end access
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
					
			jmp	main_loop_ok

;-------------------------------------------------------------------------------
; nexus write block in burst mode (r16-r19 = address)
;-------------------------------------------------------------------------------	
ppcjtag_nexus_wblockb:	call	api_resetptr
			mov	r6,r19			;Addr LSB
			mov	r7,r18
			mov	r8,r17
			mov	r9,r16			;addr MSB
			ldi	ZH,64
			
			
ppcjtag_nexus_wbkb_1:	push	ZH
			
			ldi	r16,JPPC_NEXUS_WR_ADDR	;0x13
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		
		
			movw	r16,r6			;restore address
			movw	r18,r8
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
			
			ldi	r16,JPPC_NEXUS_WR_RWCS	;0x0F
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x20		;8 transfers
			ldi	r17,0x00
			ldi	r18,0xE0		;C0 = highest priority
			ldi	r19,0xD0		;32 bit transfer, write
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		


			ldi	ZL,8
			
ppcjtag_nexus_wbkb_2:	push	ZL

			ldi	r16,JPPC_NEXUS_WR_DATA	;0x15
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift				
			
			call	api_buf_bread		;DATAL
			mov	r16,XL
			call	api_buf_bread
			mov	r17,XL
			call	api_buf_bread
			mov	r18,XL
			call	api_buf_bread		;DATAH
			mov	r19,XL
			
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			pop	ZL
			dec	ZL
			brne	ppcjtag_nexus_wbkb_2


			rcall	ppjtag_wait_ready2


			movw	ZL,r6
			adiw	ZL,32
			movw	r6,ZL

			pop	ZH
			dec	ZH
			brne	ppcjtag_nexus_wbkb_1

			ldi	r16,JPPC_NEXUS_WR_RWCS
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00		;C0 = highest
			ldi	r18,0x00
			ldi	r19,0x00		;end access
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
					
			jmp	main_loop_ok



;-------------------------------------------------------------------------------
; nexus write block in burst mode (r16-r19 = address)
;-------------------------------------------------------------------------------	
ppcjtag_nexus_wblockb2:	call	api_resetptr
			mov	r6,r19			;Addr LSB
			mov	r7,r18
			mov	r8,r17
			mov	r9,r16			;addr MSB
			ldi	ZH,64
			
			
ppcjtag_nexus_wbkb2_1:	push	ZH
			
			rcall	nexus_set_address

			
			ldi	r16,JPPC_NEXUS_WR_RWCS	;0x0F
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x20		;8 transfers
			ldi	r17,0x00
			ldi	r18,0xE0		;C0 = highest priority
			ldi	r19,0xD8		;64 bit transfer, write
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		


			ldi	ZL,8
			
ppcjtag_nexus_wbkb2_2:	push	ZL

			rcall	nexus_write_data32

			pop	ZL
			dec	ZL
			brne	ppcjtag_nexus_wbkb2_2


			rcall	ppjtag_wait_ready2

			movw	ZL,r6
			adiw	ZL,32
			movw	r6,ZL

			pop	ZH
			dec	ZH
			brne	ppcjtag_nexus_wbkb2_1
			
			rcall	nexus_clear_rwcs
					
			jmp	main_loop_ok


;-------------------------------------------------------------------------------
; nexus read block in burst mode (r16-r19 = address)
;-------------------------------------------------------------------------------	
ppcjtag_nexus_rblockb:	call	api_resetptr
			mov	r6,r19			;Addr LSB
			mov	r7,r18
			mov	r8,r17
			mov	r9,r16			;addr MSB
			ldi	ZH,64			;64 blocks
			
ppcjtag_nexus_rbkb_1:	push	ZH
			ldi	r16,JPPC_NEXUS_WR_ADDR	;0x13
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		
		
			movw	r16,r6			;set address
			movw	r18,r8
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
			
			ldi	r16,JPPC_NEXUS_WR_RWCS	;0x0F
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x20		;8 words
			ldi	r17,0x00
			ldi	r18,0xE0		;(C0 = highest priority,) burst
			ldi	r19,0x90		;32 bit transfer, read
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			rcall	ppjtag_wait_readys
							
			ldi	ZL,8	
		
ppcjtag_nexus_rbkb_2:	push	ZL
		
			ldi	r16,JPPC_NEXUS_RD_DATA
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift

			clr	r16
			clr	r17
			clr	r18
			clr	r19
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			mov	XL,r20
			call	api_buf_bwrite		;DATAL
			mov	XL,r21
			call	api_buf_bwrite
			mov	XL,r22
			call	api_buf_bwrite
			mov	XL,r23
			call	api_buf_bwrite		;DATAH


			pop	ZL
			dec	ZL
			brne	ppcjtag_nexus_rbkb_2
			
			movw	ZL,r6
			adiw	ZL,32
			movw	r6,ZL
			
			pop	ZH
			dec	ZH
			brne	ppcjtag_nexus_rbkb_1
			

			ldi	r16,JPPC_NEXUS_WR_RWCS
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00		;C0 = highest
			ldi	r18,0x00
			ldi	r19,0x00		;end access
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
					
			jmp	main_loop_ok



;-------------------------------------------------------------------------------
; nexus read block (r16-r19 = address)
;-------------------------------------------------------------------------------	
ppcjtag_nexus_rblock:	call	api_resetptr
			mov	r6,r19			;Addr LSB
			mov	r7,r18
			mov	r8,r17
			mov	r9,r16			;addr MSB
			ldi	ZL,0			;512 longs
			ldi	ZH,2
			
			
ppcjtag_nexus_rbk_1:	push	ZL
			push	ZH
			ldi	r16,JPPC_NEXUS_WR_ADDR
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift		
		
			movw	r16,r6			;set address
			movw	r18,r8
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
			
			ldi	r16,JPPC_NEXUS_WR_RWCS
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0xC0		;C0 = highest priority
			ldi	r19,0x90		;32 bit transfer, read
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
	
			ldi	r16,JPPC_NEXUS_RD_DATA
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift				
		
			clr	r16
			clr	r17
			clr	r18
			clr	r19
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			mov	XL,r20
			call	api_buf_bwrite		;DATAH
			mov	XL,r21
			call	api_buf_bwrite
			mov	XL,r22
			call	api_buf_bwrite
			mov	XL,r23
			call	api_buf_bwrite		;DATAL

			movw	ZL,r6
			adiw	ZL,4
			movw	r6,ZL

			pop	ZH
			pop	ZL
			sbiw	ZL,1
			brne	ppcjtag_nexus_rbk_1

			ldi	r16,JPPC_NEXUS_WR_RWCS
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00		;C0 = highest
			ldi	r18,0x00
			ldi	r19,0x00		;end access
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		
					
			jmp	main_loop_ok
			
ppcjtag_get_rwcs:	ldi	r16,JPPC_NEXUS_RD_RWCS
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		
			rjmp	ppcjtag_get32	
			
;-------------------------------------------------------------------------------
; wait for ready  
;-------------------------------------------------------------------------------
ppjtag_wait_ready:	ldi	r16,JPPC_NEXUS_RD_RWCS
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0x00
			ldi	r19,0x00
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			andi	r20,0x01		;loop if not completed
			breq	ppjtag_wait_ready	
			ret

;-------------------------------------------------------------------------------
; wait for ready and throw error if timeout 
;-------------------------------------------------------------------------------
ppjtag_wait_readys:	push	ZL
			push	ZH
			ldi	ZL,0
			ldi	ZH,0x40
			
ppjtag_wait_readys_1:	ldi	r16,JPPC_NEXUS_RD_RWCS
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0x00
			ldi	r19,0x00
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			andi	r20,0x01		;loop if not completed
			brne	ppjtag_wait_readys_2	
			sbiw	ZL,1
			brne	ppjtag_wait_readys_1
			pop	ZH
			pop	ZL
			pop	r16
			pop	r16
			ldi	r16,0x41
			jmp	main_loop

ppjtag_wait_readys_2:	pop	ZH
			pop	ZL
			ret


;-------------------------------------------------------------------------------
; wait for ready  
;-------------------------------------------------------------------------------
ppjtag_wait_ready2:	ldi	r16,JPPC_NEXUS_RD_RWCS
			ldi	r24,8			;8 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00
			ldi	r18,0x00
			ldi	r19,0x00
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			andi	r20,0x01		;loop if not completed
			brne	ppjtag_wait_ready2	
			ret

	
;-------------------------------------------------------------------------------
; enter nexus mode from OnCE  
;-------------------------------------------------------------------------------
ppcjtag_once_nexus:	ldi	r16,LOW(JPPC_ONCE_NEXUS)
			ldi	r17,HIGH(JPPC_ONCE_NEXUS)
			ldi	r24,10			;10 bits
			set				;IR shift
			rcall	jppc_shift
			jmp	main_loop_ok
			
;-------------------------------------------------------------------------------
; enter nexus block
;-------------------------------------------------------------------------------
ppcjtag_enter_nexus:	ldi	r24,5			;5 bits
			set				;IR shift
			ldi	r16,0x12		;ACCESS_AUX_TAP_NPC
			rcall	jppc_shift		
			jmp	main_loop_ok
			
;-------------------------------------------------------------------------------
; unlock device (r16=register)
;-------------------------------------------------------------------------------
ppcjtag_unlock:		ldi	XL,0x07			;censorship register
			call	api_resetptr		;reset buffer pointer
			ldi	r24,5			;5 bits
			set				;IR shift
			rcall	jppc_shift		
		
			ldi	XL,0xff			;this is for bit 65
			sts	0x108,XL
			
			ldi	r24,64			;bits to do
			rcall	jppc_mshift
	
ppcjtag_unlockx:	jmp	main_loop_ok

			
;-------------------------------------------------------------------------------
; unlock device (SPC58x)
;-------------------------------------------------------------------------------
ppcjtag_unlock2:	mov	XL,r16
			call	api_resetptr		;reset buffer pointer
			ldi	r24,6			;5 bits
			set				;IR shift
			rcall	jppc_shift		
		
			clt				;IR shift
			ldi	r24,0			;256 bits to do
			rcall	jppc_mshift
	
ppcjtag_unlock2x:	jmp	main_loop_ok
			

;------------------------------------------------------------------------------
; clear nexus rwcs register
;------------------------------------------------------------------------------
nexus_clear_rwcs:	ldi	r16,JPPC_NEXUS_WR_RWCS
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift		

			ldi	r16,0x00
			ldi	r17,0x00		;C0 = highest
			ldi	r18,0x00
			ldi	r19,0x00		;end access
			ldi	r24,32			;32 bits
			clt				;DR shift
			rjmp	jppc_shift		

;------------------------------------------------------------------------------
; set nexus address register
;------------------------------------------------------------------------------
nexus_set_address:	ldi	r16,JPPC_NEXUS_WR_ADDR
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift		
		
			movw	r16,r6			;set address
			movw	r18,r8
			ldi	r24,32			;32 bits
			clt				;DR shift
			rjmp	jppc_shift		

	
;------------------------------------------------------------------------------
; read 32 bits over nexus data register
;------------------------------------------------------------------------------
nexus_read_data32:	ldi	r16,JPPC_NEXUS_RD_DATA
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift				
		
			clr	r16
			clr	r17
			clr	r18
			clr	r19
			ldi	r24,32			;32 bits
			clt				;DR shift
			rcall	jppc_shift		

			mov	XL,r20
			call	api_buf_bwrite		;DATAH
			mov	XL,r21
			call	api_buf_bwrite
			mov	XL,r22
			call	api_buf_bwrite
			mov	XL,r23
			jmp	api_buf_bwrite		;DATAL

;------------------------------------------------------------------------------
; write 32 bits over nexus data register
;------------------------------------------------------------------------------
nexus_write_data32r:	ldi	r16,JPPC_NEXUS_WR_DATA
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift				

			call	api_buf_bread		;DATAH
			mov	r19,XL
			call	api_buf_bread
			mov	r18,XL
			call	api_buf_bread
			mov	r17,XL
			call	api_buf_bread		;DATAL
			mov	r16,XL
			ldi	r24,32			;32 bits
			clt				;DR shift
			rjmp	jppc_shift
			
;------------------------------------------------------------------------------
; write 32 bits over nexus data register
;------------------------------------------------------------------------------
nexus_write_data32:	ldi	r16,JPPC_NEXUS_WR_DATA
			ldi	r24,8			;10 bits
			clt				;DR shift
			rcall	jppc_shift				

			call	api_buf_bread		;DATAH
			mov	r16,XL
			call	api_buf_bread
			mov	r17,XL
			call	api_buf_bread
			mov	r18,XL
			call	api_buf_bread		;DATAL
			mov	r19,XL
			ldi	r24,32			;32 bits
			clt				;DR shift

;------------------------------------------------------------------------------
; do IR/DR SHIFT (1-32 Bits)
; T=0 -> DR SCAN
; T=1 -> IR SCAN
; r16-r19	Data to send
; r20-r23	Data read
; r24		Bits to shift
;------------------------------------------------------------------------------
jppc_shift:		clr	r20
			clr	r21
			clr	r22
			clr	r23
			ldi	r25,0x20
			sub	r25,r24				;r25=result shift
			sbi	CTRLPORT,JPPC_TMS
			PPCJTAG_CLOCK				;-> DR-scan
			brtc	jppc_shift_1
			PPCJTAG_CLOCK				;-> IR-scan
jppc_shift_1:		cbi	CTRLPORT,JPPC_TMS
			PPCJTAG_CLOCK				;-> CAPTURE
			PPCJTAG_CLOCK				;-> SHIFT

jppc_shift_2:		sbrc	r16,0				;skip if zero
			sbi	CTRLPORT,JPPC_TDI
			sbrs	r16,0				;skip if one
			cbi	CTRLPORT,JPPC_TDI
			cpi	r24,1				;is this the last?
			brne	jppc_shift_3
			sbi	CTRLPORT,JPPC_TMS		;last bit -> exit IR
jppc_shift_3:		sbi	CTRLPORT,JPPC_TCK	;2			
			lsr	r19
			ror	r18
			ror	r17
			ror	r16

			lsr	r23
			ror	r22
			ror	r21
			ror	r20
			
			sbic	CTRLPIN,JPPC_TDO
			ori	r23,0x80			;set bit
			cbi	CTRLPORT,JPPC_TCK	;2			
			dec	r24				;bit counter
			brne	jppc_shift_2			;shift loop

			PPCJTAG_CLOCK				;-> UPDATE
			cbi	CTRLPORT,JPPC_TMS		;
			PPCJTAG_CLOCK				;-> run-test-idle
			PPCJTAG_CLOCK				;-> additional clocks
			PPCJTAG_CLOCK				;-> additional clocks
			
jppc_shift_4:		cpi	r25,0
			breq	jppc_shift_5
			lsr	r23
			ror	r22
			ror	r21
			ror	r20
			dec	r25
			rjmp	jppc_shift_4
					
jppc_shift_5:		ret


;------------------------------------------------------------------------------
; do DR SHIFT (1-255 Bits)
; [buffer]	Data to send (starting with LSB first)
; r24		Bits to shift
; r20		result
;------------------------------------------------------------------------------
jppc_mshift:		sbi	CTRLPORT,JPPC_TMS
			PPCJTAG_CLOCK				;-> DR-scan
			cbi	CTRLPORT,JPPC_TMS
			PPCJTAG_CLOCK				;-> CAPTURE
			PPCJTAG_CLOCK				;-> SHIFT
			call	api_buf_bread			;read first byte
			ldi	r25,6				;bits per byte -2
			subi	r24,2				;-2 bits
			clr	r20				;result=0

			;bit 0
jppc_mshift_1:		sbrc	XL,0				;skip if zero
			sbi	CTRLPORT,JPPC_TDI
			sbrs	XL,0				;skip if one
			cbi	CTRLPORT,JPPC_TDI
			sbi	CTRLPORT,JPPC_TCK		;2			
			lsr	XL
			nop
			nop
			sbic	CTRLPIN,JPPC_TDO
			ori	r20,0x01			;set bit 0 in result
			cbi	CTRLPORT,JPPC_TCK		;2			
	
			sbrc	XL,0				;skip if zero
			sbi	CTRLPORT,JPPC_TDI
			sbrs	XL,0				;skip if one
			cbi	CTRLPORT,JPPC_TDI
			sbi	CTRLPORT,JPPC_TCK		;2			
			lsr	XL
			nop
			nop
			sbic	CTRLPIN,JPPC_TDO
			ori	r20,0x02			;set bit 1
			cbi	CTRLPORT,JPPC_TCK		;2			
	
jppc_mshift_2:		sbrc	XL,0				;skip if zero
			sbi	CTRLPORT,JPPC_TDI
			sbrs	XL,0				;skip if one
			cbi	CTRLPORT,JPPC_TDI
			lsr	XL	
			
			cpi	r24,1				;is this the last?
			brne	jppc_mshift_3
			sbi	CTRLPORT,JPPC_TMS		;last bit -> exit IR
jppc_mshift_3:		sbi	CTRLPORT,JPPC_TCK		;2			
			dec	r25
			brne	jppc_mshift_4			
			call	api_buf_bread			;read first byte
			ldi	r25,8				;bits per byte

jppc_mshift_4:		cbi	CTRLPORT,JPPC_TCK		;2			
			dec	r24				;bit counter
			brne	jppc_mshift_2			;shift loop

jppc_mshift_ex:		PPCJTAG_CLOCK				;-> UPDATE
			cbi	CTRLPORT,JPPC_TMS		;
			PPCJTAG_CLOCK				;-> run-test-idle
			PPCJTAG_CLOCK				;-> additional clocks
			PPCJTAG_CLOCK				;-> additional clocks

jppc_mshift_5:		ret


;------------------------------------------------------------------------------
; do one tck clock
;------------------------------------------------------------------------------
jppc_stcks:		sbi	CTRLPORT,JPPC_TCK	;2
			nop				;1 filling
			cbi	CTRLPORT,JPPC_TCK	;2
			ret

ppcjtag_xreset:		cbi	CTRLPORT,JPPC_RESET
			ldi	ZL,100
			ldi	ZH,0
			call	api_wait_ms
			sbi	CTRLPORT,JPPC_RESET
			out	CTRLDDR,const_0
			jmp	main_loop_ok
			

;------------------------------------------------------------------------------
; do DR SHIFT (1-256 Bytes write)
; [buffer]	Data to send (starting with LSB first)
; r24		Bytes to shift
;------------------------------------------------------------------------------
jtag_wshift:		call	api_buf_bread			;read first byte
			rcall	jtag_xshift
			dec	r24
			brne	jtag_wshift
			ret

;------------------------------------------------------------------------------
; do DR SHIFT (1-256 Bytes read)
; [buffer]	Data to send (starting with LSB first)
; r24		Bytes to shift
;------------------------------------------------------------------------------	
jtag_rshift:		ldi	XL,0xff
			rcall	jtag_xshift
			mov	XL,XH
			call	api_buf_bwrite			;read first byte
			dec	r24
			brne	jtag_rshift
			ret
			
;------------------------------------------------------------------------------
; byte exchange shift (r25 used)
; XL = IN
; XH = OUT
;------------------------------------------------------------------------------
jtag_xshift:		ldi	r25,8				;bits per byte
			clr	XH
			sbi	CTRLPORT,JPPC_TMS
			PPCJTAG_CLOCK				;-> DR-scan
			cbi	CTRLPORT,JPPC_TMS
			PPCJTAG_CLOCK				;-> CAPTURE
			PPCJTAG_CLOCK				;-> SHIFT

jtag_xshift_1:		sbrc	XL,0				;skip if zero
			sbi	CTRLPORT,JPPC_TDI
			sbrs	XL,0				;skip if one
			cbi	CTRLPORT,JPPC_TDI
			sbi	CTRLPORT,JPPC_TCK		;2			
			lsr	XL
			lsr	XH
			sbic	CTRLPIN,JPPC_TDO
			ori	XH,0x80				;set bit 7 in result
			cbi	CTRLPORT,JPPC_TCK		;2			
			cpi	r25,2
			breq	jtag_xshift_2
			dec	r25
			brne	jtag_xshift_1
			rjmp	jppc_mshift_ex			;update etc.		
						
jtag_xshift_2:		sbi	CTRLPORT,JPPC_TMS		;last bit -> exit DR
			dec	r25
			rjmp	jtag_xshift_1


