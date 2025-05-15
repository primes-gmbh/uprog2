//###############################################################################
//#										#
//# UPROG2 universal programmer							#
//#										#
//# copyright (c) 2012-2021 Joerg Wolfram (joerg@jcwolfram.de)			#
//#										#
//#										#
//# This program is free software; you can redistribute it and/or		#
//# modify it under the terms of the GNU General Public License			#
//# as published by the Free Software Foundation; either version 3		#
//# of the License, or (at your option) any later version.			#
//#										#
//# This program is distributed in the hope that it will be useful,		#
//# but WITHOUT ANY WARRANTY; without even the implied warranty of		#
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU		#
//# General Public License for more details.					#
//#										#
//# You should have received a copy of the GNU General Public			#
//# License along with this library// if not, write to the			#
//# Free Software Foundation, Inc., 59 Temple Place - Suite 330,		#
//# Boston, MA 02111-1307, USA.							#
//#										#
//###############################################################################

#include <main.h>
#include "exec/s32k311/exec_s32k311.h"
#include "exec/s32k312/exec_s32k312.h"
#include "exec/s32k314/exec_s32k314.h"

int s32k3_offset;


void print_s32k3swd_error(int errc)
{
	printf("\n");
	switch(errc)
	{
		case 0:		set_error("OK",errc);
				break;

		case 0x41:	set_error("(timeout: no ACK)",errc);
				break;

		case 0x42:	set_error("(erase timeout)",errc);
				break;

		case 0x48:	set_error("(bootcode timeout)",errc);
				break;

		case 0x50:	set_error("(wrong ID)",errc);
				break;

		case 0x51:	set_error("(verify error)",errc);
				break;

		case 0x52:	set_error("(program check)",errc);
				break;

		default:	set_error("(unexpected error)",errc);
	}
	print_error();
}


int s32k3_init(void)
{
	int i,e;
	unsigned long addr;

	i=0;
	
	#include "exec/s32k3/init_s32k3_a.inc"

	memory[i++]=255;	//end
	e=prg_comm(0x25F,i,256,0,ROFFSET,0,0,0,0);

	addr=ROFFSET;
	if(e != 0) printf("Err=%02X\n",e);
	if(e != 0) return e;	
	printf("CPU ID  : %02X%02X%02X%02X\n",memory[addr+87],memory[addr+86],memory[addr+85],memory[addr+84]);addr+=4;

	i=0;
	#include "exec/s32k3/init_s32k3_b.inc"
	memory[i++]=255;	//end
	e=prg_comm(0x25F,i,256,0,ROFFSET,0,0,0,1);
	if(e != 0) return e;	

	i=0;
	#include "exec/s32k3/init_s32k3_c.inc"
	memory[i++]=255;	//end
	e=prg_comm(0x25F,i,256,0,ROFFSET,0,0,0,1);

	return e;

}



