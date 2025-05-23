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
.equ			S32K_RST		= SIG1
.equ			S32K_CLOCK		= SIG2
.equ			S32K_DATA		= SIG3
.equ			S32K_TRIGGER		= SIG5

.equ			S32K_ZERO_R		= 0				;none
.equ			S32K_ONE_R		= SIG3_OR			;data
.equ			S32K_ZERO		= SIG1_OR			;reset
.equ			S32K_ONE		= (SIG3_OR + SIG1_OR)		;data + reset
.equ			S32K_CLK		= SIG2_OR			;only clock


;SW-DP registers
.equ			S32K_READ_IDCODE	= 0xa5
.equ			S32K_WRITE_ABORT	= 0x81

.equ			S32K_READ_CTRL		= 0xb1
.equ			S32K_WRITE_CTRL		= 0x95

.equ			S32K_READ_SELECT	= 0xa9
.equ			S32K_WRITE_SELECT	= 0x8d

.equ			S32K_READ_RDBUFF	= 0xbd		;read buffer
.equ			S32K_WRITE_RDBUFF	= 0x99


; AHB-AP registers
.equ			S32K_READ_CSW		= 0xe1		;00
.equ			S32K_WRITE_CSW		= 0xc5

.equ			S32K_READ_TAR		= 0xf5		;04
.equ			S32K_WRITE_TAR		= 0xd1

.equ			S32K_READ_IDR		= 0xed		;08
.equ			S32K_WRITE_SEL		= 0xc9

.equ			S32K_READ_DRW		= 0xf9		;0C
.equ			S32K_WRITE_DRW		= 0xdd

			
;-------------------------------------------------------------------------------
; init
; R19: 	bit 0 	without JTAG->SWD sequence
;	bit 1	with RESET enabled
;	bit 2	no debug core start
;	bit 4   no autoincrement
;-------------------------------------------------------------------------------
s32k_init:		out		CTRLPORT,const_0
			sbi		CTRLDDR,S32K_RST
			sbi		CTRLDDR,S32K_CLOCK
			sbi		CTRLDDR,S32K_DATA
			sbi		CTRLDDR,S32K_TRIGGER

			;we do a connect under reset
			call		api_vcc_on		;power on
			ldi		ZL,100
			ldi		ZH,0
			call		api_wait_ms

s32k_init_1:		cbi		CTRLPORT,S32K_CLOCK
			cbi		CTRLPORT,S32K_DATA
			clr		ZL
			rcall		swd32_w0_1

			ldi		r16,0x41		;timeout

			sbrs		r19,1
			rcall		swd32_reginit_reset	;set registers for faster output
			sbrc		r19,1
			rcall		swd32_reginit		;set registers for faster output
			out		CTRLPORT,r13		;ONE (is pull-up)

			ldi		ZL,1
			ldi		ZH,0
			call		api_wait_ms

			ldi		YL,0
			ldi		YH,1

			;now get chip id
			ldi		r24,0
			ldi		r25,4			;1024 tries
s32k_init_2:		rcall		swd32_reset		;reset state machine

			ldi		XL,S32K_READ_IDCODE	;read ID code
			rcall		swd32_read_dap
			
			sts		0x100,XL
			cpi		XL,0x04			;no ack -> exit
			breq		s32k_init_3
			rcall		swd32_wait_1ms		;next try
			sbiw		r24,1
			brne		s32k_init_2
			rjmp		s32k_init_err

s32k_init_3:		call		gen_w32

			sbrc		r19,0
			rjmp		s32k_init_end

			ldi		ZL,LOW(s32k_data_init1*2)
			ldi		ZH,HIGH(s32k_data_init1*2)
			sbrc		r19,4
			adiw		ZL,36
			sbrc		r19,4
			adiw		ZL,30

			rcall		swd32_read_ctrlstat			

			ldi		r24,5
s32k_init_3a:		rcall		swd32_write_dap_table
			dec		r24
			brne		s32k_init_3a

;			sbi		CTRLPORT,S32K_TRIGGER
;			cbi		CTRLPORT,S32K_TRIGGER
			sbrc		r19,2
			rjmp		s32k_init_4

			;debug core start
			ldi		r24,3
s32k_init_3b:		rcall		swd32_write_dap_table
			dec		r24
			brne		s32k_init_3b

		
			;set reset vector catch
			ldi		r24,3
s32k_init_3c:		rcall		swd32_write_dap_table
			dec		r24
			brne		s32k_init_3c
			
