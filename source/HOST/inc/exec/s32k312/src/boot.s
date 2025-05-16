################################################################################
#                                                                              #
#   Bootloader for S32K                                                        #
#                                                                              #
################################################################################
		.include "inc/regdefs.asm"
		
		.text
		.thumb
		.cpu cortex-m4

		.org 0
	
reset_vector:
		.word 0x20400FFC	// start SP
		.word 0x20400009	// start PC

main_start:
################################################################################
# init clock system 
################################################################################
			cpsie	i			//disable interrupts

			bl	unlock_all		//unlock all sectors
			
################################################################################
# the main loop
################################################################################
main_loop:		ldr	r1, =0x20400c00		// CMD/ADDR
			mov	r2, #0
			str	r2, [r1,#0]
			
main_loop_wait:		ldr	r0, [r1,#0]
			cmp	r0,r2
		#	ldr	r0, [r1,#0]
			beq	main_loop_wait

			mov	r3,r0
			ldr	r2, =0xFFFFFF00
			and	r3,r2			// this is our address
			mov	r2,#0xFF
			and	r0,r2			// this is our cmd

			cmp	r0,#0x52		// prog flash
			beq	prog_flash

			cmp	r0,#0x53		// erase sector
			beq	erase_sector

			cmp	r0,#0x54		// erase block
			beq	erase_block

			cmp	r0,#0x51		// read flash
			beq	read_flash

			b	main_loop


################################################################################
# program flash 
################################################################################
prog_flash:		ldr	r4,=0x20400400		// buffer address
			ldr	r7,=0x10		// rows to do
prog_flash_1:		bl	prog_row
			sub	r7,#1
			bne	prog_flash_1
			b	main_loop


################################################################################
# erase sector
# r3 = address 
################################################################################
erase_sector:		ldr	r5, =PFLASH_PEADRL
			str	r3,[r5,#0]		//store sector addr
			
			ldr	r5, =C40ASF_DATA
			str	r3,[r5,#0]		//dummy write data for interlock
			
			ldr	r5, =C40ASF_BASE
			ldr	r1, =0x10		//set ERS
			str	r1,[r5,#0]
			ldr	r1, =0x11		//set EHV
			str	r1,[r5,#C40ASF_MCR]
			
			ldr	r2, = 0x8000		//done flag
		
erase_sector_wait:	ldr	r1, [r5,#C40ASF_MCRS]	//wait until done
			and	r1, r2
			beq	erase_sector_wait
			
			ldr	r1, =0x10		//clear EHV
			str	r1,[r5,#0]
			ldr	r1, =0x00		//clear ERS
			str	r1,[r5,#C40ASF_MCR]
			
			b	main_loop


################################################################################
# erase block
# r3 = address 
################################################################################
erase_block:		ldr	r5, =PFLASH_PEADRL
			str	r3,[r5,#0]		//store sector addr
			
			ldr	r5, =C40ASF_DATA
			str	r3,[r5,#0]		//dummy write data for interlock
			
			ldr	r5, =C40ASF_BASE
			ldr	r1, =0x30		//set ERS
			str	r1,[r5,#0]
			ldr	r1, =0x31		//set EHV
			str	r1,[r5,#C40ASF_MCR]
			
			ldr	r2, = 0x8000		//done flag
		
erase_block_wait:	ldr	r1, [r5,#C40ASF_MCRS]	//wait until done
			and	r1, r2
			beq	erase_block_wait
			
			ldr	r1, =0x30		//clear EHV
			str	r1,[r5,#0]
			ldr	r1, =0x00		//clear ERS
			str	r1,[r5,#C40ASF_MCR]
			
			b	main_loop

	
################################################################################
# program 128 bytes
# r3 = address
# r4 = pointer to SRAM 
################################################################################
prog_row:	
			ldr	r5, =PFLASH_PEADRL
			str	r3,[r5,#0]		//store addr

			ldr	r5, =C40ASF_DATA
			mov	r0,#32			//LONGS

			
prog_row_loop:		ldr	r1, [r4,#0]
			str	r1,[r5,#0]		//write data
			add	r4,#4
			add	r5,#4
			sub	r0,#1
			bne	prog_row_loop

			ldr	r5, =C40ASF_BASE
			ldr	r1, =0xFFFFFFFF
			str	r1, [r5,#C40ASF_MCRS]	//clear all flags
			ldr	r1, =0x100		//set PGM
			str	r1,[r5,#0]
			ldr	r1, =0x101		//set EHV
			str	r1,[r5,#C40ASF_MCR]
			
			ldr	r2, = 0x8000		//done flag
		
prog_row_wait:		ldr	r1, [r5,#C40ASF_MCRS]	//wait until done
			and	r1, r2
			beq	prog_row_wait
			
			ldr	r1, =0x100		//clear EHV
			str	r1,[r5,#0]
			ldr	r1, =0x00		//clear PGM
			str	r1,[r5,#C40ASF_MCR]
		
			add	r3,#128
			
			bx	lr
			
################################################################################
# read out 2K Flash
################################################################################
read_flash:		ldr	r4,=0x20400400		// buffer address
			ldr	r7,=0x200		// LW to do
read_flash_1:		ldr	r2,[r3,#0]
			str	r2,[r4,#0]
			add	r3,#4
			add	r4,#4
			sub	r7,#1
			bne	read_flash_1
			b	main_loop


################################################################################
# wait for FLASH RDY
################################################################################
wait_fl_rdy:		ldr	r5, =C40ASF_BASE		
			ldrb	r2, [r5,#C40ASF_MCRS]		//wait until 
			ldr	r1, =0x8000		//DONE
			and	r2, r1
			beq	wait_fl_rdy
			b	main_loop
			

################################################################################
# unlock all flash
################################################################################
unlock_all:		ldr	r5, =PFLASH_PEADRL	//flash addr
			mov	r1,#0
			str	r1,[r5,#PFLASH_SPELOCK0]		//unlock block 0
	//		str	r1,[r5,#PFLASH_SPELOCK1]		//unlock block 1
	//		str	r1,[r5,#PFLASH_SPELOCK2]		//unlock block 2
	//		str	r1,[r5,#PFLASH_SPELOCK3]		//unlock block 3
	//		str	r1,[r5,#PFLASH_SPELOCK4]		//unlock block 4 (DF) 
			str	r1,[r5,#PFLASH_SSPELOCK0]		//unlock
			str	r1,[r5,#PFLASH_SSPELOCK1]		//unlock
			str	r1,[r5,#PFLASH_SSPELOCK2]		//unlock
	//		str	r1,[r5,#PFLASH_SSPELOCK3]		//unlock

			ldr	r1,=0xFC000000
			str	r1,[r5,#PFLASH_SPELOCK1]		//unlock block 1

			ldr	r1,=0xFFFF0000
			str	r1,[r5,#PFLASH_SPELOCK2]		//unlock DF


			bx	lr

			.align 4

		