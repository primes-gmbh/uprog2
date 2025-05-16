//###############################################################################
//#										#
//# UPROG2 universal programmer							#
//#										#
//# copyright (c) 2012-2020 Joerg Wolfram (joerg@jcwolfram.de)			#
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

void print_xspi_error(int errc)
{
	printf("\n");
	switch(errc)
	{
		case 0:		set_error("OK",0);
				break;

		case 1:		set_error("WRONG COMMAND",1);
				break;

		case 0x40:	set_error("NO DATA",1);
				break;

		default:	set_error("(unexpected error)",errc);
	}
	print_error();
}

int xspi(void)
{
	int errc=0;
	int spimode=0;
	int divider=0;
	char xdata[1000];

	if((strstr(cmd,"help")) && ((strstr(cmd,"help") - cmd) == 1))
	{
		printf("-- x0 -- exchange 4/4 bytes data with CS at PORTB0\n");
		printf("-- x1 -- exchange 4/4 bytes data with CS at PORTB1\n");

		printf("-- b1 -- set bitrate to 5 MHz (10 MHz default)\n");
		printf("-- b2 -- set bitrate to 2.5 MHz\n");
		printf("-- b3 -- set bitrate to 1.25 MHz\n");
		printf("-- b4 -- set bitrate to 625 kHz\n");
		printf("-- b1 -- set bitrate to 312.5 kHz\n");
		printf("-- b6 -- set bitrate to 156.25 kHz\n");
		printf("-- b7 -- set bitrate to 78.125 kHz\n");

		printf("-- m1 -- set SPI-Mode to 1 (default 0)\n");
		printf("-- m2 -- set SPI-Mode to 2\n");
		printf("-- m3 -- set SPI-Mode to 3\n");
		
		return 0;
	}

	if(find_cmd("m1"))
	{
		printf("## set SPI-Mode to 1\n");
		spimode=1;	
	}

	if(find_cmd("m2"))
	{
		printf("## set SPI-Mode to 2\n");
		spimode=2;	
	}

	if(find_cmd("m3"))
	{
		printf("## set SPI-Mode to 3\n");
		spimode=3;	
	}

	if(find_cmd("b1"))
	{
		printf("## set bitrate to 5 MHz\n");
		divider=1;	
	}

	if(find_cmd("b2"))
	{
		printf("## set bitrate to 2.5 MHz\n");
		divider=2;	
	}

	if(find_cmd("b3"))
	{
		printf("## set bitrate to 1.25 MHz\n");
		divider=3;	
	}

	if(find_cmd("b4"))
	{
		printf("## set bitrate to 625 kHz\n");
		divider=4;	
	}

	if(find_cmd("b5"))
	{
		printf("## set bitrate to 312.5 kHz\n");
		divider=5;	
	}

	if(find_cmd("b6"))
	{
		printf("## set bitrate to 156.25 kHz\n");
		divider=6;	
	}

	if(find_cmd("b7"))
	{
		printf("## set bitrate to 78.125 kHz\n");
		divider=7;	
	}

	if(find_cmd("x0"))
	{
		if(have_expar == 0) errc=0x40;
		else
		{
			printf("## Action: exchange 4 Bytes data with CS at PORTB0\n");
			memory[0]=(expar >> 24) & 0xFF;
			memory[1]=(expar >> 24) & 0xFF;
			memory[2]=(expar >> 24) & 0xFF;
			memory[3]=(expar >> 24) & 0xFF;
			memory[4]=0;
			memory[5]=0;
			memory[6]=0x55;
			memory[7]=0;
			errc=prg_comm(0x2e0,8,8,0,ROFFSET,divider,spimode,8,0);			//init
			snprintf(xdata,100,"RES = %02X %02X %02X %02X",memory[ROFFSET+4],memory[ROFFSET+5],memory[ROFFSET+6],memory[ROFFSET+7]);
			printf("%s\n",xdata);
			addlog2(xdata);	
		}
	}

	if(find_cmd("x1"))
	{
		if(have_expar == 0) errc=0x40;
		else
		{
			printf("## Action: exchange 4 Bytes data with CS at PORTB1\n");
			memory[0]=(expar >> 24) & 0xFF;
			memory[1]=(expar >> 24) & 0xFF;
			memory[2]=(expar >> 24) & 0xFF;
			memory[3]=(expar >> 24) & 0xFF;
			memory[4]=0;
			memory[5]=0;
			memory[6]=0x55;
			memory[7]=0;
			errc=prg_comm(0x2e1,8,8,0,ROFFSET,divider,spimode,8,0);			//init
			snprintf(xdata,100,"RES = %02X %02X %02X %02X",memory[ROFFSET+4],memory[ROFFSET+5],memory[ROFFSET+6],memory[ROFFSET+7]);
			printf("%s\n",xdata);
			addlog2(xdata);
		}
	}

	print_xspi_error(errc);

	return errc;
}