;			rcall		swd32_read_drwx
s32k_init_4:		sbi		CTRLPORT,S32K_RST	;release reset

			ldi		ZL,50
			ldi		ZH,0
;			call		api_wait_ms

			rcall		swd32_reginit		;set registers for faster output

s32k_init_end:		jmp		main_loop_ok

		
s32k_init_err:		ldi		r16,0x41
			out		CTRLDDR,const_0
			out		CTRLPORT,const_0
			jmp		main_loop

s32k_data_init1:	;DebugPortStart
			.db S32K_WRITE_ABORT,	0x00,	0x00,0x00,0x00,0x1e	;clear all errors
			.db S32K_WRITE_SELECT,	0x00,	0x00,0x00,0x00,0x00	;switch to Bank 0x00			
			.db S32K_WRITE_CTRL,	0x00,	0x50,0x00,0x00,0x00	;power up debug interface
			.db S32K_WRITE_CTRL,	0x00,	0x54,0x00,0x00,0x00	;request debug reset
			.db S32K_WRITE_CTRL,	0x00,	0x50,0x00,0x0F,0x00	;init AP transfer mode

			;DebugCoreStart
			.db S32K_WRITE_CSW,	0x00,	0x23,0x00,0x00,0x32	;32 bit access, auto increment
			.db S32K_WRITE_TAR,	0x00,	0xE0,0x00,0xED,0xF0	;DHCSR
			.db S32K_WRITE_DRW,	0x00,	0xA0,0x5F,0x00,0x09	;halt CPU and enable debug
			
			;set reset vector catch
			.db S32K_WRITE_TAR,	0x00,	0xE0,0x00,0xED,0xFC	;DEMCR
			.db S32K_WRITE_DRW,	0x00,	0x00,0x00,0x00,0x01	;enable reset vector catch
			.db S32K_WRITE_DRW,	0x00,	0x00,0x00,0x00,0x01	;enable reset vector catch		


s32k_data_init2:	;DebugPortStart
			.db S32K_WRITE_ABORT,	0x00,	0x00,0x00,0x00,0x1e	;clear all errors
			.db S32K_WRITE_SELECT,	0x00,	0x00,0x00,0x00,0x00	;switch to Bank 0x00			
			.db S32K_WRITE_CTRL,	0x00,	0x50,0x00,0x00,0x00	;power up debug interface
			.db S32K_WRITE_CTRL,	0x00,	0x54,0x00,0x00,0x00	;request debug reset
			.db S32K_WRITE_CTRL,	0x00,	0x50,0x00,0x0F,0x00	;init AP transfer mode

			;DebugCoreStart
			.db S32K_WRITE_CSW,	0x00,	0x23,0x00,0x00,0x02	;32 bit access
			.db S32K_WRITE_TAR,	0x00,	0xE0,0x00,0xED,0xF0	;DHCSR
			.db S32K_WRITE_DRW,	0x00,	0xA0,0x5F,0x00,0x09	;halt CPU and enable debug
			
			;set reset vector catch
			.db S32K_WRITE_TAR,	0x00,	0xE0,0x00,0xED,0xFC	;DEMCR
			.db S32K_WRITE_DRW,	0x00,	0x00,0x00,0x00,0x01	;enable reset vector catch
			.db S32K_WRITE_DRW,	0x00,	0x00,0x00,0x00,0x01	;enable reset vector catch		


;-------------------------------------------------------------------------------
; check_auth
;-------------------------------------------------------------------------------
s32k3_check_auth:	ldi		YL,0
			ldi		YH,1
			rcall		swd32_reginit		;set registers for faster output

			ldi		ZL,LOW(s32k3_data_cauth*2)
			ldi		ZH,HIGH(s32k3_data_cauth*2)

			rcall		swd32_write_dap_table
			rcall		swd32_write_dap_table
			rcall		swd32_write_dap_table
			rcall		s32k_rap0

			rcall		swd32_write_dap_table
			rcall		s32k_rap0
			

			rcall		swd32_write_dap_table
			rcall		s32k_rap0
			
			jmp		main_loop_ok



