//###############################################################################
//#										#
//# UPROG2 universal programmer							#
//#										#
//# copyright (c) 2012-2016 Joerg Wolfram (joerg@jcwolfram.de)			#
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
#include "exec/ppcjtag_84c/exec_ppcjtag.h"
unsigned char jkey[32]; 


void print_ppcjtag3_error(int errc)
{
	printf("\n");
	switch(errc)
	{
		case 0:		set_error("OK",errc);
				break;

		case 0x41:	set_error("(TimeOut)",errc);
				break;

		case 0x42:	set_error("Bootloader: no start",errc);
				break;

		case 0x43:	set_error("(Wrong JTAD ID)",errc);
				break;

		case 0x44:	set_error("(Device is protected)",errc);
				break;

		case 0x45:	set_error("(Verify error)",errc);
				break;

		default:	set_error("(unexpected error)",errc);
	}
	print_error();
}

void ppcjtag3_setcpu(void)
{
	memory[ 0]=0x00;		//WBBRl
	memory[ 1]=0x00;
	memory[ 2]=0x00;
	memory[ 3]=0x00;

	memory[ 4]=0x00;		//WBBRh
	memory[ 5]=0x00;
	memory[ 6]=0x00;
	memory[ 7]=0x00;
		
	memory[ 8]=0x00;		//MSR
	memory[ 9]=0x00;
	memory[10]=0x00;
	memory[11]=0x00;
		
	memory[12]=0xFE;		//PC
	memory[13]=0x00;
	memory[14]=0x80;
	memory[15]=0x52;
		
	memory[16]=0x22;		//IR
	memory[17]=0x44;
	memory[18]=0x22;
	memory[19]=0x44;
		
	memory[20]=0x02;		//CTL
	memory[21]=0x00;
	memory[22]=0x00;
	memory[23]=0x00;

	prg_comm(0x180,24,0,0,0,0,0,0,0);		//-> write CPUSCR
}

//start code in main flash
void ppcjtag3_setcpu2(void)
{
	memory[ 0]=0x00;		//WBBRl
	memory[ 1]=0x00;
	memory[ 2]=0x00;
	memory[ 3]=0x00;

	memory[ 4]=0x00;		//WBBRh
	memory[ 5]=0x00;
	memory[ 6]=0x00;
	memory[ 7]=0x00;
		
	memory[ 8]=0x00;		//MSR
	memory[ 9]=0x00;
	memory[10]=0x00;
	memory[11]=0x00;
		
	memory[12]=0xFE;		//PC
	memory[13]=0x00;
	memory[14]=0xFC;
	memory[15]=0x00;
		
	memory[16]=0x22;		//IR
	memory[17]=0x44;
	memory[18]=0x22;
	memory[19]=0x44;
		
	memory[20]=0x02;		//CTL
	memory[21]=0x00;
	memory[22]=0x00;
	memory[23]=0x00;

	prg_comm(0x180,24,0,0,0,0,0,0,0);		//-> write CPUSCR
}


void ppcjtag3_oncestat(void)
{
	unsigned short stat;
	prg_comm(0x185,0,2,0,0,0,0,0,0);					//read OnCE status register
	stat=memory[0] + (memory[1] << 8);
	printf("OnCE-STAT = %04X\n",stat);	
	prg_comm(0x179,0,0,0,0,0,0,0,0);					//enable Nexus 0
}