int prog_s32k3swd(void)
{
	int errc,blocks,i,j;
	unsigned long addr,len,maddr,flen=0;
//	int mass_erase=0;
	int main_prog=0;
	int main_erase=0;
	int main_verify=0;
	int main_readout=0;
	int data_erase=0;
	int data_prog=0;
	int data_verify=0;
	int data_readout=0;
	int utest_prog=0;
	int utest_readout=0;

	int dev_start=0;
	int run_ram=0;
	int unsecure=0;
//	int ignore_id=0;	
	int debug_ram=0;
	int debug_flash=0;
	
	printf("## S32K14x Device\n");


	errc=0;

	if((strstr(cmd,"help")) && ((strstr(cmd,"help") - cmd) == 1))
	{
		printf("-- 5V -- set VDD to 5V\n");
	//	printf("-- ea -- erase all (mass erase)\n");
		printf("-- em -- main flash erase\n");
		printf("-- pm -- main flash program\n");
		printf("-- vm -- main flash verify\n");
		printf("-- rm -- main flash readout\n");
		printf("-- ed -- data flash erase\n");
		printf("-- pd -- data flash program\n");
		printf("-- vd -- data flash verify\n");
		printf("-- rd -- data flash readout\n");
		printf("-- pu -- utest flash program\n");
		printf("-- ru -- utest flash readout\n");

//		printf("-- ii -- ignore ID\n");

		printf("-- rr -- run code in RAM\n");
		printf("-- dr -- debug code in RAM\n");
		printf("-- df -- debug code in FLASH\n");
		printf("-- st -- start device\n");
 		printf("-- d2 -- switch to device 2\n");

		return 0;
	}

	if(find_cmd("5v"))
	{
		errc=prg_comm(0xfb,0,0,0,0,0,0,0,0);	//5V mode
		printf("## using 5V VDD\n");
	}


	if(find_cmd("d2"))
	{
		errc=prg_comm(0x2ee,0,0,0,0,0,0,0,0);	//dev 2
		printf("## switch to device 2\n");
	}

/*
	if(find_cmd("ii"))
	{
		ignore_id=1;
		printf("## ignore ID\n");
	}

	if(find_cmd("un"))
	{
		unsecure=1;
		printf("## unsecure code\n");
	}
*/
	if(find_cmd("rr"))
	{
		if(file_found < 2)
		{
			run_ram = 0;
			printf("## Action: run code in RAM !! DISABLED BECAUSE OF NO FILE !!\n");
		}
		else
		{
			run_ram=1;
			printf("## Action: run code in RAM using %s\n",sfile);
			goto s32k3swd_ORUN;
		}
	}
	else if(find_cmd("dr"))
	{
		if(file_found < 2)
		{
			debug_ram = 0;
			printf("## Action: debug code in RAM !! DISABLED BECAUSE OF NO FILE !!\n");
		}
		else
		{
			debug_ram = 1;
			printf("## Action: debug code in RAM using %s\n",sfile);
			goto s32k3swd_ORUN;
		}
	}
	else if(find_cmd("df"))
	{
		debug_flash = 1;
		printf("## Action: debug code in FLASH\n");
		goto s32k3swd_ORUN;
	}
	else
	{
/*
		if(find_cmd("un"))
		{
			unsecure=1;
			printf("## Action: unsecure device\n");
		}

		if(find_cmd("ea"))
		{
			mass_erase=1;
			printf("## Action: mass erase\n");
		}
*/
		if(find_cmd("em"))
		{
			main_erase=1;
			printf("## Action: main flash erase\n");
		}

		if(find_cmd("ed"))
		{
			data_erase=1;
			printf("## Action: data flash erase\n");
		}

		main_prog=check_cmd_prog("pm","code flash");
		main_verify=check_cmd_verify("vm","code flash");
		main_readout=check_cmd_read("rm","code flash",&main_prog,&main_verify);

		data_prog=check_cmd_prog("pd","data flash");
		data_verify=check_cmd_verify("vd","data flash");
		data_readout=check_cmd_read("rd","data flash",&data_prog,&data_verify);

		utest_prog=check_cmd_prog("pu","utest flash");
		utest_readout=check_cmd_read("ru","utest flash",&utest_prog,&utest_prog);


		if(find_cmd("st"))
		{
			dev_start=1;
			printf("## Action: start device\n");
		}
	}
	printf("\n");

s32k3swd_ORUN:

	//open file if read 
	if((main_readout == 1) || (data_readout == 1) || (utest_readout == 1))
	{
		errc=writeblock_open();
	}

	if(errc > 0) return errc;

	if(dev_start == 0)
	{
		errc=prg_comm(0x1D0,0,16,0,0,0,0,0,0x45);	//init
		if(errc > 0) goto ERR_EXIT;
		printf("JTAG ID : %02X%02X%02X%02X\n",memory[3],memory[2],memory[1],memory[0]);

		errc=s32k3_init();
				
		errc=prg_comm(0x1D1,0,4,0,0,0x08,0xC0,0x2E,0x40);	//get MCRE
//		errc=prg_comm(0x1D1,0,4,0,0,0x00,0x00,0x40,0x20);	//get MCRE
		printf("MCRE    : %02X%02X%02X%02X\n",memory[3],memory[2],memory[1],memory[0]);
		printf("  2M blocks: %d      1M blocks: %d\n",memory[3]>>5,memory[2]>>5);
		printf("512K blocks: %d    256K blocks: %d\n",memory[1]>>6,memory[0]>>6);
				
		//transfer loader to ram
		if((run_ram == 0) && (errc == 0) && ((main_prog == 1) || (data_prog == 1) || (main_erase == 1) || (data_erase == 1)))
		{
			printf("TRANSFER LOADER\n");
			for(j=0;j<512;j++)
			{
				switch(param[10])
				{
					case 311:	memory[j]=exec_s32k311[j]; break;
					case 312:	memory[j]=exec_s32k312[j]; break;
					case 314:	memory[j]=exec_s32k314[j]; break;
					default:	memory[j]=0xff; printf(".");
				}
			}

			addr=param[4];				//RAM start

			errc=prg_comm(0x1b7,0x200,0,0,0,	//write 0,5 K bootloader
				(addr >> 8) & 0xff,
				(addr >> 16) & 0xff,
				(addr >> 24) & 0xff,
				2);

			errc=prg_comm(0x128,8,12,0,0,0,0,0,0);	//set pc + sp	

			errc=prg_comm(0x12b,0,64,0,0,0,0,0,0);	

		}

	}
		
	if((run_ram == 0) && (errc == 0) && (dev_start == 0))
	{
		if((main_erase == 1) && (errc == 0))
		{
			addr=param[0];
			maddr=0;
			blocks = (param[1]) / 0x2000;
			blocks=1;
//			printf("ADDR = %08lx  LEN= %d Sectors\n",addr,blocks);
			
			progress("FLASH0 ERASE ",blocks,0);

			for(i=0;i<blocks;i++)
			{
				//execute erase
				errc=prg_comm(0x153,0,0,0,0,
					0x54,
					(addr >> 8) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 24) & 0xff);
			
				addr+=0x2000;	// sector++

				progress("FLASH0 ERASE ",blocks,i+1);
			}
			printf("\n");

			if(param[6] != 0)
			{
				addr=param[6];
				maddr=0;
				blocks = (param[7]) / 0x2000;
				blocks=1;
//				printf("ADDR = %08lx  LEN= %d Sectors\n",addr,blocks);
			
				progress("FLASH1 ERASE ",blocks,0);

				for(i=0;i<blocks;i++)
				{
					//execute erase
					errc=prg_comm(0x153,0,0,0,0,
						0x54,
						(addr >> 8) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 24) & 0xff);
			
					addr+=0x2000;	// sector++

					progress("FLASH1 ERASE ",blocks,i+1);
				}
				printf("\n");
			}


			if(param[8] != 0)
			{
				addr=param[8];
				maddr=0;
				blocks = (param[9]) / 0x2000;
				blocks=1;
//				printf("ADDR = %08lx  LEN= %d Sectors\n",addr,blocks);
			
				progress("FLASH2 ERASE ",blocks,0);

				for(i=0;i<blocks;i++)
				{
					//execute erase
					errc=prg_comm(0x153,0,0,0,0,
						0x54,
						(addr >> 8) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 24) & 0xff);
			
					addr+=0x2000;	// sector++

					progress("FLASH2 ERASE ",blocks,i+1);
				}
				printf("\n");
			}

			if(param[11] != 0)
			{
				addr=param[11];
				maddr=0;
				blocks = (param[12]) / 0x2000;
				blocks=1;
//				printf("ADDR = %08lx  LEN= %d Sectors\n",addr,blocks);
			
				progress("FLASH3 ERASE ",blocks,0);

				for(i=0;i<blocks;i++)
				{
					//execute erase
					errc=prg_comm(0x153,0,0,0,0,
						0x54,
						(addr >> 8) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 24) & 0xff);
			
					addr+=0x2000;	// sector++

					progress("FLASH3 ERASE ",blocks,i+1);
				}
				printf("\n");
			}
		}

		if((data_erase == 1) && (errc == 0))
		{
			addr=param[2];
			maddr=0;
			blocks = param[3] / 0x2000;
			//blocks=1;
			//printf("ADDR = %08lx  LEN= %d Blocks\n",addr,blocks);
			progress("DFLASH ERASE ",blocks,0);

			for(i=0;i<blocks;i++)
			{
				//execute erase
				errc=prg_comm(0x153,0,0,0,0,
					0x53,
					(addr >> 8) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 24) & 0xff);

				addr+=0x2000;

				progress("DFLASH ERASE ",blocks,i+1);
			}
			printf("\n");
		}
	
		if((main_prog == 1) && (errc == 0))
		{
			addr=param[0];
			maddr=0;
			blocks=param[1]/max_blocksize;
			len=read_block(param[0],param[1],0);		//read flash

			progress("FLASH0 PROG  ",blocks,0);
			printf("ADDR = %08lx  LEN= %d Blocks\n",addr,blocks);

			for(i=0;i<blocks;i++)
			{
				if(must_prog(maddr,max_blocksize) && (errc==0))
				{
					//transfer data
					errc=prg_comm(0x1b7,max_blocksize,0,maddr,0,	//b2 or 1b7
						0x04,0x40,0x20,max_blocksize >> 8);

					//execute prog
					errc=prg_comm(0x153,0,0,0,0,
						0x52,
						(addr >> 8) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 24) & 0xff);

				}
				addr+=max_blocksize;
				maddr+=max_blocksize;
				progress("FLASH0 PROG  ",blocks,i+1);
			}
			printf("\n");

			if(param[6] != 0)
			{
				addr=param[6];
				maddr=0;
				blocks=param[7]/max_blocksize;
				len=read_block(param[6],param[7],0);		//read flash

				progress("FLASH1 PROG  ",blocks,0);
//				printf("ADDR = %08lx  LEN= %d Blocks\n",addr,blocks);

				for(i=0;i<blocks;i++)
				{
					if(must_prog(maddr,max_blocksize) && (errc==0))
					{
						//transfer data
						errc=prg_comm(0x1b7,max_blocksize,0,maddr,0,	//b2 or 1b7
							0x04,0x40,0x20,max_blocksize >> 8);	

						//execute prog
						errc=prg_comm(0x153,0,0,0,0,
							0x52,
							(addr >> 8) & 0xff,
							(addr >> 16) & 0xff,
							(addr >> 24) & 0xff);

					}
					addr+=max_blocksize;
					maddr+=max_blocksize;
					progress("FLASH1 PROG  ",blocks,i+1);
				}
				printf("\n");

			}

			if(param[8] != 0)
			{
				addr=param[8];
				maddr=0;
				blocks=param[9]/max_blocksize;
				len=read_block(param[8],param[9],0);		//read flash

				progress("FLASH2 PROG  ",blocks,0);
//				printf("ADDR = %08lx  LEN= %d Blocks\n",addr,blocks);

				for(i=0;i<blocks;i++)
				{
					if(must_prog(maddr,max_blocksize) && (errc==0))
					{
						//transfer data
						errc=prg_comm(0x1b7,max_blocksize,0,maddr,0,	//b2 or 1b7
							0x04,0x40,0x20,max_blocksize >> 8);	

						//execute prog
						errc=prg_comm(0x153,0,0,0,0,
							0x52,
							(addr >> 8) & 0xff,
							(addr >> 16) & 0xff,
							(addr >> 24) & 0xff);

					}
					addr+=max_blocksize;
					maddr+=max_blocksize;
					progress("FLASH2 PROG  ",blocks,i+1);
				}
				printf("\n");

			}

			if(param[11] != 0)
			{
				addr=param[11];
				maddr=0;
				blocks=param[12]/max_blocksize;
				len=read_block(param[11],param[12],0);		//read flash

				progress("FLASH3 PROG  ",blocks,0);
//				printf("ADDR = %08lx  LEN= %d Blocks\n",addr,blocks);

				for(i=0;i<blocks;i++)
				{
					if(must_prog(maddr,max_blocksize) && (errc==0))
					{
						//transfer data
						errc=prg_comm(0x1b7,max_blocksize,0,maddr,0,	//b2 or 1b7
							0x04,0x40,0x20,max_blocksize >> 8);	

						//execute prog
						errc=prg_comm(0x153,0,0,0,0,
							0x52,
							(addr >> 8) & 0xff,
							(addr >> 16) & 0xff,
							(addr >> 24) & 0xff);

					}
					addr+=max_blocksize;
					maddr+=max_blocksize;
					progress("FLASH3 PROG  ",blocks,i+1);
				}
				printf("\n");
			}
		}


		if(((main_readout == 1) || (main_verify == 1)) && (errc == 0))
		{
			flen=param[1]+param[7]+param[9]+param[12];
			maddr=0;
			addr=param[0];
			blocks=flen/max_blocksize;
			progress("FLASH READ  ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				if(errc==0)
				{
					errc=prg_comm(0x1b6,0,2048,0,ROFFSET+maddr,	//1b6 oder bf
						(addr >> 8) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 24) & 0xff,
						max_blocksize >> 8);					

					addr+=max_blocksize;
					maddr+=max_blocksize;
					progress("FLASH READ  ",blocks,i+1);
				}
			}
			printf("\n");
		}


		if((main_readout == 1) && (errc == 0))
		{
			writeblock_data(0,flen,param[0]);
		}

		//verify main
		if((main_verify == 1) && (errc == 0))
		{
			read_block(param[0],flen,0);
			if(unsecure==1) memory[0x40c]=0xFE;
			printf("VERIFY FLASH (%ld KBytes)\n",flen/1024);
			addr = param[0];
			maddr=0;
			len = flen;
			for(j=0;j<len;j++)
			{
				if(memory[maddr+j] != memory[maddr+j+ROFFSET])
				{
					printf("ERR -> ADDR= %08lX  FILE= %02X  READ= %02X\n",
						addr+j,memory[maddr+j],memory[maddr+j+ROFFSET]);
					errc=0x51;
				}
			}
		}


		if((data_prog == 1) && (errc == 0))
		{
			addr=param[2];
			maddr=0;
			blocks=param[3]/max_blocksize;
			len=read_block(param[2],param[3],0);		//read flash
			progress("DFLASH PROG ",blocks,0);

			for(i=0;i<blocks;i++)
			{
				if(must_prog(maddr,max_blocksize) && (errc==0))
				{
		//			printf("ADDR = %08lx  LEN= %d Blocks\n",addr,blocks);
					//transfer data
					errc=prg_comm(0x1b7,max_blocksize,0,maddr,0,
						0x04,0x40,0x20,max_blocksize >> 8);

					//execute prog
					errc=prg_comm(0x153,0,0,0,0,
						0x52,
						(addr >> 8) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 24) & 0xff);

				}
				addr+=max_blocksize;
				maddr+=max_blocksize;
				progress("DFLASH PROG ",blocks,i+1);
			}
			printf("\n");
		}
		

		if(((data_readout == 1) || (data_verify == 1)) && (errc == 0))
		{
			maddr=0;
			addr=param[2];
			blocks=param[3]/max_blocksize;

			progress("DFLASH READ ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				if(errc==0)
				{
					errc=prg_comm(0x1b6,0,2048,0,ROFFSET+maddr,
						(addr >> 8) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 24) & 0xff,
						max_blocksize >> 8);
					addr+=max_blocksize;
					maddr+=max_blocksize;
					progress("DFLASH READ ",blocks,i+1);
				}
			}
			printf("\n");
		}

		if((data_readout == 1) && (errc == 0))
		{
			writeblock_data(0,param[3],param[2]);
		}

		//verify data
		if((data_verify == 1) && (errc == 0))
		{
			read_block(param[2],param[3],0);
			printf("VERIFY DFLASH (%ld KBytes)\n",param[3]/1024);
			addr = param[2];
			maddr=0;
			len = param[3];
			for(j=0;j<len;j++)
			{
				if(memory[maddr+j] != memory[maddr+j+ROFFSET])
				{
					printf("ERR -> ADDR= %08lX  FILE= %02X  READ= %02X\n",
						addr+j,memory[maddr+j],memory[maddr+j+ROFFSET]);
					errc=0x51;
				}
			}
		}

		if((utest_readout == 1) && (errc == 0))
		{
			maddr=0;
			addr=param[13];
			blocks=param[14]/max_blocksize;

			progress("UTEST READ ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				if(errc==0)
				{
					errc=prg_comm(0x1b6,0,2048,0,ROFFSET+maddr,
						(addr >> 8) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 24) & 0xff,
						max_blocksize >> 8);
					addr+=max_blocksize;
					maddr+=max_blocksize;
					progress("UTEST READ ",blocks,i+1);
				}
			}
			printf("\n");
		}

		if((utest_readout == 1) && (errc == 0))
		{
			writeblock_data(0,param[14],param[13]);
		}


		//open file if was read 
		if((main_readout == 1) || (data_readout == 1) || (utest_readout == 1))
		{
			writeblock_close();
		}
	}


	if((run_ram == 1) && (errc == 0))
	{

	//	printf("ADDR= %08lX\n",param[4]);
	//	printf("LEN= %08lX\n",param[5]);

		len = read_block(param[4],param[5],0);
		printf("BYTES= %04lX\n",len);

		printf("TRANSFER & START CODE\n");
		addr=param[4];
		maddr=0;
		blocks=(len+2047) >> 11;	//param[5]

		progress("TRANSFER ",blocks,0);

		for(i=0;i<blocks;i++)
		{
			errc=prg_comm(0xb2,max_blocksize,0,maddr,0,		//write 1.K
				(addr >> 8) & 0xff,
				(addr >> 16) & 0xff,
				0x20,max_blocksize >> 8);
		
			addr+=max_blocksize;
			maddr+=max_blocksize;
			progress("TRANSFER ",blocks,i+1);
		}


		addr=param[4];

		printf("\nSTART CODE AT 0x%02x%02x%02x%02x\n",memory[7],memory[6],memory[5],memory[4]);
		
		errc=prg_comm(0x128,8,12,0,0,0,0,0,0);	//set pc + sp	

		errc=prg_comm(0x12b,0,100,0,0,0,0,0,0);	
		
		if(errc == 0)
		{
			waitkey();
		}		

	}

	if((debug_ram==1) && (errc==0)) debug_armcortex(0);
	if((debug_flash==1) && (errc==0)) debug_armcortex(1);
	
	if(dev_start == 1)
	{
		i=prg_comm(0x0e,0,0,0,0,1,1,0,0);				//init
		waitkey();
		i=prg_comm(0x0f,0,0,0,0,0,0,0,0);					//exit
	}

ERR_EXIT:

	i=prg_comm(0x91,0,0,0,0,0,0,0,0);					//exit

	prg_comm(0x2ef,0,0,0,0,0,0,0,0);	//dev 1

	print_s32k3swd_error(errc);

	return errc;
}

