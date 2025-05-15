//###############################################################################
//#										#
//# UPROG universal programmer							#
//#										#
//# copyright (c) 2012-2022 Joerg Wolfram (joerg@jcwolfram.de)			#
//#										#
//#										#
//# This program is free software; you can redistribute it and/or		#
//# modify it under the terms of the GNU General Public License			#
//# as published by the Free Software Foundation; either version 2		#
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

void print_at24rf08_error(int errc)
{
	printf("\n");
	switch(errc)
	{
		case 0:		set_error("OK",errc);
				break;

		case 0x40:	set_error("(No ACK at adressing)",errc);
				break;

		case 0x41:	set_error("(at24rf08 error)",errc);
				break;

		default:	set_error("(unexpected error)",errc);
	}
	print_error();
}


int prog_at24rf08(void)
{
	int errc,blocks,i;
	unsigned char abyte;
	unsigned long addr,maddr;
	int bsize;
	int main_erase=0;
	int main_prog=0;
	int main_verify=0;
	int main_readout=0;

	int acc_readout=0;
	int acc_display=0;
	int acc_write=-1;
	int id_readout=0;
	int id_display=0;
	int id_write=-1;

	int speedmode=0;
//	int iaddr=0;
//	unsigned int csum;

	errc=0;


	if((strstr(cmd,"help")) && ((strstr(cmd,"help") - cmd) == 1))
	{
		printf("-- 5V -- set VDD to 5V\n");
		printf("-- ee -- eeprom erase\n");
		printf("-- pe -- eeprom program\n");
		printf("-- ve -- eeprom verify\n");
		printf("-- re -- eeprom read\n");

		printf("-- ra -- access page read\n");
		printf("-- da -- access page display\n");
		printf("-- ri -- id page read\n");
		printf("-- di -- id page display\n");

		printf("-- wa0x -- access byte x write\n");
		printf("-- wi0x -- id byte x write\n");

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

	if(find_cmd("ee"))
	{
		main_erase=1;
		printf("## Action: eeprom erase\n");
	}


	main_prog=check_cmd_prog("pe","eeprom");
	main_verify=check_cmd_verify("ve","eeprom");
	main_readout=check_cmd_read("re","eeprom",&main_prog,&main_verify);

	if((main_erase | main_prog | main_verify) == 0)
	{
		if(find_cmd("ra"))
		{
			acc_readout=1;
			printf("## Action: access page read\n");
		}

		if(find_cmd("da"))
		{
			acc_display=1;
			printf("## Action: access page display\n");
		}

		if(find_cmd("ri"))
		{
			id_readout=1;
			printf("## Action: id page read\n");
		}

		if(find_cmd("di"))
		{
			id_display=1;
			printf("## Action: id page display\n");
		}

	}

	if((main_erase | main_prog | main_readout | main_verify | acc_readout | id_readout) == 0)
	{
		memory[0]=expar & 0xFF;
	
		if((find_cmd("wa00")) && (acc_write < 0) && (id_write < 0))
		{
			acc_write = 0;
			printf("## Actionn: write 0x%02X to acc page byte %d\n",memory[0],acc_write);
		}

		if((find_cmd("wa01")) && (acc_write < 0) && (id_write < 0))
		{
			acc_write = 1;
			printf("## Actionn: write 0x%02X to acc page byte %d\n",memory[0],acc_write);
		}

		if((find_cmd("wa02")) && (acc_write < 0) && (id_write < 0))
		{
			acc_write = 2;
			printf("## Actionn: write 0x%02X to acc page byte %d\n",memory[0],acc_write);
		}

		if((find_cmd("wa03")) && (acc_write < 0) && (id_write < 0))
		{
			acc_write = 3;
			printf("## Actionn: write 0x%02X to acc page byte %d\n",memory[0],acc_write);
		}

		if((find_cmd("wa04")) && (acc_write < 0) && (id_write < 0))
		{
			acc_write = 4;
			printf("## Actionn: write 0x%02X to acc page byte %d\n",memory[0],acc_write);
		}

		if((find_cmd("wa05")) && (acc_write < 0) && (id_write < 0))
		{
			acc_write = 5;
			printf("## Actionn: write 0x%02X to acc page byte %d\n",memory[0],acc_write);
		}

		if((find_cmd("wa06")) && (acc_write < 0) && (id_write < 0))
		{
			acc_write = 6;
			printf("## Actionn: write 0x%02X to acc page byte %d\n",memory[0],acc_write);
		}

		if((find_cmd("wa07")) && (acc_write < 0) && (id_write < 0))
		{
			acc_write = 7;
			printf("## Actionn: write 0x%02X to acc page byte %d\n",memory[0],acc_write);
		}

		if((find_cmd("wa08")) && (acc_write < 0) && (id_write < 0))
		{
			acc_write = 8;
			printf("## Actionn: write 0x%02X to acc page byte %d\n",memory[0],acc_write);
		}

		if((find_cmd("wa09")) && (acc_write < 0) && (id_write < 0))
		{
			acc_write = 9;
			printf("## Actionn: write 0x%02X to acc page byte %d\n",memory[0],acc_write);
		}

		if((find_cmd("wa10")) && (acc_write < 0) && (id_write < 0))
		{
			acc_write = 10;
			printf("## Actionn: write 0x%02X to acc page byte %d\n",memory[0],acc_write);
		}


		if((find_cmd("wi00")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 0;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi01")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 1;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi02")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 2;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi03")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 3;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi04")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 4;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi05")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 5;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi06")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 6;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi07")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 7;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi08")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 8;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi09")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 9;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi10")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 10;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi11")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 11;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi12")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 12;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi13")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 13;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi14")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 14;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

		if((find_cmd("wi15")) && (id_write < 0) && (id_write < 0))
		{
			id_write = 15;
			printf("## Actionn: write 0x%02X to id page byte %d\n",memory[0],id_write);
		}

	}

	printf("\n");
	
	if((main_readout > 0) || (acc_readout > 0) || (id_readout > 0))
	{
		errc=writeblock_open();
	}

	if(errc==0) 
	{
		errc=prg_comm(0xfe,0,0,0,0,3,3,0,0);					//enable pull-up
		errc=prg_comm(0xa0,0,0,0,0,speedmode,16,1,50);				//init
	}
	
	if((main_erase == 1) && (errc == 0))
	{
		printf("ERASE EEPROM\n");
		for(maddr=0;maddr<1024;maddr++) memory[maddr]=255;

		bsize=128;
		blocks=8;
		addr=0;
		maddr=0;

		progress("ERASE ",blocks,0);
		for(i=0;i<blocks;i++)
		{
			abyte=0xA8+(i & 0x06);
			if(errc==0) errc=prg_comm(0xa3,bsize,0,maddr,0,
			addr,				//LOW addr
			0,				//HIGH addr
			abyte,				//address byte
			8);				//pages per block
			addr+=bsize;
			maddr+=bsize;
			progress("ERASE ",blocks,i+1);
		}
		printf("\n");
	}

	if((main_prog == 1) && (errc == 0))
	{
		read_block(param[0],param[1],0);

		bsize=128;
		blocks=8;
		addr=0;
		maddr=0;

		progress("PROG ",blocks,0);
		for(i=0;i<blocks;i++)
		{
			abyte=0xA8+(i & 0x06);
			if(errc==0) errc=prg_comm(0xa3,bsize,0,maddr,0,
			addr,				//LOW addr
			0,				//HIGH addr
			abyte,				//address byte
			8);				//pages per block
			addr+=bsize;
			maddr+=bsize;
			progress("PROG ",blocks,i+1);
		}
		printf("\n");
	}


	if(((main_readout == 1) || (main_verify == 1)) && (errc == 0))
	{
		bsize=128;
		blocks=8;
		addr=0;
		maddr=0;

		progress("READ ",blocks,0);
		for(i=0;i<blocks;i++)
		{
			abyte=0xA8+(i & 0x06);
			if(errc==0) errc=prg_comm(0xa2,0,bsize,0,maddr+ROFFSET,
			addr & 0xff,			//LOW addr
			0,				//HIGH addr
			abyte,				//address byte
			bsize / param[2]);		//pages per block
			addr+=bsize;
			maddr+=bsize;
			progress("READ ",blocks,i+1);
		}
		printf("\n");
	}

	if((main_readout == 1) && (errc == 0))
	{
		writeblock_data(0,param[1],param[0]);
	}

	errc=prg_comm(0xa0,0,0,0,0,speedmode,1,1,20);	//re-init with 1 byte page

	if((acc_write >= 0) && (errc == 0))
	{
		blocks=1;
		maddr=0;

		progress("AWRITE ",blocks,0);
		for(i=0;i<blocks;i++)
		{
			abyte=i;
			if(errc==0) errc=prg_comm(0xa3,1,0,0,0,
			acc_write,			//LOW addr
			0,				//HIGH addr
			0xB8,				//address byte
			1);				//pages per block
			maddr+=1;
			progress("AWRITE ",blocks,i+1);
		}
		printf("\n");
	}

	if((id_write >= 0) && (errc == 0))
	{
		blocks=1;
		maddr=0;

		progress("IWRITE ",blocks,0);
		for(i=0;i<blocks;i++)
		{
			abyte=i;
			if(errc==0) errc=prg_comm(0xa3,1,0,0,0,
			id_write+16,			//LOW addr
			0,				//HIGH addr
			0xA8,				//address byte
			1);				//pages per block
			maddr+=1;
			progress("IWRITE ",blocks,i+1);
		}
		printf("\n");
	}


	if(((acc_readout == 1) || (acc_display == 1)) && (errc == 0))
	{
		blocks=16;
		maddr=0;

		progress("AREAD ",blocks,0);
		for(i=0;i<blocks;i++)
		{
			abyte=i;
			if(errc==0) errc=prg_comm(0xa2,0,1,0,maddr+ROFFSET,
			i,				//LOW addr
			0,				//HIGH addr
			0xB8,				//address byte
			1);				//pages per block
			maddr+=1;
			progress("AREAD ",blocks,i+1);
		}
		printf("\n");
	}

	if((acc_readout == 1) && (errc == 0))
	{
		writeblock_data(0,16,0x400);
	}

	if((acc_display == 1) && (errc == 0))
	{
//		printf("\n");       
		printf("-----------------------------------------------------------------------------------\n");       
		printf("SB  sticky bit\n");       
		printf("TW  if 0 then write protect from RFID if Tamper=1\n");       
		printf("RF  RFID protection (0,1 protect / 2 read only  / 3 no protection)\n");       
		printf("PB  Block protection (0,1 protect / 2 read only  / 3 no protection)\n");       
		printf("-----------------------------------------------------------------------------------\n");       
		for(i=0;i<8;i++)
		{
			abyte=memory[i+ROFFSET];
			printf("BLOCK %d (%02X):  SB=%d    TW=%d    RF=%d    PB=%d\n",i,abyte,(abyte >> 7) & 1,(abyte >> 6) & 1,(abyte >> 4) & 3,abyte & 3);       
		}
		i=8;
		abyte=memory[i+ROFFSET];
		printf("AP      (%02X):  SB=%d                    PB=%d\n",abyte,(abyte >> 7) & 1,abyte & 3);       
		i=9;
		abyte=memory[i+ROFFSET];
		printf("WP      (%02X):  Write protection within block 0\n",abyte);       		
		i=10;
		abyte=memory[i+ROFFSET];
		printf("TP      (%02X):  Tamper Bit = %d\n",abyte, abyte & 1);       		
		i=15;
		abyte=memory[i+ROFFSET];
		printf("REV     (%02X):  Device revision\n\n",abyte);       		
	}


	if(((id_readout == 1) || (id_display == 1)) && (errc == 0))
	{
		blocks=16;
		maddr=0;

		progress("IREAD ",blocks,0);
		for(i=0;i<blocks;i++)
		{
			abyte=i;
			if(errc==0) errc=prg_comm(0xa2,0,1,0,maddr+ROFFSET,
			i+16,				//LOW addr
			0,				//HIGH addr
			0xB8,				//address byte
			1);				//pages per block
			maddr+=1;
			progress("IREAD ",blocks,i+1);
		}
		printf("\n");
	}

	if((id_readout == 1) && (errc == 0))
	{
		writeblock_data(0,16,0x410);
	}


	if((id_display == 1) && (errc == 0))
	{
//		printf("\n");       
		printf("-----------------------------------------------------------------------------------------\n");       
		printf("ID bytes   B0   B1   B2   B3   B4   B5   B6   B7   B8   B9   B10  B11  B12  B13  B14  B15\n");       
		printf("-----------------------------------------------------------------------------------------\n");       
		printf("Data       ");       
		for(i=0;i<16;i++)
		{
			abyte=memory[i+ROFFSET];
			printf("%02X   ",abyte);
		}       
		printf("\n-----------------------------------------------------------------------------------------\n\n");       
	}



	if((main_readout > 0) || (acc_readout > 0) || (id_readout > 0))
	{
		writeblock_close();
	}


	i=prg_comm(0xa1,0,0,0,0,0,0,0,0);					//at24rf08 exit
	prg_comm(0x2ef,0,0,0,0,0,0,0,0);	//dev 1

	print_at24rf08_error(errc);

	return errc;
}