int prog_ppcjtag3(void)
{
	int errc,blocks,bsize;
//	unsigned short stat;
	unsigned long addr,maddr,i,j,devid,idreg;
	long len;
	int main_erase=0;
	int main_prog=0;
	int main_verify=0;
	int main_readout=0;
	int data_erase=0;
	int data_prog=0;
	int data_verify=0;
	int data_readout=0;

	int smain_erase=0;
	int smain_prog=0;
	int smain_verify=0;
	int smain_readout=0;
	int sdata_erase=0;
	int sdata_prog=0;
	int sdata_verify=0;
	int sdata_readout=0;

	int utest_prog=0;
	int utest_verify=0;
	int utest_readout=0;


	int dev_start=0;
	int run_ram=0;
	int start_flash=0;
	int write_ram=0;
	int unlock=0;
	int lb0,lb1,lb2,lb3,lb4,lb5,lb6,lb7,lbx;
	char hexbyte[5];
	char *parptr;

	errc=0;

	lb0=0xfe;	//default KEY
	lb1=0xed;
	lb2=0xfa;
	lb3=0xce;
	lb4=0xca;
	lb5=0xfe;
	lb6=0xbe;
	lb7=0xef;


	if((strstr(cmd,"help")) && ((strstr(cmd,"help") - cmd) == 1))
	{
		printf("-- key: -- set key (hex)\n");

		printf("-- em -- main flash erase\n");
		printf("-- pm -- main flash program\n");
		printf("-- vm -- main flash verify\n");
		printf("-- rm -- main flash readout\n");

		printf("-- ed -- data flash erase\n");
		printf("-- pd -- data flash program\n");
		printf("-- vd -- data flash verify\n");
		printf("-- rd -- data flash readout\n");

		printf("-- pu -- UTest flash program\n");
		printf("-- vu -- UTest flash verify\n");
		printf("-- ru -- UTest flash readout\n");

		printf("-- escm -- security main flash erase\n");
		printf("-- pscm -- security main flash program\n");
		printf("-- vscm -- security main flash verify\n");
		printf("-- rscm -- security main flash readout\n");

		printf("-- escd -- security data flash erase\n");
		printf("-- pscd -- security data flash program\n");
		printf("-- vscd -- security data flash verify\n");
		printf("-- rscd -- security data flash readout\n");

		printf("-- rr -- run code in RAM\n");
		printf("-- sf -- start code in Flash\n");
		printf("-- st -- start device\n");
		printf("-- d2 -- switch to device 2\n");

		return 0;
	}

	if(find_cmd("d2"))
	{
		errc=prg_comm(0x2ee,0,0,0,0,0,0,0,0);	//dev 2
		printf("## switch to device 2\n");
	}


	if(find_cmd("5v"))
	{
		errc=prg_comm(0xfb,0,0,0,0,0,0,0,0);	//5V mode
		printf("## using 5V VDD\n");
	}

	if(find_cmd("sf"))
	{
		start_flash=1;
		printf("## Action: start code in flash\n");
	}

	if((strstr(cmd,"key:")) && ((strstr(cmd,"key:") - cmd) % 2 == 1))
	{
		parptr=strstr(cmd,"key:");
		
		for(i=0;i<32;i++)
		{
			strncpy(&hexbyte[0],parptr + (4 + 2 * i) * sizeof(char),2);
			hexbyte[2]=0;
			sscanf(hexbyte,"%x",&jkey[32-i]);
		}

		printf("## Action: unlock device using  key:\n");
		for(i=0;i<4;i++)
		{
			for(j=0;j<8;j++)
			{
				printf(" %02X",jkey[j*4+i]);	
			}
			printf("\n");
		}		
		unlock=1;
	}

	errc=prg_comm(0xfe,0,0,0,0,3,3,0,0);	//enable PU

	if(find_cmd("tr"))
	{
		if(file_found < 2)
		{
			write_ram = 0;
			printf("## Action: write code to RAM !! DISABLED BECAUSE OF NO FILE !!\n");
		}
		else
		{
			write_ram=1;
			printf("## Action: write code to RAM using %s\n",sfile);
		}
	}


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
		}
	}
	else
	{

		if(find_cmd("em"))
		{
			main_erase=1;
			printf("## Action: code flash erase\n");
		}

		if(find_cmd("ed"))
		{
			data_erase=1;
			printf("## Action: data flash erase\n");
		}

		if(find_cmd("escm"))
		{
			smain_erase=1;
			printf("## Action: security code flash erase\n");
		}

		if(find_cmd("escd"))
		{
			sdata_erase=1;
			printf("## Action: security data flash erase\n");
		}


		main_prog=check_cmd_prog("pm","code flash");
		data_prog=check_cmd_prog("pd","data flash");
		smain_prog=check_cmd_prog("pscm","security code flash");
		sdata_prog=check_cmd_prog("pscd","security data flash");
		utest_prog=check_cmd_prog("pu","UTest flash");

		main_verify=check_cmd_verify("vm","code flash");
		data_verify=check_cmd_verify("vd","data flash");
		smain_verify=check_cmd_verify("vscm","security code flash");
		sdata_verify=check_cmd_verify("vscd","security data flash");

		main_readout=check_cmd_read("rm","code flash",&main_prog,&main_verify);
		data_readout=check_cmd_read("rd","data flash",&data_prog,&data_verify);
		smain_readout=check_cmd_read("rscm","security code flash",&smain_prog,&smain_verify);
		sdata_readout=check_cmd_read("rscd","security data flash",&sdata_prog,&sdata_verify);
		utest_readout=check_cmd_read("ru","UTest flash",&utest_prog,&utest_verify);

		if(find_cmd("st"))
		if(strstr(cmd,"st") && ((strstr(cmd,"st") - cmd) %2 == 1))
		{
			dev_start=1;
			printf("## Action: start device\n");
		}
	}
	printf("\n");

	if((main_readout == 1) || (data_readout == 1) || (smain_readout == 1) || (sdata_readout == 1) || (utest_readout == 1))
	{
		errc=writeblock_open();
	}

	if(dev_start == 0)
	{
		printf("INIT DEVICE\n");
		errc=prg_comm(0x181,0,4,0,0,0,0,0,6);					//init
		idreg=memory[0] + (memory[1] << 8) + (memory[2] << 16) + (memory[3] << 24);
		printf("ID-REG    = %08lX\n",idreg);	

		if(unlock == 1)
		{
			printf("TRY TO UNLOCK....\n");
			for(j=0;j<32;j++) memory[j]=jkey[j];
			errc=prg_comm(0x18A,32,0,0,0,7,0,0,0);				//unlock
		}


//		printf("ENABLE OnCE \n");
//		errc=prg_comm(0x188,0,0,0,0,0x28,0,0,6);				//enable OnCE
		errc=prg_comm(0x176,0,6,0,0,0x2A,0,0,6);				//read JTAG ID
		devid=memory[0] + (memory[1] << 8) + (memory[2] << 16) + (memory[3] << 24);

		if(devid != param[10])
		{			
			printf("JTAG ID =%08lX   , must be %08lX\n",devid,param[10]);
			errc=0x43;
			if((devid == 0) && (idreg != 0)) errc=0x44;
			goto ppcjtag3_END;
		}
		else
		{
			printf("JTAG ID   = %08lX\n",devid);	
		}

		printf("HALT CPU\n");
		errc=prg_comm(0x177,0,0,0,0,0,0,0,0);				//init debug mode
		errc=prg_comm(0x186,0,0,0,0,0,0,0,0);				//init debug mode
		ppcjtag3_oncestat();						//print status and enable NEXUS
	}

	if((run_ram == 0) && (start_flash == 0) && (errc == 0) && (dev_start == 0))
	{	
		for(j=0;j<0x500;j++)
		{
			if ((param[12] & 0xf0) == 0x40) memory[j]=exec_ppcjtag_84c[j];
		}
		printf("TRANSFER & EXEC LOADER\n");

		errc=prg_comm(0x17b,2048,0,0,0,0x52,0x80,0x00,0x00);		//write block

		//set a wrong address
		memory[0]=0xFE;
		memory[1]=0xFD;
		memory[2]=0xFC;
		memory[3]=0xFB;
		errc=prg_comm(0x17A,4,0,0,0,0x52,0x80,0x04,0xfc);		//-> write

		ppcjtag3_setcpu();				//set new cpu state
		ppcjtag3_oncestat();				//print status
		errc=prg_comm(0x178,0,0,0,0,0,0,0,0);		//start cpu

		usleep(200000);

		ppcjtag3_oncestat();				//print status


		errc=prg_comm(0x17f,0,4,0,0,0x52,0x80,0x04,0xf8);
		if(memory[0] != 0) 
		{
			show_data(0,4);	
			errc= 0x42;
		}
		
		printf("ERRC=%02X\n",errc);

		if((main_erase == 1) && (errc == 0))
		{
			printf("ERASE MAIN CODE FLASH\n");

			memory[ 0]=0x15;		//ERASE CF
			memory[ 1]=0x00;
			memory[ 2]=0x00;
			memory[ 3]=0x00;
			errc=prg_comm(0x17A,4,0,0,0,0x52,0x80,0x04,0xf8);			//-> write
	
			do{
				errc=prg_comm(0x18f,0,4,0,0,0x52,0x80,0x04,0xf8);
			}while(memory[3] != 0);

		}

		if((data_erase == 1) && (errc == 0))
		{
			printf("ERASE MAIN DATA FLASH\n");
			memory[ 0]=0x17;		//ERASE DF
			memory[ 1]=0x00;
			memory[ 2]=0x00;
			memory[ 3]=0x00;
			errc=prg_comm(0x17A,4,0,0,0,0x52,0x80,0x04,0xf8);			//-> write
	
			do{
				errc=prg_comm(0x17f,0,4,0,0,0x52,0x80,0x04,0xf8);
				if(errc != 0) goto ppcjtag3_END;
			}while(memory[3] != 0);
		}

		if((smain_erase == 1) && (errc == 0))
		{
			printf("ERASE SECURITY CODE FLASH\n");

			memory[ 0]=0x19;		//ERASE CF
			memory[ 1]=0x00;
			memory[ 2]=0x00;
			memory[ 3]=0x00;
			errc=prg_comm(0x17A,4,0,0,0,0x52,0x80,0x04,0xf8);			//-> write
	
			do{
				errc=prg_comm(0x17f,0,4,0,0,0x52,0x80,0x04,0xf8);
				if(errc != 0) goto ppcjtag3_END;
			}while(memory[3] != 0);

		}

		if((sdata_erase == 1) && (errc == 0))
		{
			printf("ERASE SECURITY DATA FLASH\n");
			memory[ 0]=0x1B;		//ERASE DF
			memory[ 1]=0x00;
			memory[ 2]=0x00;
			memory[ 3]=0x00;
			errc=prg_comm(0x17A,4,0,0,0,0x52,0x80,0x04,0xf8);			//-> write
	
			do{
				errc=prg_comm(0x17f,0,4,0,0,0x52,0x80,0x04,0xf8);
				if(errc != 0) goto ppcjtag3_END;
			}while(memory[3] != 0);
		}

	//------------------------------------------------------------------------
	// Program
	//------------------------------------------------------------------------	
		if((utest_prog == 1) && (errc == 0))
		{
			read_block(0,2048,0);					//ADDR starts at 0
			maddr=0;
			addr=0x00400340;					//utest free area

			while(memory[maddr] != 0xff)
			{

				//addr
				memory[ROFFSET+0]=addr & 0xff;
				memory[ROFFSET+1]=(addr >> 8) & 0xff;
				memory[ROFFSET+2]=(addr >> 16) & 0xff;
				memory[ROFFSET+3]=(addr >> 24) & 0xff;	

				errc=prg_comm(0x17A,4,0,ROFFSET,0,0x52,0x80,0x04,0xfc);		//-> write
				if(errc != 0) goto ppcjtag3_END;

						
				errc=prg_comm(0x17e,8,0,maddr,0,0x52,0x80,0x10,0x00);
				if(errc != 0) goto ppcjtag3_END;
				
				memory[ROFFSET+0]=0x1C;		//PROGRAM CF
				memory[ROFFSET+1]=0x00;
				memory[ROFFSET+2]=0x00;
				memory[ROFFSET+3]=0x00;
				errc=prg_comm(0x17A,4,0,ROFFSET,0,0x52,0x80,0x04,0xf8);		//-> write
	
				do{
					errc=prg_comm(0x17f,0,4,0,ROFFSET,0x52,0x80,0x04,0xf8);
					if(errc != 0) goto ppcjtag3_END;
				}while(memory[ROFFSET+3] != 0);
			
				maddr+=8;
				addr+=8;
			}			
			printf("UTEST PROG DONE\n");
		}


		if((main_prog == 1) && (errc == 0))
		{
			read_block(param[0],param[1],0);
			addr=param[0];
			bsize=max_blocksize;
			blocks=param[1] / bsize;
			maddr=0;
			
			progress("MAIN CFLASH PROG ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				if(must_prog(maddr,bsize) && (errc==0))
				{
//					printf("ADDR = %08lX  LEN: %08lX\n",addr,maddr);
					//transfer 2K
					errc=prg_comm(0x17e,2048,0,maddr,0,0x52,0x80,0x10,0x00);
					if(errc != 0) goto ppcjtag3_END;
					
					//set addr
					memory[ROFFSET+3]=(addr >> 24) & 0xff;
					memory[ROFFSET+2]=(addr >> 16) & 0xff;
					memory[ROFFSET+1]=(addr >> 8) & 0xff;
					memory[ROFFSET+0]=(addr) & 0xff;
					errc=prg_comm(0x17A,4,0,ROFFSET,0,0x52,0x80,0x04,0xfc);		//-> write
					if(errc != 0) goto ppcjtag3_END;
				
					memory[ROFFSET+0]=0x0D;		//PROGRAM CF
					memory[ROFFSET+1]=0x00;
					memory[ROFFSET+2]=0x00;
					memory[ROFFSET+3]=0x00;
					errc=prg_comm(0x17A,4,0,ROFFSET,0,0x52,0x80,0x04,0xf8);		//-> write
					if(errc != 0) goto ppcjtag3_END;
	
					do{
						errc=prg_comm(0x17f,0,4,0,ROFFSET,0x52,0x80,0x04,0xf8);
						if(errc != 0) goto ppcjtag3_END;
					}while(memory[ROFFSET+3] != 0);
				}
				addr+=bsize;
				maddr+=bsize;
				progress("MAIN CFLASH PROG ",blocks,i+1);
			}
			printf("\n");
		}

		if((data_prog == 1) && (errc == 0))
		{
			read_block(param[2],param[3],0);
			addr=param[2];
			bsize=max_blocksize;
			blocks=param[3] / bsize;
			maddr=0;
			
			progress("MAIN DFLASH PROG ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				if(must_prog(maddr,bsize) && (errc==0))
				{
//					printf("ADDR = %08lX  LEN: %08lX\n",addr,maddr);
					//transfer 2K
					errc=prg_comm(0x17e,2048,0,maddr,0,0x52,0x80,0x10,0x00);
					if(errc != 0) goto ppcjtag3_END;
					
					//set addr
					memory[ROFFSET+3]=(addr >> 24) & 0xff;
					memory[ROFFSET+2]=(addr >> 16) & 0xff;
					memory[ROFFSET+1]=(addr >> 8) & 0xff;
					memory[ROFFSET+0]=(addr) & 0xff;
					errc=prg_comm(0x17A,4,0,ROFFSET,0,0x52,0x80,0x04,0xfc);		//-> write
					if(errc != 0) goto ppcjtag3_END;
				
					memory[ROFFSET+0]=0x0D;		//PROGRAM CF
					memory[ROFFSET+1]=0x00;
					memory[ROFFSET+2]=0x00;
					memory[ROFFSET+3]=0x00;
					errc=prg_comm(0x17A,4,0,ROFFSET,0,0x52,0x80,0x04,0xf8);		//-> write
					if(errc != 0) goto ppcjtag3_END;
	
					do{
						errc=prg_comm(0x17f,0,4,0,ROFFSET,0x52,0x80,0x04,0xf8);
						if(errc != 0) goto ppcjtag3_END;
					}while(memory[ROFFSET+3] != 0);
				}
				addr+=bsize;
				maddr+=bsize;
				progress("MAIN DFLASH PROG ",blocks,i+1);
			}
			printf("\n");
		}
		

		if((smain_prog == 1) && (errc == 0))
		{
			read_block(param[4],param[5],0);
			addr=param[4];
			bsize=max_blocksize;
			blocks=param[5] / bsize;
			maddr=0;
			
			progress("SECM CFLASH PROG ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				if(must_prog(maddr,bsize) && (errc==0))
				{
//					printf("ADDR = %08lX  LEN: %08lX\n",addr,maddr);
					//transfer 2K
					errc=prg_comm(0x17e,2048,0,maddr,0,0x52,0x80,0x10,0x00);
					if(errc != 0) goto ppcjtag3_END;
					
					//set addr
					memory[ROFFSET+3]=(addr >> 24) & 0xff;
					memory[ROFFSET+2]=(addr >> 16) & 0xff;
					memory[ROFFSET+1]=(addr >> 8) & 0xff;
					memory[ROFFSET+0]=(addr) & 0xff;
					errc=prg_comm(0x17A,4,0,ROFFSET,0,0x52,0x80,0x04,0xfc);		//-> write
					if(errc != 0) goto ppcjtag3_END;
				
					memory[ROFFSET+0]=0x0D;		//PROGRAM CF
					memory[ROFFSET+1]=0x00;
					memory[ROFFSET+2]=0x00;
					memory[ROFFSET+3]=0x00;
					errc=prg_comm(0x17A,4,0,ROFFSET,0,0x52,0x80,0x04,0xf8);		//-> write
					if(errc != 0) goto ppcjtag3_END;
	
					do{
						errc=prg_comm(0x17f,0,4,0,ROFFSET,0x52,0x80,0x04,0xf8);
						if(errc != 0) goto ppcjtag3_END;
					}while(memory[ROFFSET+3] != 0);
				}
				addr+=bsize;
				maddr+=bsize;
				progress("SECM CFLASH PROG ",blocks,i+1);
			}
			printf("\n");
		}


		if((sdata_prog == 1) && (errc == 0))
		{
			read_block(param[6],param[7],0);
			addr=param[6];
			bsize=max_blocksize;
			blocks=param[7] / bsize;
			maddr=0;
			
			progress("SECM DFLASH PROG ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				if(must_prog(maddr,bsize) && (errc==0))
				{
//					printf("ADDR = %08lX  LEN: %08lX\n",addr,maddr);
					//transfer 2K
					errc=prg_comm(0x17e,2048,0,maddr,0,0x52,0x80,0x10,0x00);
					if(errc != 0) goto ppcjtag3_END;
					
					//set addr
					memory[ROFFSET+3]=(addr >> 24) & 0xff;
					memory[ROFFSET+2]=(addr >> 16) & 0xff;
					memory[ROFFSET+1]=(addr >> 8) & 0xff;
					memory[ROFFSET+0]=(addr) & 0xff;
					errc=prg_comm(0x17A,4,0,ROFFSET,0,0x52,0x80,0x04,0xfc);		//-> write
					if(errc != 0) goto ppcjtag3_END;
				
					memory[ROFFSET+0]=0x0D;		//PROGRAM CF
					memory[ROFFSET+1]=0x00;
					memory[ROFFSET+2]=0x00;
					memory[ROFFSET+3]=0x00;
					errc=prg_comm(0x17A,4,0,ROFFSET,0,0x52,0x80,0x04,0xf8);		//-> write
					if(errc != 0) goto ppcjtag3_END;
	
					do{
						errc=prg_comm(0x17f,0,4,0,ROFFSET,0x52,0x80,0x04,0xf8);
						if(errc != 0) goto ppcjtag3_END;
					}while(memory[ROFFSET+3] != 0);
				}
				addr+=bsize;
				maddr+=bsize;
				progress("SECM DFLASH PROG ",blocks,i+1);
			}
			printf("\n");
		}


	//------------------------------------------------------------------------
	// ReadOut
	//------------------------------------------------------------------------	
		if(((utest_readout == 1) || (utest_verify == 1)) && (errc == 0))
		{
			addr=param[13];
			bsize=max_blocksize;
			blocks=param[14] / bsize;
			maddr=0;
			progress("UTEST FLASH READ ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				errc=prg_comm(0x17c,0,bsize,0,ROFFSET+maddr,
					(addr >> 24) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 8) & 0xff,
					(addr) & 0xff);
				addr+=bsize;
				maddr+=bsize;
				progress("UTEST FLASH READ ",blocks,i+1);
				if(errc != 0) goto ppcjtag3_END;
			}
			printf("\n");
		}

		if((utest_readout == 1) && (errc == 0))
		{
			writeblock_data(0,param[14],param[13]);
		}


		if(((smain_readout == 1) || (smain_verify == 1)) && (errc == 0))
		{
			addr=param[4];
			bsize=max_blocksize;
			blocks=param[5] / bsize;
			maddr=0;
			progress("SECM CFLASH READ ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				errc=prg_comm(0x17d,0,bsize,0,ROFFSET+maddr,
					(addr >> 24) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 8) & 0xff,
					(addr) & 0xff);
				addr+=bsize;
				maddr+=bsize;
				progress("SECM CFLASH READ ",blocks,i+1);
				if(errc != 0) goto ppcjtag3_END;
			}
			printf("\n");
		}

		if((smain_readout == 1) && (errc == 0))
		{
			writeblock_data(0,param[5],param[4]);
		}

		//verify main
		if((smain_verify == 1) && (errc == 0))
		{
			read_block(param[4],param[5],0);
			addr = param[4];
			len = param[5];
			i=0;
			printf("SECM CFLASH VERIFY\n");
			for(j=0;j<len;j++)
			{
				if(memory[j] != memory[j+ROFFSET])
				{
					printf("ERR -> ADDR= %08lX  FILE= %02X  READ= %02X\n",
						addr+j,memory[j],memory[j+ROFFSET]);
					errc=0x45;
				}
			}
		}


		if(((sdata_readout == 1) || (sdata_verify == 1)) && (errc == 0))
		{
			addr=param[6];
			bsize=max_blocksize;
			blocks=param[7] / bsize;
			maddr=0;
			progress("SECM DFLASH READ ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				errc=prg_comm(0x17d,0,bsize,0,ROFFSET+maddr,
					(addr >> 24) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 8) & 0xff,
					(addr) & 0xff);
				addr+=bsize;
				maddr+=bsize;
				progress("SECM DFLASH READ ",blocks,i+1);
				if(errc != 0) goto ppcjtag3_END;
			}
			printf("\n");
		}

		if((sdata_readout == 1) && (errc == 0))
		{
			writeblock_data(0,param[7],param[6]);
		}

		//verify main
		if((sdata_verify == 1) && (errc == 0))
		{
			read_block(param[6],param[7],0);
			addr = param[6];
			len = param[7];
			i=0;
			printf("SECM DFLASH VERIFY\n");
			for(j=0;j<len;j++)
			{
				if(memory[j] != memory[j+ROFFSET])
				{
					printf("ERR -> ADDR= %08lX  FILE= %02X  READ= %02X\n",
						addr+j,memory[j],memory[j+ROFFSET]);
					errc=0x45;
				}
			}
		}


		if(((data_readout == 1) || (data_verify == 1)) && (errc == 0))
		{
			addr=param[2];
			bsize=max_blocksize;
			blocks=param[3] / bsize;
			maddr=0;
			progress("MAIN DFLASH READ ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				errc=prg_comm(0x17d,0,bsize,0,ROFFSET+maddr,
					(addr >> 24) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 8) & 0xff,
					(addr) & 0xff);
				addr+=bsize;
				maddr+=bsize;
				progress("MAIN DFLASH READ ",blocks,i+1);
				if(errc != 0) goto ppcjtag3_END;
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
			addr = param[2];
			len = param[3];
			i=0;
			printf("MAIN DFLASH VERIFY\n");
			for(j=0;j<len;j++)
			{
				if(memory[j] != memory[j+ROFFSET])
				{
					printf("ERR -> ADDR= %08lX  FILE= %02X  READ= %02X\n",
						addr+j,memory[j],memory[j+ROFFSET]);
					errc=0x45;
				}
			}
		}


		if(((main_readout == 1) || (main_verify == 1)) && (errc == 0))
		{
			addr=param[0];
			bsize=max_blocksize;
			blocks=param[1] / bsize;
			maddr=0;
			progress("MAIN CFLASH READ ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				errc=prg_comm(0x17d,0,bsize,0,ROFFSET+maddr,
					(addr >> 24) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 8) & 0xff,
					(addr) & 0xff);
				addr+=bsize;
				maddr+=bsize;
				progress("MAIN CFLASH READ ",blocks,i+1);
				if(errc != 0) goto ppcjtag3_END;
			}
			printf("\n");
		}

		if((main_readout == 1) && (errc == 0))
		{
			writeblock_data(0,param[1],param[0]);
		}

		//verify main
		if((main_verify == 1) && (errc == 0))
		{
			read_block(param[0],param[1],0);
			addr = param[0];
			len = param[1];
			i=0;
			printf("MAIN CFLASH VERIFY\n");
			for(j=0;j<len;j++)
			{
				if(memory[j] != memory[j+ROFFSET])
				{
					printf("ERR -> ADDR= %08lX  FILE= %02X  READ= %02X\n",
						addr+j,memory[j],memory[j+ROFFSET]);
					errc=0x45;
				}
			}
		}

	}


	//------------------------------------------------------------------------
	// Run code in RAM
	//------------------------------------------------------------------------	
	if((run_ram == 1) && (errc == 0))
	{
		len=read_block(param[8],param[9],0);
		if (len < 1) len=read_block(0,param[9],0);
		if (len < 1 ) goto ppcjtag3_END;

		printf("TRANSFER & START CODE\n");

		len+=2;
		printf("## transfer size: %ld bytes\n",len);

		blocks=(len + max_blocksize -1)/max_blocksize;
		i=0;

//		show_data(0x100,16);

		progress("TRANSFER ",blocks,0);

		maddr=0x00000000;	//mem addr
		addr =param[8];		//µC addr
		
//		waitkey();
		
		while(len > 0)
		{
//			printf("ADDR = %08lX  LEN: %04X\n",addr,len);
			bsize=max_blocksize;
			errc=prg_comm(0x17e,bsize,0,maddr,0,
						(addr >> 24) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 8) & 0xff,
						(addr) & 0xff);
	
			progress("TRANSFER ",blocks,i+1);
			maddr+=bsize;
			addr+=bsize;
			len-=bsize;
			i++;
		}

		printf("\nSET PC & GO\n");
		ppcjtag3_setcpu();				//set new cpu state
		printf("START CPU\n");
