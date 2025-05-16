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

int hex2hex(void)
{
	int errc,j;
	int bpl=0;

	printf("## HEX TO HEX converter\n");
	printf("## IN:  %s\n",sfile);
	printf("## OUT: %s\n",tfile);

	if((strstr(cmd,"help")) && ((strstr(cmd,"help") - cmd) == 1))
	{
		printf("-- ll04 -- 4 bytes per line\n");
		printf("-- ll08 -- 8 bytes per line\n");
		printf("-- ll16 -- 16 bytes per line\n");
		return 0;
	}

	bpl=32;

	if(find_cmd("ll04"))
	{
		printf("## 4 bytes per line\n");
		bpl=4;
	}

	if(find_cmd("ll08"))
	{
		printf("## 8 bytes per line\n");
		bpl=8;
	}

	if(find_cmd("ll16"))
	{
		printf("## 16 bytes per line\n");
		bpl=16;
	}

	if(find_cmd("ll32"))
	{
		printf("## 16 bytes per line\n");
		bpl=32;
	}

	if(file_found < 2)
	{
		printf("## !! ABORTED BECAUSE OF NO INPUT FILE !!\n");
		goto S192HEX_ERR;
	}

	if(tfile_found < 1)
	{
		printf("## !! ABORTED BECAUSE OF NO OUTPUT FILE !!\n");
		goto S192HEX_ERR;
	}

	read_block(0,ROFFSET,0);		//get data
	for(j=0;j<ROFFSET;j++)
	{
		memory[ROFFSET+j]=memory[j];
	}

	printf(">LO ADDR = %08X\n",loaddr & 0xFFFFFFFF);
	printf(">HI ADDR = %08X\n",hiaddr & 0xFFFFFFFF);
		
	write_hexblock(loaddr,hiaddr-loaddr+1,loaddr,bpl);	

S192HEX_ERR:

	return errc;
}