s32k3_data_cauth:	;DebugPortStart
			.db S32K_WRITE_ABORT,	0x00,	0x00,0x00,0x00,0x1e	;clear all errors
			.db S32K_WRITE_SELECT,	0x00,	0x07,0x00,0x00,0x90	;switch to SDA AP			
			.db S32K_WRITE_CSW,	0x00,	0x00,0x00,0x00,0x00	;init AP transfer mode
			.db S32K_WRITE_SELECT,	0x00,	0x07,0x00,0x00,0x80	;switch to SDA AP addr 8x			
			.db S32K_WRITE_CSW,	0x00,	0x30,0x00,0x00,0xF0	;init AP transfer mode
			.db S32K_WRITE_SELECT,	0x00,	0x04,0x00,0x00,0x00	;switch to SDA AP addr 9x			


;-------------------------------------------------------------------------------
; check_test
;-------------------------------------------------------------------------------
s32k3_check_test:	ldi		YL,0
			ldi		YH,1
			rcall		swd32_reginit		;set registers for faster output

			ldi		ZL,LOW(s32k3_data_test*2)
			ldi		ZH,HIGH(s32k3_data_test*2)

			rcall		swd32_write_dap_table
			rcall		swd32_write_dap_table
			rcall		swd32_write_dap_table
;			rcall		swd32_write_dap_table

			ldi		XL,S32K_READ_CSW	;read ID code
			rcall		swd32_read_dap


			ldi		XL,S32K_READ_TAR	;read ID code
			rcall		swd32_read_dap


			ldi		XL,S32K_READ_CSW	;read ID code
			rcall		swd32_read_dap


			ldi		XL,S32K_READ_TAR	;read ID code
			rcall		swd32_read_dap


;			ldi		XL,S32K_READ_IDR	;read ID code
;			rcall		swd32_read_dap

;			rcall		swd32_read_drwx		;dummy value


			call		gen_w32
			jmp		main_loop_ok



s32k3_data_test:	;DebugPortStart
			.db S32K_WRITE_ABORT,	0x00,	0x00,0x00,0x00,0x1e	;clear all errors
			.db S32K_WRITE_SELECT,	0x00,	0x06,0x00,0x00,0x00	;switch to MDM AP			
			.db S32K_WRITE_CTRL,	0x00,	0x50,0x00,0x0F,0x00	;init AP transfer mode
;			.db S32K_WRITE_TAR,	0x00,	0xE0,0x00,0xED,0xF0	;DHCSR
;			.db S32K_WRITE_TAR,	0x00,	0x00,0x40,0x00,0x00	;addr


;-------------------------------------------------------------------------------
; erase
;-------------------------------------------------------------------------------
s32k_erase2:		ldi		ZL,LOW(s32k_data_erase2*2)
			ldi		ZH,HIGH(s32k_data_erase2*2)
			ldi		r24,6

s32k_erase2_1:		rcall		swd32_write_dap_table
			dec		r24
			brne		s32k_erase2_1
			ldi		r24,100

s32k_erase2_2:		ldi		ZL,10
			ldi		ZH,0
			call		api_wait_ms
			dec		r24
			breq		s32k_erase2_3
;			rjmp		s32k_erase2_2
			
			rcall		swd32_read_drwx		;dummy value
			mov		r16,r20
			andi		r20,0x80
			brne		s32k_erase2_2
;			andi		r16,0x7f
			mov		r16,r24
			jmp		main_loop
		
s32k_erase2_3:		ldi		r16,0x42			;erase timeout
			jmp		main_loop
			
s32k_data_erase2:	.db S32K_WRITE_TAR,	0x00,	0x40,0x02,0x00,0x00	;FSTAT			
			.db S32K_WRITE_DRW,	0x00,	0x70,0x00,0x00,0x80	;clear flags
			.db S32K_WRITE_TAR,	0x00,	0x40,0x02,0x00,0x04	;FCCOB			
			.db S32K_WRITE_DRW,	0x00,	0x0B,0x00,0x00,0x00	;start erase (was: 49)
			.db S32K_WRITE_TAR,	0x00,	0x40,0x02,0x00,0x00	;FSTAT			
			.db S32K_WRITE_DRW,	0x00,	0x80,0x00,0x00,0x80	;start erase


;-------------------------------------------------------------------------------
; S32K mass erase
;-------------------------------------------------------------------------------
s32k_erase:		ldi		ZL,LOW(s32k_data_erase*2)
			ldi		ZH,HIGH(s32k_data_erase*2)
			ldi		r24,5
s32k_erase_0:		rcall		swd32_write_dap_table
			dec		r24
			brne		s32k_erase_0

			sbi		CTRLPORT,S32K_TRIGGER		

			ldi		r24,20
s32k_erase_2:		ldi		ZL,20
			ldi		ZH,1
			call		api_wait_ms
		
