//###############################################################################
//#										#
//# UPROG universal programmer							#
//#										#
//# copyright (c) 2010-2015 Joerg Wolfram (joerg@jcwolfram.de)			#
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
#include "exec/rl78_dump/rl78-dump-dflash.h"
#include "exec/rl78_fdump/rl78-dump-flash.h"

void print_rl78_error(int errc,unsigned long addr)
{
	printf("\n");
	switch(errc)
	{
		case 0:		set_error("OK",errc);
				break;

		case 0x41:	set_error("(TIMEOUT at SYNC)",errc);
				break;

		case 0x42:	set_error("(wrong sync)",errc);
				break;

		case 0x43:	set_error("(TIMEOUT RESET)",errc);
				break;

		case 0x40:	set_error("(WRONG answer)",errc);
				break;

		case 0x45:	set_error2("(TIMEOUT ACK, CBLOCK)",errc,addr);
				break;

		case 0x46:	set_error2("(NO ACK, CBLOCK)",errc,addr);
				break;

		case 0x47:	set_error2("(VERIFY FAILED)",errc,addr);
				break;

		case 0x48:	set_error2("(TIMEOUT ACK, DBLOCK)",errc,addr);
				break;

		case 0x49:	set_error2("(NO ACK, DBLOCK)",errc,addr);
				break;

		case 0x4A:	set_error2("(BLOCK NOT BLANK)",errc,addr);
				break;

		default:	set_error("(unexpected error)",errc);
	}
	print_error();
}

