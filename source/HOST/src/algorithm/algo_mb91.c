//###############################################################################
//#										#
//# UPROG2 universal programmer							#
//#										#
//# copyright (c) 2012-2022 Joerg Wolfram (joerg@jcwolfram.de)			#
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
#include "exec/mb91/exec_f522.h"
#include "exec/mb91/exec_f523.h"
#include "exec/mb91/exec_f524.h"
#include "exec/mb91/exec_f525.h"
#include "exec/mb91/exec_f526.h"
#include "exec/mb91/exec_f527.h"
#include "exec/mb91/exec_f528.h"


void print_mb91_error(int errc)
{
	printf("\n");
	switch(errc)
	{
		case 0:		set_error("OK",errc);
				break;

		case 0x41:	set_error("(password: no echo)",errc);
				break;

		case 0x42:	set_error("(password: wrong echo)",errc);
				break;

		case 0x43:	set_error("(data: no echo)",errc);
				break;

		case 0x44:	set_error("(data: wrong echo)",errc);
				break;

		default:	set_error("(unexpected error)",errc);
	}
	print_error();
}


int prog_mb91(void)
{
	int errc,blocks,bsize;
	unsigned long addr,maddr,i,j,csum;
	long len;
	int main_erase=0;
	int main_prog=0;
	int main_verify=0;
	int main_readout=0;
	int dev_start=0;
	int run_ram=0;
	int dflash_erase=0;
	int dflash_prog=0;
	int dflash_verify=0;
	int dflash_readout=0;
	int lb0,lb1,lb2,lb3,lb4,lb5,lb6,lb7;
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
		printf("-- 5V -- set VDD to 5V\n");
		printf("-- key: -- set key (hex)\n");

		printf("-- em -- main flash erase\n");
		printf("-- pm -- main flash program\n");
		printf("-- vm -- main flash verify\n");
		printf("-- rm -- main flash readout\n");

		printf("-- ed -- data flash erase\n");
		printf("-- pd -- data flash program\n");
		printf("-- vd -- data flash verify\n");
		printf("-- rd -- data flash readout\n");

		printf("-- rr -- run code in RAM\n");
		printf("-- st -- start device\n");
		return 0;
	}

	if(find_cmd("5v"))
	{
		errc=prg_comm(0xfb,0,0,0,0,0,0,0,0);	//5V mode
		printf("## using 5V VDD\n");
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
			dflash_erase=1;
			printf("## Action: data flash erase\n");
		}

		main_prog=check_cmd_prog("pm","code flash");
		dflash_prog=check_cmd_prog("pd","data flash");

		main_verify=check_cmd_verify("vm","code flash");
		dflash_verify=check_cmd_verify("vd","data flash");

		main_readout=check_cmd_read("rm","code flash",&main_prog,&main_verify);
		dflash_readout=check_cmd_read("rd","data flash",&dflash_prog,&dflash_verify);

		if(find_cmd("st"))
		if(strstr(cmd,"st") && ((strstr(cmd,"st") - cmd) %2 == 1))
		{
			dev_start=1;
			printf("## Action: start device\n");
		}
	}
	printf("\n");

	if((main_readout == 1) || (dflash_readout == 1))
	{
		errc=writeblock_open();
	}

	printf("INIT DEVICE \n");
	if(dev_start == 0)
	{
		errc=prg_comm(0x1A0,0,0,0,0,0,0,(param[11] >> 8) & 0xff,param[11] & 0xff);					//init
		if(errc != 0) goto MB91_EXIT;
	}

	waitkey();

	if((run_ram == 0) && (errc == 0) && (dev_start == 0))
	{
		printf("TRANSFER LOADER\n");
		
//		show_data(0x100,16);
		
		errc=prg_comm(0x92,0x410,0,0x0f0,0,param[6] & 0xff,0,0x10,0x04);		//transfer loader & exec

		printf("SPD=%02X\n",param[6] & 0xff);
		

		sleep(1);

//		printf("ERRC=%02X\n",errc);


		if((main_erase == 1) && (errc == 0))
		{
			printf("ERASE CODE FLASH\n");
			errc=prg_comm(0x95,0,0,0,0,0,0x03,0,0x3f);				//erase
		}

		if((dflash_erase == 1) && (errc == 0))
		{
			printf("ERASE DATA FLASH\n");
			errc=prg_comm(0x97,0,0,0,0,0x00,0x00,0x00,0x0f);				//erase
		}

		if((main_prog == 1) && (errc == 0))
		{
			read_block(param[0],param[1],0);
			addr=param[0];
			bsize=max_blocksize;
			blocks=param[1] / bsize;
			maddr=0;
			
			progress("CFLASH PROG ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				if(must_prog(maddr,bsize) && (errc==0))
				{
					errc=prg_comm(0x94,bsize,0,maddr,0,
						(addr >> 24) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 8) & 0xff,
						(addr) & 0xff);
				}
				addr+=bsize;
				maddr+=bsize;
				progress("CFLASH PROG ",blocks,i+1);
			}
			printf("\n");
		}

		if((dflash_prog == 1) && (errc == 0) && (param[3]>0))
		{
			read_block(param[2],param[3],0);
			addr=param[2];
			bsize=max_blocksize;
			blocks=param[3] / bsize;
			maddr=0;
			progress("DFLASH PROG ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				if(must_prog(maddr,bsize) && (errc==0))
				{
					errc=prg_comm(0x96,bsize,0,maddr,0,
						(addr >> 24) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 8) & 0xff,
						(addr) & 0xff);
				}
				addr+=bsize;
				maddr+=bsize;
				progress("DFLASH PROG ",blocks,i+1);
			}
			printf("\n");
		}


		if(((main_readout == 1) || (main_verify == 1)) && (errc == 0))
		{
			addr=param[0];
			bsize=max_blocksize;
			blocks=param[1] / bsize;
			maddr=0;
			progress("CFLASH READ ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				errc=prg_comm(0x93,0,bsize,0,ROFFSET+maddr,
					(addr >> 24) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 8) & 0xff,
					(addr) & 0xff);
				addr+=bsize;
				maddr+=bsize;
				progress("CFLASH READ ",blocks,i+1);
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
			printf("CFLASH VERIFY\n");
			for(j=0;j<len;j++)
			{
				if(memory[j] != memory[j+ROFFSET])
				{
					printf("ERR -> ADDR= %08lX  FILE= %02X  READ= %02X\n",
						addr+j,memory[j],memory[j+ROFFSET]);
					errc=1;
				}
			}
		}

		if(((dflash_readout == 1) || (dflash_verify == 1)) && (errc == 0) && (param[3]>0))
		{
			addr=param[2];
			bsize=max_blocksize;
			blocks=param[3] / bsize;
			maddr=0;
			progress("DFLASH READ ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				errc=prg_comm(0x93,0,bsize,0,ROFFSET+maddr,
					(addr >> 24) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 8) & 0xff,
					(addr) & 0xff);
				addr+=bsize;
				maddr+=bsize;
				progress("DFLASH READ ",blocks,i+1);
			}
			printf("\n");
		}

		if((dflash_readout == 1) && (errc == 0) && (param[3]>0))
		{
			writeblock_data(0,param[3],param[2]);
		}

		//verify dflash
		if((dflash_verify == 1) && (errc == 0) && (param[3]>0))
		{
			read_block(param[2],param[3],0);
			addr = param[2];
			len = param[3];
			i=0;
			printf("DFLASH VERIFY\n");
			for(j=0;j<len;j++)
			{
				if(memory[j] != memory[j+ROFFSET])
				{
					printf("ERR -> ADDR= %08lX  FILE= %02X  READ= %02X\n",
						addr+j,memory[j],memory[j+ROFFSET]);
					errc=1;
				}
			}
		}

	}


	if((run_ram == 1) && (errc == 0))
	{
		len=read_block(param[8],param[9],0);

		printf("TRANSFER & START CODE\n");
		printf("## transfer size: %ld bytes\n",len);

		csum=0;

		for(i=0;i<len;i++)
		{
			csum += memory[i];
//			printf("%02X -> %04lX\n",memory[i],csum);
		}

		errc=prg_comm(0x1A1,0,0,0,0,0,0,len & 0xff,(len >> 8) & 0xff);			//transfer head

		blocks=(len + max_blocksize -1)/max_blocksize;
		i=0;

//		show_data(0x0000,16);

		progress("TRANSFER ",blocks,0);

		maddr=0;
		
		while(len > 0)
		{
			bsize=max_blocksize;
			if(bsize > len) bsize=len;
			errc=prg_comm(0x1A2,bsize,0,maddr,0,param[6] & 0xff,0,bsize & 0xff,(bsize >> 8) & 0xff);			//transfer & exec
			progress("TRANSFER ",blocks,i+1);
			maddr+=bsize;
			len-=bsize;
			i++;
		}

		errc=prg_comm(0x1A3,0,0,0,0,0,0,0,csum & 0xff);		//transfer end
		errc=prg_comm(0x1A4,0,0,0,0,0,0,0,0);			//execute

		waitkey();

	}

	if((main_readout == 1) || (dflash_readout == 1))
	{
		i=writeblock_close();
	}

	if(dev_start == 1)
	{
		i=prg_comm(0x0e,0,0,0,0,0,0,0,0);		//init
		i=prg_comm(0x1e4,0,0,0,0,0,0,0,0x01);		//reset 0
		i=prg_comm(0x1e3,0,0,0,0,0,0,0,0x01);		//reset out
		i=prg_comm(0x1e1,0,0,0,0,0,0,0,0x01);		//reset 1
		i=prg_comm(0x1e5,0,0,0,0,0,0,0,0x01);		//reset in
		waitkey();					//exit
	}

MB91_EXIT:

	i=prg_comm(0x1A7,0,0,0,0,0,0,0,0);			//exit


	print_mb91_error(errc);
	return errc;
}