s32k_erase_3:		ldi		XL,S32K_READ_TAR
			rcall		swd32_read_dap		;status register
			ldi		XL,S32K_READ_TAR
			rcall		swd32_read_dap		;control register
			andi		r20,0x01
			breq		s32k_erase_4
			dec		r24
			brne		s32k_erase_2
s32_erase_timeout:	ldi		r16,0x42			;erase timeout
			sts		0x100,r24
			jmp		main_loop

s32_erase_err:		ldi		r16,0x43			;erase timeout
			sts		0x100,r24
			jmp		main_loop

			
s32k_erase_4:		sts		0x100,r24
			jmp		main_loop_ok

s32k_data_erase:	.db S32K_WRITE_ABORT,	0x00,	0x00,0x00,0x00,0x1e	;clear all errors
			.db S32K_WRITE_SELECT,	0x00,	0x01,0x00,0x00,0x00	;switch to MDM-AP
			.db S32K_WRITE_CTRL,	0x00,	0x50,0x00,0x00,0x00	;power up debug interface
			.db S32K_WRITE_CTRL,	0x00,	0x54,0x00,0x00,0x00	;request debug reset
			.db S32K_WRITE_TAR,	0x00,	0x00,0x00,0x00,0x01	;initiate erase			

			

;-------------------------------------------------------------------------------
; S9KEA mass erase
;-------------------------------------------------------------------------------
s9kea_erase:		ldi		ZL,LOW(s9kea_data_erase*2)
			ldi		ZH,HIGH(s9kea_data_erase*2)
			ldi		r24,3
s9kea_erase_0:		rcall		swd32_write_dap_table
			dec		r24
			brne		s9kea_erase_0

			rcall		swd32_read_drwx			;read IDR and check for 0x1C0020
			cpi		r20,0x20
			brne		s32_erase_err
			cpi		r21,0x00
			brne		s32_erase_err
			cpi		r22,0x1c
			brne		s32_erase_err

			sbi		CTRLPORT,S32K_TRIGGER		
			rcall		swd32_write_dap_table		;switch to MDM CTRL/STAT DAP
			rcall		swd32_write_dap_table

			rcall		swd32_reginit			;RESET HIGH

			rcall		s32k_w20ms

			ldi		r24,20
s9kea_erase_1:		ldi		XL,S32K_READ_CSW
			rcall		swd32_read_dap		;status register
			ldi		XL,S32K_READ_CSW
			rcall		swd32_read_dap		;status register
			andi		r20,0x02
			brne		s9kea_erase_1e
			rcall		s32k_w20ms
			dec		r24
			brne		s9kea_erase_1
			rjmp		s32_erase_err	

s9kea_erase_1e:		rcall		swd32_write_dap_table

			ldi		r24,20
s9kea_erase_2:		rcall		s32k_w20ms
		
s9kea_erase_3:		ldi		XL,S32K_READ_TAR
			rcall		swd32_read_dap		;status register
			ldi		XL,S32K_READ_TAR
			rcall		swd32_read_dap		;control register
			andi		r20,0x01
			breq		s32k_erase_4
			dec		r24
			brne		s9kea_erase_2
s9kea_erase_timeout:	ldi		r16,0x42			;erase timeout
			sts		0x100,r24
			jmp		main_loop

s9kea_erase_err:	ldi		r16,0x43			;erase timeout
			sts		0x100,r24
			jmp		main_loop

s9kea_erase_4:		sts		0x100,r24
			jmp		main_loop_ok


s9kea_data_erase:	.db S32K_WRITE_ABORT,	0x00,	0x00,0x00,0x00,0x1e	;clear all errors
			.db S32K_WRITE_SELECT,	0x00,	0x01,0x00,0x00,0xF0	;switch to MDM-AP
			.db S32K_WRITE_CTRL,	0x00,	0x50,0x00,0x00,0x00	;power up debug interface
;			.db S32K_WRITE_CTRL,	0x00,	0x55,0x00,0x00,0x00	;request debug reset
		
			.db S32K_WRITE_SELECT,	0x00,	0x01,0x00,0x00,0x00	;switch to MDM-AP
			.db S32K_WRITE_TAR,	0x00,	0x00,0x00,0x00,0x08	;initiate erase			
			.db S32K_WRITE_TAR,	0x00,	0x00,0x00,0x00,0x01	;initiate erase			