int prog_rl78(void)
{
	int errc,blocks,tblock,bsize,j;
	unsigned long addr=0,maddr;
	int main_erase=0;
	int main_prog=0;
	int main_verify=0;
	int main_bcheck=0;
	int main_csum=0;
	int main_dump=0;
	int dflash_erase=0;
	int dflash_prog=0;
	int dflash_verify=0;
	int dflash_bcheck=0;
	int dflash_dump=0;
	int dflash_csum=0;
	int unsec=0;
	int rsig=0;
	int gsec=0;

	int dev_start=0;
	errc=0;
	unsigned short checksum[1024];
	unsigned long mcsum;


	if((strstr(cmd,"help")) && ((strstr(cmd,"help") - cmd) == 1))
	{
		printf("-- 5V -- set VDD to 5V\n");
		printf("-- rs -- read silicon signature\n");
		
		printf("-- gs -- get security\n");
		printf("-- un -- unsecure device\n");

		printf("-- em -- main flash erase\n");
		printf("-- bm -- main flash blank check\n");
		printf("-- pm -- main flash program\n");
		printf("-- vm -- main flash verify\n");
		printf("-- cm -- main flash checksum\n");
		printf("-- dm -- main flash dump (will erase first 2K of main)\n");
		
		printf("-- ed -- data flash erase\n");
		printf("-- bd -- data flash blank check\n");
		printf("-- pd -- data flash program\n");
		printf("-- vd -- data flash verify\n");
		printf("-- cd -- data flash checksum\n");
		printf("-- dd -- data flash dump (will erase first 2K of main)\n");

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

	if(find_cmd("rs"))
	{
		rsig=1;
		printf("## Action: read silicon signature\n");
	}

	if(find_cmd("dm"))
	{
		main_dump=1;
		printf("## Action: main flash dump\n");
		goto ONLY_DD;	
	}

	if(find_cmd("dd"))
	{
		dflash_dump=1;
		printf("## Action: data flash dump\n");
		goto ONLY_DD;	
	}


	if(find_cmd("em"))
	{
		main_erase=1;
		printf("## Action: main flash erase\n");
	}

	if(find_cmd("bm"))
	{
		main_bcheck=1;
		printf("## Action: main flash blank check\n");
	}

	if(find_cmd("ed"))
	{
		dflash_erase=1;
		printf("## Action: data flash erase\n");
	}

	if(find_cmd("cm"))
	{
		main_csum=1;
		printf("## Action: main flash checksum calculation\n");
	}

	if(find_cmd("cd"))
	{
		dflash_csum=1;
		printf("## Action: data flash checksum calculation\n");
	}

	if(find_cmd("bd"))
	{
		dflash_bcheck=1;
		printf("## Action: data flash blank check\n");
	}

	if(find_cmd("un"))
	{
		unsec=1;
		printf("## Action: release security\n");
	}

	if(find_cmd("gs"))
	{
		gsec=1;
		printf("## Action: get security\n");
	}


	main_prog=check_cmd_prog("pm","code flash");
	dflash_prog=check_cmd_prog("pd","data flash");

	main_verify=check_cmd_verify("vm","code flash");
	dflash_verify=check_cmd_verify("vd","data flash");

ONLY_DD:

	if((strstr(cmd,"st")) && ((strstr(cmd,"st") - cmd) % 2 == 1))
	{
		dev_start=1;
		printf("## Action: start device\n");
	}

	printf("\n");

	errc=0;

	prg_comm(0xfe,0,0,0,0,3,3,0,0);	//enable PU

	if(dev_start == 0)
	{
		printf("INIT\n");
		prg_comm(0x69,0,0,0,0,0,0,0,0);	//exit
		usleep(100);
		errc=prg_comm(0x68,0,7,0,0,0,0,0,0);	//init
		if(errc != 0)
		{
			printf("RESP(%02X) = %02X %02X %02X %02X %02X\n",errc,memory[2],memory[3],memory[4],memory[5],memory[6]);
		}
//		printf("RD= %02X %02X %02X %02X %02X \n",memory[0],memory[1],memory[2],memory[3],memory[4]);
	}
	
	//erase
	if ((errc == 0) && (main_erase == 1))
	{
		blocks=param[1] / 1024;
		addr=param[0];
		printf("ERASE %d BLOCKS OF MAIN FLASH\n",blocks);


		progress("MAIN ERASE   ",blocks,0);
		
		for(tblock=0;tblock<blocks;tblock++)
		{
			errc=prg_comm(0x6a,0,7,0,0,(addr >> 8) & 0xff,(addr >> 16) & 0xff,1,0);	//program

			if(errc!=0) 
			{
				printf("RESP = %02X %02X %02X %02X %02X\n",memory[2],memory[3],memory[4],memory[5],memory[6]);		
			}
			addr+=1024;
			progress("MAIN ERASE   ",blocks,tblock+1);
		}
		printf("\n");

/*
		errc=prg_comm(0x6a,0,7,0,0,(addr >> 8) & 0xff,(addr >> 16) & 0xff,blocks,0);	//program
		if(errc != 0)
		{
			printf("%d: RESP(%02X) = %02X %02X %02X %02X %02X\n",tblock,errc,memory[2],memory[3],memory[4],memory[5],memory[6]);
		}
*/
	}

	//erase
	if ((errc == 0) && ((dflash_dump == 1) || (main_dump == 1)))
	{
		blocks=2;
		addr=param[0];
		printf("ERASE %d BLOCKS OF MAIN FLASH FOR DUMPER CODE\n",blocks);
		errc=prg_comm(0x6a,0,7,0,0,(addr >> 8) & 0xff,(addr >> 16) & 0xff,blocks,0);	//program
//		printf("ADDR= %08lx\n",addr);
		if(errc != 0)
		{
			printf("RESP(%02X) = %02X %02X %02X %02X %02X\n",errc,memory[2],memory[3],memory[4],memory[5],memory[6]);
		}
	}


	//unsecure
	if ((errc == 0) && (unsec == 1))
	{
		printf("RELEASE SECURITY\n");
		errc=prg_comm(0x6d,0,10,0,0,0,0,0,0);	//unsecure
		if(errc != 0)
		{
			printf("RESP(%02X) = %02X %02X %02X %02X %02X\n",errc,memory[2],memory[3],memory[4],memory[5],memory[6]);
		}
	}

	//read signature
	if ((errc == 0) && (rsig == 1))
	{
		printf("READ SILICON SIGNATURE\n");
		errc=prg_comm(0x6e,0,40,0,0,0,0,0,0);	//get silicon signature
//		printf("RESP = %02X %02X %02X %02X %02X\n\n",memory[0],memory[1],memory[2],memory[3],memory[4]);
		printf("DEVICE CODE  = %02X %02X %02X\n",memory[7],memory[8],memory[9]);
		printf("DEVICE       = %c%c%c%c%c%c%c%c%c%c\n",memory[10],memory[11],memory[12],
		memory[13],memory[14],memory[15],memory[16],memory[17],memory[18],memory[19]);
		printf("CODE END     = 0x%02X%02X%02X\n",memory[22],memory[21],memory[20]);
		printf("DATA END     = 0x%02X%02X%02X\n",memory[25],memory[24],memory[23]);
		printf("VERSION      = V%X.%X%X\n",memory[26],memory[27],memory[28]);
		printf("READ MAIN FLASH CHECKSUM ...");
		errc=prg_comm(0x5d,0,8,0,0,
				(param[0] >> 8) & 0xff,(param[0] >> 16) & 0xff,
				((param[0]+param[1]) >> 8) & 0xff,((param[0]+param[1]) >> 16) & 0xff);
		if(errc==0)
		{
//			printf("RESP(%02X) = %02X %02X %02X %02X %02X\n",errc,memory[2],memory[3],memory[4],memory[5],memory[6]);
			printf("%02X%02X\n",memory[5],memory[4]);
		}
		else
		{
			printf("FAILED\n");
		}
		printf("READ DATA FLASH CHECKSUM ...");
		errc=prg_comm(0x5d,0,8,0,0,
				(param[2] >> 8) & 0xff,(param[2] >> 16) & 0xff,
				((param[2]+param[3]) >> 8) & 0xff,((param[2]+param[3]) >> 16) & 0xff);
		if(errc==0)
		{
//			printf("RESP(%02X) = %02X %02X %02X %02X %02X\n",errc,memory[2],memory[3],memory[4],memory[5],memory[6]);
			printf("%02X%02X\n",memory[5],memory[4]);
		}
		else
		{
			printf("FAILED\n");
		}


	}

	if ((errc == 0) && (gsec == 1))
	{
		printf("GET SECURITY\n");
		errc=prg_comm(0x5f,0,40,0,0,0,0,0,0);	//unsecure
		printf("SEC FLAG           = %02X\n",memory[7]);
		if((memory[7] & 0x10) == 0) printf("-> PROGRAMMING DISABLED\n");
		if((memory[7] & 0x04) == 0) printf("-> BLOCK ERASE DISABLED\n");
		if((memory[7] & 0x02) == 0) printf("-> BOOT BLOCK CLUSTER REWRITE DISABLED\n");
		if((memory[7] & 0x01) == 1) printf("-> BOOT AREA CHANGE PROVIDED\n");
		printf("BOOT BLOCK CLUSTER = %02X\n",memory[8]);
		printf("FS WINDOW START    = %04X\n",memory[9]+256*memory[10]);
		printf("FS WINDOW END      = %04X\n",memory[11]+256*memory[12]);
	}

	//erase
	if ((errc == 0) && (dflash_erase == 1))
	{
		blocks=param[3] >> 10;
		addr=param[2];
		printf("ERASE %d BLOCKS OF DATA FLASH\n",blocks);
		errc=prg_comm(0x6a,0,10,0,0,(addr >> 8) & 0xff,(addr >> 16) & 0xff,blocks,0);	//program
		if(errc != 0)
		{
			printf("RESP(%02X) = %02X %02X %02X %02X %02X\n",errc,memory[2],memory[3],memory[4],memory[5],memory[6]);
		}

	}

	//blankcheck main
	if ((errc == 0) && (main_bcheck == 1))
	{
		bsize = 2048;
		if(bsize > param[1]) bsize=param[1];
		addr=param[0];
		blocks=param[1]/bsize;

		progress("MAIN BCHECK ",blocks,0);
		for(tblock=0;tblock<blocks;tblock++)
		{
			progress("MAIN BCHECK ",blocks,tblock+1);
			if(errc == 0)
			{
				errc=prg_comm(0x6f,0,7,0,0,(addr >> 8) & 0xff,(addr >> 16) & 0xff,0,0);	//program
				if(errc!=0) 
				{
					if(memory[4]==0x1b) 
						errc=0x4A;
					else	
						printf("RESP = %02X %02X %02X %02X %02X\n",memory[2],memory[3],memory[4],memory[5],memory[6]);		
					goto RL78_END;
				}
				addr+=bsize;
			}
		}
		printf("\n");
	}

	//program main
	if ((errc == 0) && (main_prog == 1))
	{
		read_block(param[0],param[1],0);
		bsize = 2048;
		if(bsize > param[1]) bsize=param[1];
		addr=param[0];
		blocks=param[1]/bsize;
		maddr=0;

		progress("MAIN PROG   ",blocks,0);
		for(tblock=0;tblock<blocks;tblock++)
		{
			if((must_prog(maddr,bsize)) && (errc == 0))
			{
				//printf("ADDR = %08lX\n",addr);
				errc=prg_comm(0x6b,bsize,7,maddr,0,
					(addr >> 8) & 0xff,(addr >> 16) & 0xff,0,0);	//program
				if(errc!=0) 
				{
					printf("RESP = %02X %02X %02X %02X %02X\n",memory[2],memory[3],memory[4],memory[5],memory[6]);		
				}
			}
			maddr+=bsize;
			addr+=bsize;
			progress("MAIN PROG   ",blocks,tblock+1);
		}
		printf("\n");
	}

	//program dumper
	if ((errc == 0) && (dflash_dump == 1))
	{
		bsize = 2048;
		if(bsize > param[1]) bsize=param[1];
		addr=param[0];
		blocks=1;
		maddr=0;

		for(j=0;j<2048;j++)
		{
			memory[j] = rl78_dump[j];
		}

		progress("DUMP PROG   ",blocks,0);
		for(tblock=0;tblock<blocks;tblock++)
		{
			if((must_prog(maddr,bsize)) && (errc == 0))
			{
				//printf("ADDR = %08lX\n",addr);
				errc=prg_comm(0x6b,bsize,7,maddr,0,
					(addr >> 8) & 0xff,(addr >> 16) & 0xff,0,0);	//program
				if(errc!=0) 
				{
					printf("RESP = %02X %02X %02X %02X %02X\n",memory[2],memory[3],memory[4],memory[5],memory[6]);		
				}
			}
			maddr+=bsize;
			addr+=bsize;
			progress("DUMP PROG   ",blocks,tblock+1);
		}
		printf("\n");
	}

	//program dumper
	if ((errc == 0) && (main_dump == 1))
	{
		bsize = 2048;
		if(bsize > param[1]) bsize=param[1];
		addr=param[0];
		blocks=1;
		maddr=0;

		for(j=0;j<2048;j++)
		{
			memory[j] = rl78_fdump[j];
		}

		progress("DUMP PROG   ",blocks,0);
		for(tblock=0;tblock<blocks;tblock++)
		{
			if((must_prog(maddr,bsize)) && (errc == 0))
			{
				//printf("ADDR = %08lX\n",addr);
				errc=prg_comm(0x6b,bsize,7,maddr,0,
					(addr >> 8) & 0xff,(addr >> 16) & 0xff,0,0);	//program
				if(errc!=0) 
				{
					printf("RESP = %02X %02X %02X %02X %02X\n",memory[2],memory[3],memory[4],memory[5],memory[6]);		
				}
			}
			maddr+=bsize;
			addr+=bsize;
			progress("DUMP PROG   ",blocks,tblock+1);
		}
		printf("\n");
	}



	//verify main
	if ((errc == 0) && (main_verify == 1))
	{
		read_block(param[0],param[1],0);
		bsize = 2048;
		if(bsize > param[1]) bsize=param[1];
		addr=param[0];
		blocks=param[1]/bsize;
		maddr=0;

//		waitkey();

		progress("MAIN VERIFY ",blocks,0);
		for(tblock=0;tblock<blocks;tblock++)
		{
			progress("MAIN VERIFY ",blocks,tblock+1);
			if(errc == 0)
			{
				errc=prg_comm(0x6c,bsize,0,maddr,0,
					(addr >> 8) & 0xff,(addr >> 16) & 0xff,0,0);	//verify
				if(errc==0) addr+=bsize;
				maddr+=bsize;
			}
		}
		printf("\n");
	}

	//blankcheck dflash
	if ((errc == 0) && (dflash_bcheck == 1))
	{
		bsize = 2048;
		if(bsize > param[3]) bsize=param[3];
		addr=param[2];
		blocks=param[3]/bsize;

		progress("DATA BCHECK ",blocks,0);
		for(tblock=0;tblock<blocks;tblock++)
		{
			progress("DATA BCHECK ",blocks,tblock+1);
			if(errc == 0)
			{
				errc=prg_comm(0x6f,0,0,0,0,(addr >> 8) & 0xff,(addr >> 16) & 0xff,0,0);	//program
				addr+=bsize;
			}
		}
		printf("\n");
	}

	//program dflash
	if ((errc == 0) && (dflash_prog == 1))
	{
		read_block(param[2],param[3],0);
		bsize = 2048;
		if(bsize > param[3]) bsize=param[3];
		addr=param[2];
		blocks=param[3]/bsize;
		maddr=0;

		progress("DATA PROG   ",blocks,0);
		for(tblock=0;tblock<blocks;tblock++)
		{
			if((must_prog(maddr,bsize)) && (errc == 0))
			{
				//printf("ADDR = %08lX\n",addr);
				errc=prg_comm(0x6b,bsize,7,maddr,0,
					(addr >> 8) & 0xff,(addr >> 16) & 0xff,0,0);	//program
				if(errc!=0) 
				{
					printf("RESP = %02X %02X %02X %02X %02X\n",memory[2],memory[3],memory[4],memory[5],memory[6]);		
				}
			}
			maddr+=bsize;
			addr+=bsize;
			progress("DATA PROG   ",blocks,tblock+1);
		}
		printf("\n");
	}

	//verify dflash
	if ((errc == 0) && (dflash_verify == 1))
	{
		read_block(param[2],param[3],0);
		bsize = 2048;
		if(bsize > param[3]) bsize=param[3];
		addr=param[2];
		blocks=param[3]/bsize;
		maddr=0;

		progress("DATA VERIFY ",blocks,0);
		for(tblock=0;tblock<blocks;tblock++)
		{
			progress("DATA VERIFY ",blocks,tblock+1);
			if(errc == 0)
			{
				errc=prg_comm(0x6c,bsize,0,maddr,0,(addr >> 8) & 0xff,(addr >> 16) & 0xff,0,0);	//verify
				if(errc==0) addr+=bsize;
				maddr+=bsize;
			}
		}
		printf("\n");
	}


	//checksum main
	if ((errc == 0) && (main_csum == 1))
	{
		blocks=param[1]/1024;
		addr=param[0];
		maddr=addr+1024;
		mcsum=0x80000000;
		
		progress("MAIN CHECKSUM ",blocks,0);
		for(tblock=0;tblock<blocks;tblock++)
		{
			progress("MAIN CHHECKSUM ",blocks,tblock+1);
			if(errc == 0)
			{
				errc=prg_comm(0x5d,0,8,0,0,(addr >> 8) & 0xff,(addr >> 16) & 0xff,(maddr >> 8) & 0xff,(maddr >> 16) & 0xff);
				if(errc==0) 
				{
					checksum[tblock]=memory[4]+(memory[5] << 8);
				}
				addr+=1024;
				maddr+=1024;
			}
		}
		if(errc != 0) goto RL78_END;
		addr=param[0];
		for(tblock=0;tblock<blocks;tblock++)
		{
			if((tblock & 15) == 0)
			{
				printf("\n%06lX  ",addr);
			}
			printf("%04X ",checksum[tblock]);
			mcsum -= checksum[tblock];
			addr+=1024;
		}
		printf("\nMAIN FLASH CHECKSUM = 0x%04X\n\n",(unsigned short)mcsum & 0xFFFF);		
	}

	//checksum data
	if ((errc == 0) && (dflash_csum == 1))
	{
		blocks=param[3]/1024;
		addr=param[2];
		maddr=addr+1024;
		mcsum=0x80000000;

		progress("DATA CHECKSUM ",blocks,0);
		for(tblock=0;tblock<blocks;tblock++)
		{
			progress("DATA CHHECKSUM ",blocks,tblock+1);
			if(errc == 0)
			{
				errc=prg_comm(0x5d,0,8,0,0,(addr >> 8) & 0xff,(addr >> 16) & 0xff,(maddr >> 8) & 0xff,(maddr >> 16) & 0xff);
				if(errc==0) 
				{
					checksum[tblock]=memory[4]+(memory[5] << 8);
				}
				addr+=1024;
				maddr+=1024;
			}
		}
		if(errc != 0) goto RL78_END;
		addr=param[2];
		for(tblock=0;tblock<blocks;tblock++)
		{
			if((tblock & 15) == 0)
			{
				printf("\n%06lX  ",addr);
			}
			printf("%04X ",checksum[tblock]);
			mcsum -= checksum[tblock];
			addr+=1024;
		}
		printf("\nDATA FLASH CHECKSUM = 0x%04X\n\n",(unsigned short)mcsum & 0xFFFF);		
	}


	if(dev_start == 1)
	{
		if(errc == 0) errc=prg_comm(0x0e,0,0,0,0,0,0,0,0);			//init
		waitkey();
	}

RL78_END:

	prg_comm(0x65,0,0,0,0,8,0,0,0);	//exit

	if ((errc == 0) && (dflash_dump == 1))
	{
		bsize=2048;
		sleep(1);
		errc=writeblock_open();
		if(errc == 0) errc=prg_comm(0x0e,0,0,0,0,0,0,0,0);			//init
		maddr=0;
		blocks=param[3]/bsize;
		
		progress("READ DUMP   ",blocks,0);
		for(j=0;j<blocks;j++)
		{
			errc=prg_comm(0x193,0,bsize,0,ROFFSET+maddr,0,0,0,0);
			maddr+=bsize;
			progress("READ DUMP   ",blocks,j+1);
		}
		writeblock_data(0,param[3],param[2]);
		writeblock_close();
		prg_comm(0x65,0,0,0,0,8,0,0,0);	//exit
	}


	if ((errc == 0) && (main_dump == 1))
	{
		bsize=2048;
		sleep(1);
		errc=writeblock_open();
		if(errc == 0) errc=prg_comm(0x0e,0,0,0,0,0,0,0,0);			//init
		maddr=0;
		blocks=param[1]/bsize;
				
		progress("READ DUMP   ",blocks,0);
		for(j=0;j<blocks;j++)
		{
			errc=prg_comm(0x193,0,bsize,0,ROFFSET+maddr,0,0,0,0);
			maddr+=bsize;
			progress("READ DUMP   ",blocks,j+1);
		}
		writeblock_data(0,param[1],param[0]);
		writeblock_close();
		prg_comm(0x65,0,0,0,0,8,0,0,0);	//exit
	}


	prg_comm(0x2ef,0,0,0,0,0,0,0,0);	//dev 1
	print_rl78_error(errc,addr);

	return errc;
}








