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

void print_pic18b_error(int errc)
{
	printf("\n");
	switch(errc)
	{
		case 0:		set_error("OK",errc);
				break;

		case 0x7e:	set_error("(WRONG DEVICE ID)",errc);
				break;

		case 0x52:	set_error("(LOCK ERROR)",errc);
				break;

		case 0x53:	set_error("(SYNC ERROR)",errc);
				break;

		case 0x55:	set_error("(FETCH ERROR)",errc);
				break;

		default:	set_error("(unexpected error)",errc);
	}
	print_error();
}


int prog_pic18b()
{
	int errc,blocks,bsize,j,dev_id;
	unsigned int addr,len,fdata,rdata,maddr;
	int main_erase=0;
	int main_prog=0;
	int main_verify=0;
	int main_readout=0;
	int eeprom_erase=0;
	int eeprom_prog=0;
	int eeprom_verify=0;
	int eeprom_readout=0;
	int conf_prog=0;
	int conf_verify=0;
	int conf_readout=0;
	int uid_prog=0;
	int uid_verify=0;
	int uid_readout=0;
	int dev_start=0;
	int all_erase=0;
	int ignore_devid=0;

	errc=0;

	if((strstr(cmd,"help")) && ((strstr(cmd,"help") - cmd) == 1))
	{
		printf("-- 5V -- set VDD to 5V\n");
		printf("-- ea -- chip erase\n");

		printf("-- em -- main flash erase\n");
		printf("-- pm -- main flash program\n");
		printf("-- vm -- main flash verify\n");
		printf("-- rm -- main flash readout\n");

		printf("-- ee -- eeprom erase\n");
		printf("-- pe -- eeprom program\n");
		printf("-- ve -- eeprom verify\n");
		printf("-- re -- eeprom readout\n");

		printf("-- pc -- configuration program\n");
		printf("-- vc -- configuration verify\n");
		printf("-- rc -- configuration readout\n");

		printf("-- pu -- user id program\n");
		printf("-- vu -- user id verify\n");
		printf("-- ru -- user id readout\n");

		printf("-- ii -- ignore wrong ID\n");
		printf("-- st -- start device\n");
		return 0;
	}


	if(find_cmd("5v"))
	{
		errc=prg_comm(0xfb,0,0,0,0,0,0,0,0);	//5V mode
		printf("## using 5V VDD\n");
	}

	if(find_cmd("ii"))
	{
		ignore_devid=1;
		printf("## Ignore device ID\n");
	}

	if(find_cmd("em"))
	{
		main_erase=1;
		printf("## Action: main flash erase\n");
	}

	if(find_cmd("ee"))
	{
		eeprom_erase=1;
		printf("## Action: eeprom erase\n");
	}

	if(find_cmd("ea"))
	{
		all_erase=1;
		printf("## Action: chip (bulk) erase\n");
	}

	main_prog=check_cmd_prog("pm","code flash");
	eeprom_prog=check_cmd_prog("pe","eeprom");
	conf_prog=check_cmd_prog("pc","config");
	uid_prog=check_cmd_prog("pu","user id");

	main_verify=check_cmd_verify("vm","code flash");
	eeprom_verify=check_cmd_verify("ve","eeprom");
	conf_verify=check_cmd_verify("vc","config");
	uid_verify=check_cmd_verify("vu","user id");

	main_readout=check_cmd_read("rm","code flash",&main_prog,&main_verify);
	eeprom_readout=check_cmd_read("re","eeprom",&eeprom_prog,&eeprom_verify);
	conf_readout=check_cmd_read("rc","config",&conf_prog,&conf_verify);
	uid_readout=check_cmd_read("ru","user id",&uid_prog,&uid_verify);


	if(find_cmd("st"))
	{
		dev_start=1;
		printf("## Action: start device\n");
	}
	printf("\n");

	if((main_readout || eeprom_readout || conf_readout || uid_readout) > 0)
	{
		errc=writeblock_open();
	}

	//init
	if(dev_start == 0)
	{
		if(errc == 0) errc=prg_comm(0xf5,0,0,0,0,0,0,0,0);			//vpp off
		if(errc == 0) errc=prg_comm(0xf2,0,0,0,0,param[8],0,0,0);		//set vpp
		read_volt();
		if(errc == 0) errc=prg_comm(0x260,0,0,0,0,0,0,0,0);			//init
		if(errc == 0) errc=prg_comm(0x263,0,2,0,0,0xFE,0xFF,0x3F,1);		//get ID

		dev_id=(memory[0]+256*memory[1]) & 0xFFFF;
		if(dev_id != param[10])
		{
			printf("ID READ = %04X, SHOULD BE %04lX\n",dev_id,param[10]);
			errc=0x7e;
		}
		else
		{
			printf("ID = %04X\n",dev_id);
		}

		if((ignore_devid == 1) && (errc == 0x7e)) errc=0;
		if(errc != 0) goto PROG_END; 

	}

	//erase
	if((all_erase == 1) && (errc == 0))
	{
		printf("BULK ERASE\n");
		errc=prg_comm(0x261,0,0,0,0,0,0,0,0x0F);			//erase all
	}

	if((main_erase == 1) && (errc == 0))
	{
		printf("FLASH ERASE\n");
		errc=prg_comm(0x261,0,0,0,0,0,0,0,0x02);			//erase flash
	}

	if((eeprom_erase == 1) && (errc == 0))
	{
		printf("EEPROM ERASE\n");
		errc=prg_comm(0x261,0,0,0,0,0,0,0,0x01);			//erase data
	}


	//program main flash
	if((main_prog == 1) && (errc == 0))
	{
		read_block(param[0],param[1],0);
		addr = param[0];
		len=param[1];
		bsize = max_blocksize;
		blocks = len / bsize;
		maddr=0;

		progress("PROG MAIN ",blocks,0);
		for(j=0;j<blocks;j++)
		{
//			printf("bsize= %04X  addr= %04X\n",bsize,maddr);
			if(must_prog(maddr,bsize) && (errc == 0))
			{
				errc=prg_comm(0x265,bsize,0,maddr,0,
					addr & 0xFF,(addr >> 8) & 0xFF,(addr >> 16) & 0xFF,bsize/512);	//program block
			}
			addr+=bsize;
			maddr+=bsize;
			progress("PROG MAIN ",blocks,j+1);
		}
		printf("\n");
	}

	//verify/readout main flash
	if(((main_readout == 1) || (main_verify == 1)) && (errc == 0))
	{
		addr = param[0];
		len=param[1];
		bsize = max_blocksize;
		blocks = len / bsize;
		maddr=0;

		progress("READ MAIN ",blocks,0);
		for(j=0;j<blocks;j++)
		{
			if(errc == 0) 
			{
				errc=prg_comm(0x262,0,bsize,0,maddr+ROFFSET,
					addr & 0xFF,(addr >> 8) & 0xFF,(addr >> 16) & 0xFF,bsize/512);	//program block
			}
			progress("READ MAIN ",blocks,j+1);
			addr+=bsize;
			maddr+=bsize;
		}
		printf("\n");
	}

	if(main_verify == 1)
	{
		read_block(param[0],param[1],0);
		addr = param[0];
		len = param[1];

		for(j=0;j<len;j+=2)
		{
			rdata=(memory[j+ROFFSET]+256*memory[j+ROFFSET+1]) & 0x3FFF;
			fdata=(memory[j]+256*memory[j+1]) & 0x3FFF;
			
			if(rdata != fdata)
			{
				printf("ERR -> ADDR= %04X  DATA= %04X  READ= %04X\n",addr+j,fdata,rdata);
				errc=1;
			}
		}
	}

	if(main_readout == 1)
	{
		writeblock_data(0,param[1],param[0]);
	}

	//program data flash
	if((eeprom_prog == 1) && (errc == 0))
	{
		read_block(param[2],param[3],0);
		addr = param[2];
		len = param[3];
		bsize = 128;
		blocks = len / bsize;
		maddr=0;
	
//		printf("bsize= %04X  addr= %04X\n",bsize,addr);

		progress("PROG DATA ",blocks,0);
		for(j=0;j<blocks;j++)
		{
//			printf("bsize= %04X  addr= %04X\n",bsize,maddr);
			if(must_prog(maddr,bsize) && (errc == 0)) 
			{
				errc=prg_comm(0x267,bsize,0,maddr,0,
					addr & 0xFF,(addr >> 8) & 0xFF,(addr >> 16) & 0xFF,bsize);	//program block
			}
			addr+=bsize;
			maddr+=bsize;
			progress("PROG DATA ",blocks,j+1);
		}

		printf("\n");
	}

	//verify/readout data flash
	if(((eeprom_readout == 1) || (eeprom_verify == 1)) && (errc == 0))
	{
		addr = param[2];
		len = param[3];
		bsize = 128;
		blocks = len / bsize;
		maddr=0;

		progress("READ DATA ",blocks,0);
		for(j=0;j<blocks;j++)
		{
			if(errc == 0) 
			{
				errc=prg_comm(0x264,0,bsize,0,maddr+ROFFSET,
					addr & 0xFF,(addr >> 8) & 0xFF,(addr >> 16) & 0xFF,bsize);	//program block
			}
			progress("READ DATA ",blocks,j+1);
			addr+=bsize;
			maddr+=bsize;
		}
		printf("\n");
	}

	if(eeprom_verify == 1)
	{
		read_block(param[2],param[3],0);
		addr = param[2];
		len = param[3];
		for(j=0;j<len;j+=1)
		{
			rdata=memory[addr+j+ROFFSET];
			fdata=memory[addr+j];
			
			if(rdata != fdata)
			{
				printf("ERR -> ADDR= %04X  DATA= %02X  READ= %02X\n",addr+j,fdata,rdata);
				errc=1;
			}
		}
	}

	if(eeprom_readout == 1)
	{
		writeblock_data(0,param[3],param[2]);
	}


	//program user ID
	if((uid_prog == 1) && (errc == 0))
	{
		read_block(param[6],param[7],0);
		addr = param[6];
		bsize = param[7];
		blocks=bsize / 2;
		maddr=0;

		printf("UID=");	for(j=0;j<blocks;j++) printf(" %02X%02X",memory[2*j+1],memory[2*j]); printf("\n");


		progress("PROG UID ",blocks,0);
		for(j=0;j<blocks;j++)
		{
	//		printf("bsize= %04X  addr= %08lX\n",bsize,addr);
			if(must_prog(maddr,bsize) && (errc == 0)) 
			{
				errc=prg_comm(0x266,2,0,maddr,0,
					addr & 0xFF,(addr >> 8) & 0xFF,(addr >> 16) & 0xFF,1);	//program WORD
			}
			addr+=2;
			maddr+=2;
			progress("PROG UID ",blocks,j+1);
		}

		printf("\n");

	}

	//verify/readout user ID
	if(((uid_readout == 1) || (uid_verify == 1)) && (errc == 0))
	{
		addr = param[6];
		bsize = param[7];
		maddr=0;

		progress("READ UID  ",1,0);
		errc=prg_comm(0x263,0,bsize,0,maddr+ROFFSET,
			addr & 0xFF,(addr >> 8) & 0xFF,(addr >> 16) & 0xFF,bsize/2);	//read WORDs

		progress("READ UID  ",1,1);
		printf("\n");
	}

	if(uid_verify == 1)
	{
		read_block(param[6],param[7],0);
		addr = param[6];
		len = param[7];
		for(j=0;j<len;j+=2)
		{
			rdata=(memory[j+ROFFSET]+256*memory[j+ROFFSET+1]) & 0x3FFF;
			fdata=(memory[j]+256*memory[j+1]) & 0x3FFF;
			
			if(rdata != fdata)
			{
				printf("ERR -> ADDR= %04X  DATA= %04X  READ= %04X\n",addr+j,fdata,rdata);
				errc=1;
			}
		}
	}

	if(uid_readout == 1)
	{
		writeblock_data(0,param[7],param[6]);
	}

	//program config words
	if((conf_prog == 1) && (errc == 0))
	{
		read_block(param[4],param[5],0);
		addr = param[4];
		bsize = param[5];
		len=bsize;
		blocks=len;
		maddr=0;

		printf("CONF=");
		for(j=0;j<len;j++) printf(" %02X",memory[j]);
		printf("\n");
		
		progress("PROG CONF ",blocks,0);
		for(j=0;j<blocks;j++)
		{
//			printf("bsize= %04X  addr= %04X\n",bsize,maddr);
			if(must_prog(maddr,bsize) && (errc == 0)) 
			{
				errc=prg_comm(0x267,2,0,maddr,0,
					addr & 0xFF,(addr >> 8) & 0xFF,(addr >> 16) & 0xFF,1);	//program WORD
			}
			addr+=1;
			maddr+=1;
			progress("PROG CONF ",blocks,j+1);
		}

		printf("\n");

	}

	//verify/readout config words
	if(((conf_readout == 1) || (conf_verify == 1)) && (errc == 0))
	{
		addr = param[4];
		len = param[5];
		bsize=1;
		blocks=len;
		maddr=0;

		progress("READ CONF ",blocks,0);
		for(j=0;j<blocks;j++)
		{
			if(errc == 0) 
			{
				errc=prg_comm(0x264,0,bsize,0,maddr+ROFFSET,
					addr & 0xFF,(addr >> 8) & 0xFF,(addr >> 16) & 0xFF,bsize);	//read bytes
			}
			progress("READ CONF ",blocks,j+1);
			addr+=bsize;
			maddr+=bsize;
		}
		printf("\n");

		printf("CONF=");
		for(j=0;j<len;j++) printf(" %02X",memory[ROFFSET+j]);
		printf("\n");
	}

	if(conf_verify == 1)
	{
		read_block(param[4],param[5],0);
		addr = param[4];
		len = param[5];
		
		for(j=0;j<len;j+=2)
		{
			rdata=(memory[j+ROFFSET]+256*memory[j+ROFFSET+1]) & param[13+j/2];
			fdata=(memory[j]+256*memory[j+1]) & param[13+j/2];
//			rdata|=param[13+j/2];
//			fdata|=param[13+j/2];

			if(rdata != fdata)
			{
				printf("ERR -> ADDR= %04X  DATA= %04X  READ= %04X\n",addr+j,fdata,rdata);
				errc=1;
			}
		}
	}
	if(conf_readout == 1)
	{
		writeblock_data(0,param[5],param[4]);
	}

	if((main_readout || eeprom_readout || conf_readout || uid_readout) > 0)
	{
		errc=writeblock_close();
	}


	if(dev_start == 1)
	{
		if(errc == 0) errc=prg_comm(0x70,0,0,0,0,4,0,0,0);			//init
		waitkey();
		prg_comm(0x71,0,0,0,0,0,0,0,0);						//exit
	}

PROG_END:

	prg_comm(0x71,0,0,0,0,0,0,0,0);					//exit
	prg_comm(0xf5,0,0,0,0,0,0,0,0);					//vpp off
	prg_comm(0xf2,0,0,0,0,0,0,0,0);					//disable vpp

	print_pic18b_error(errc);
	return errc;

}