;-------------------------------------------------------------------------------
; S32K unlock
;-------------------------------------------------------------------------------
s32k_unlock:		ldi		ZL,LOW(s32k_data_unlock*2)
			ldi		ZH,HIGH(s32k_data_unlock*2)
			ldi		YL,0
			ldi		YH,1

			sbi		CTRLPORT,S32K_TRIGGER
			cbi		CTRLPORT,S32K_TRIGGER			
			
			ldi		r24,5
s32k_unlock_0:		rcall		swd32_write_dap_table
			dec		r24
			brne		s32k_unlock_0

			rcall		s32k_swrite_drw
			rcall		swd32_write_dap_table
			rcall		s32k_swrite_drw

			rcall		swd32_write_dap_table
			rcall		swd32_write_dap_table

			jmp		main_loop_ok

s32k_swrite_drw:	ld		r20,Y+
			ld		r21,Y+
			ld		r22,Y+
			ld		r23,Y+
			ldi		XL,SWD32_WRITE_DRW
			jmp		swd32_write_dap


s32k_data_unlock:	.db S32K_WRITE_TAR,	0x00,	0x40,0x02,0x00,0x00	;FSTAT			
			.db S32K_WRITE_DRW,	0x00,	0x70,0x00,0x00,0x80	;clear flags			
			.db S32K_WRITE_TAR,	0x00,	0x40,0x02,0x00,0x04	;FCCOB0			
			.db S32K_WRITE_DRW,	0x00,	0x45,0x00,0x00,0x00	;VFYKEY
			.db S32K_WRITE_TAR,	0x00,	0x40,0x02,0x00,0x08	;FCCOB4			
			.db S32K_WRITE_TAR,	0x00,	0x40,0x02,0x00,0x0C	;FCCOB8			
			
			.db S32K_WRITE_TAR,	0x00,	0x40,0x02,0x00,0x00	;FSTAT			
			.db S32K_WRITE_DRW,	0x00,	0x80,0x00,0x00,0x80	;start cmd


;-------------------------------------------------------------------------------
; S32K get MDM status
;-------------------------------------------------------------------------------
s32k_gstatus:		ldi		ZL,LOW(s32k_data_gstatus*2)
			ldi		ZH,HIGH(s32k_data_gstatus*2)
s32k_gstatus_1:		ldi		r24,4

			sbi		CTRLPORT,S32K_TRIGGER
			cbi		CTRLPORT,S32K_TRIGGER			


s32k_gstatus_0:		rcall		swd32_write_dap_table
			dec		r24
			brne		s32k_gstatus_0

			ldi		XL,S32K_READ_CSW	;read status
			rcall		swd32_read_dap

			ldi		XL,S32K_READ_CSW	;read status
			rcall		swd32_read_dap
	
			call		gen_wres

			jmp		main_loop_ok

s32k_data_gstatus:	.db S32K_WRITE_ABORT,	0x00,	0x00,0x00,0x00,0x1e	;clear all errors
			.db S32K_WRITE_SELECT,	0x00,	0x01,0x00,0x00,0x00	;switch to MDM-AP
			.db S32K_WRITE_CTRL,	0x00,	0x50,0x00,0x00,0x00	;power up debug interface
			.db S32K_WRITE_CTRL,	0x00,	0x54,0x00,0x00,0x00	;request debug reset


;-------------------------------------------------------------------------------
; SAMD mass erase
;-------------------------------------------------------------------------------
samd_erase:		ldi		ZL,LOW(samd_data_erase*2)
			ldi		ZH,HIGH(samd_data_erase*2)
			ldi		r24,2

samd_erase_1:		rcall		swd32_write_dap_table
			dec		r24
			brne		samd_erase_1

			ldi		ZL,10
			ldi		ZH,4
			call		api_wait_ms
			jmp		main_loop_ok
			
samd_data_erase:	.db S32K_WRITE_TAR,	0x00,	0x41,0x00,0x20,0x00	;CTRL			
			.db S32K_WRITE_DRW,	0x00,	0x00,0x00,0x00,0x10	;ERASE


;-------------------------------------------------------------------------------
; some wait routines
;-------------------------------------------------------------------------------
s32k_w20ms:		push		ZL
			push		ZH
			ldi		ZL,20
			ldi		ZH,0
			call		api_wait_ms
			pop		ZH
			pop		ZL
			ret
			
s32k_rap0:		ldi		XL,S32K_READ_CSW	;read
			rcall		swd32_read_dap

			ldi		XL,S32K_READ_RDBUFF	;read
			rcall		swd32_read_dap
		
			jmp		gen_w32