//		ppcjtag3_oncestat();				//print status
		errc=prg_comm(0x178,0,0,0,0,0,0,0,0);		//start cpu

		usleep(200000);

//		ppcjtag3_oncestat();				//print status

		if(errc == 0)
		{
			waitkey();
		}
	}


	//------------------------------------------------------------------------
	// write code to RAM
	//------------------------------------------------------------------------	
	if((write_ram == 1) && (errc == 0))
	{
		len=read_block(param[8],param[9],0);
		if (len < 1) len=read_block(0,param[9],0);
		if (len < 1 ) goto ppcjtag3_END;

		printf("TRANSFER CODE\n");

		len+=2;
		printf("## transfer sizes: %ld bytes\n",len);

		blocks=(len + max_blocksize -1)/max_blocksize;
		i=0;

//		show_data(0x100,16);

		progress("TRANSFER ",blocks,0);

		maddr=0x00000000;		//mem addr
		addr =0x52800000;		///µC addr
		
		while(len > 0)
		{
			printf("ADDR = %08lX  LEN: %04lX\n",addr,len);
			bsize=max_blocksize;
			errc=prg_comm(0x17e,bsize,0,maddr,0,
						(addr >> 24) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 8) & 0xff,
						(addr) & 0xff);
	
			progress("TRANSFER ",blocks,i+1);
			maddr+=bsize;
			addr+=bsize;
			len-=bsize;
			i++;
		}

	}


	//------------------------------------------------------------------------
	// start code to FLASH aur 0x00FC0100
	//------------------------------------------------------------------------	
	if((start_flash == 1) && (errc == 0))
	{

		printf("START CODE IN FLASH\n");
		ppcjtag3_setcpu2();				//set new cpu state
		errc=prg_comm(0x178,0,0,0,0,0,0,0,0);		//start cpu

		usleep(200000);

		if(errc == 0)
		{
			waitkey();
		}
	}



	if((main_readout == 1) || (data_readout == 1) || (smain_readout == 1) || (sdata_readout == 1) || (utest_readout == 1))
	{
		i=writeblock_close();
	}

	if(dev_start == 1)
	{
		i=prg_comm(0x0e,0,0,0,0,0,0,0,0);		//init
		i=prg_comm(0x1e4,0,0,0,0,0,0,0,0x10);		//reset 0
		i=prg_comm(0x1e3,0,0,0,0,0,0,0,0x10);		//reset out
		i=prg_comm(0x1e1,0,0,0,0,0,0,0,0x10);		//reset 1
		i=prg_comm(0x1e5,0,0,0,0,0,0,0,0x10);		//reset in
		waitkey();					//exit
	}

ppcjtag3_END:

	i=prg_comm(0x0f,0,0,0,0,0,0,0,0);			//exit

	prg_comm(0x2ef,0,0,0,0,0,0,0,0);	//dev 1

	print_ppcjtag3_error(errc);
	return errc;
}


