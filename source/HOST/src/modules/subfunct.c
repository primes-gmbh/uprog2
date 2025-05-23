//###############################################################################
//#										#
//#										#
//#										#
//# copyright (c) 2010-2020 Joerg Wolfram (joerg@jcwolfram.de)			#
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
#include <poll.h>
#include <termios.h>

extern int usb_io(int,int);

//----------------------------------------------------------------------------------
// send a command to programmer
//----------------------------------------------------------------------------------
int prg_comm(int cmd,int txlen,int rxlen,long txaddr,long rxaddr,int p1,int p2,int p3,int p4)
{
	int offset;
	int lt,ht,lr,hr,i;
	lt = txlen & 0xff;
	ht = (txlen >> 8) & 0xff;
	lr = rxlen & 0xff;
	hr = (rxlen >> 8) & 0xff;


	if(interface_type < 9)
	{
		shm[4]=(0xab + (cmd >> 8)) & 0xff;
		shm[5]=cmd & 0xff;
		shm[6]=lt;
		shm[7]=ht;
		shm[8]=lr;
		shm[9]=hr;
		shm[10]=p1 & 0xff;
		shm[11]=p2 & 0xff;
		shm[12]=p3 & 0xff;
		shm[13]=p4 & 0xff;

		if(txlen > 0)
		{
			for(i=0;i<txlen;i++)
			{
				shm[14+i]=memory[txaddr+i];
			}
		}
		shm[txlen+14]=0xcc;		//last byte is 0xcc

		shm[1]=0xcd;			//start transfer
	
//		printf("sent data\n");
//		printf("CMD = %02X %02X\n",shm[4],shm[5]);
	
		while(shm[1] == 0xcd) usleep(10);

//		printf("get data\n");
	
		//now read back result
		offset=0;
		while(offset < rxlen)
		{
			memory[rxaddr+offset]=shm[14+offset];
			offset++;
		}

//		printf("RES = %02X %02X\n",shm[1],shm[14]);

		return shm[14+offset] & 0xff;
	}
	
	else
	
	{
		unsigned char combuf[9000];
		
		com_buf[0]=(0xab + (cmd >> 8)) & 0xff;
		com_buf[1]=cmd & 0xff;
		com_buf[2]=lt;
		com_buf[3]=ht;
		com_buf[4]=lr;
		com_buf[5]=hr;
		com_buf[6]=p1 & 0xff;
		com_buf[7]=p2 & 0xff;
		com_buf[8]=p3 & 0xff;
		com_buf[9]=p4 & 0xff;

		if(txlen > 0)
		{
			for(i=0;i<txlen;i++)
			{
				com_buf[10+i]=memory[txaddr+i];
			}
		}
		com_buf[txlen+10]=0xcc;		//last byte is 0xcc
	
		usb_io(txlen,rxlen);

		offset=0;
		while(offset < rxlen)
		{
			memory[rxaddr+offset]=com_buf[11+offset];
			offset++;
		}

//		printf("RES = %02X %02X\n",shm[1],shm[14]);

		return com_buf[11+offset] & 0xff;
	}
}


//----------------------------------------------------------------------------------
// read voltages from programmer
//----------------------------------------------------------------------------------
int read_volt()
{
	int result;

//	result = prg_comm(int cmd,int txlen,int rxlen,long txaddr,long rxaddr,int p1,int p2,int p3,int p4)
	result = prg_comm(0xf8,0,3,0,0,0,0,0,0);

	if(result == 0)
	{
		v_batt=memory[0];
		v_ext=memory[1];
		v_prog=memory[2];
		v_batt/=10;
		v_ext/=10;
		v_prog/=10;
		if((interface_type == 1) || (interface_type == 3)) printf("V-batt  = %4.1fV\n",v_batt);
		printf("V-Ext     = %3.1fV\n",v_ext);
		printf("V-PROG    = %3.1fV\n",v_prog);
	}
	else
	{
		printf("ERROR CODE %X\n\n",result);
	}
	return result;
}

//----------------------------------------------------------------------------------
// read info from programmer
//----------------------------------------------------------------------------------
int read_info()
{
	int result;
	float ver;

//	result = prg_comm(int cmd,int txlen,int rxlen,long txaddr,long rxaddr,int p1,int p2,int p3,int p4)
	result = prg_comm(0xf0,0,4,0,0,0,0,0,0);


	if(result == 0)
	{
		blver=memory[0];
		ver=blver & 0x3f;
		programmer_type=(blver >> 6) & 3;
		ver=ver/10;
		max_blocksize=256*memory[1];
		switch(programmer_type)
		{
			case 1: 	printf("ProgType  = ParaProg\n");break;
			case 2: 	printf("ProgType  = EProg2\n");break;
			case 3: 	printf("ProgType  = reserved\n");break;
			default: 	printf("ProgType  = UPROG2\n");
		}
		printf("SYS-Ver   = %3.1f\n",ver);
		printf("Blocksize = %d\n",max_blocksize);
		sysversion=265*memory[2]+memory[3];
		printf("PRG-Ver   = %04d\n",memory[2]*256+memory[3]);
	}
	else
	{
		sysversion=0;
		printf("ERROR CODE %X\n\n",result);
	}
	return result;
}

//----------------------------------------------------------------------------------
// progress bar
//----------------------------------------------------------------------------------
void progress(char *mystring, int v_max, int v_act)
{
	char funct[22];
	char fill[52];
	int i;
	strncpy(funct,mystring,20);
	for(i=strlen(mystring);i<21;i++) funct[i]=0x20;
	funct[i]=0;
	
	for(i=0;i<50;i++)
	{
		if(i<=((v_act*50)/v_max))
			fill[i]='*';
		else
			fill[i]='.';
	}
	fill[50]=0;
	printf("%s |%s|\r",funct,fill);
	fflush(stdout);
}

//----------------------------------------------------------------------------------
// check for block is completely 0xff
//----------------------------------------------------------------------------------
int must_prog(long mad,int blen)
{
	int i,j;
	j=0;
	for(i=0;i<blen;i++)
	{
		if(memory[mad+i] != 255) j=1;
	}
//	printf("ADDR= %04X RES: %d\n",mad,j);
	return j;
}

int must_prog_used(long mad,int blen)
{
	int i,j;
	j=0;
	for(i=0;i<blen;i++)
	{
		if(memory_used[mad+i] != 0) j=1;
	}
//	printf("ADDR= %04X RES: %d\n",mad,j);
	return j;
}

int must_prog_pic16(long mad,int blen)
{
	int i,j;
	j=0;
	for(i=0;i<blen;i+=2)
	{
		if(memory[mad+i] != 0xFF) j=1;
		if((memory[mad+i+1] & 0x3F) != 0x3F) j=1;
	}
//	printf("ADDR= %04X RES: %d\n",mad,j);
	return j;
}


//----------------------------------------------------------------------------------
// check for block is completely 0x00
//----------------------------------------------------------------------------------
int check_00(long mad,int blen)
{
	int i,j;
	j=1;
	for(i=0;i<blen;i++)
	{
		if(memory[mad+i] != 0) j=0;
	}
//	printf("ADDR= %04X RES: %d\n",mad,j);
	return j;
}

int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void waitkey(void)
{
		printf("\nPRESS ENTER TO EXIT \n");
		getchar();
}

int get_currentkey(void) 
{
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
    
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
}


void waitkey_dbg2(void)
{
		printf("\nPRESS ENTER TO BREAK\n");
		getchar();
}


int abortkey(void)
{
	int j;
	
	printf("\nPRESS ENTER TO ABORT \n");
	do
	{
		usleep(1000);
		memory[0]=0;
		j=prg_comm(0x19f,1,0,0,0,0,0,0,0);	//check if done
	}while((!(kbhit())) && (j==0));

	printf("RDAT: %02X %02X\n",shm[1],shm[14]);
		
	if(j==0)
	{
		printf("==> Aborted\n");
		return 1;
	}
	else
	{
		printf("==> Done\n");
	}
	return 0;	
}



int find_cmd(char *cptr)
{
	if(strstr(cmd,cptr) && ((strstr(cmd,cptr)-cmd) %2 == 1))
	{
		return(1);
	}
	else return(0);
}

void show_data(long addr, int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		printf("ADDR= %08lX  DATA= %02X\n",addr+i,memory[addr+i]);
	}
}

void show_bdata(unsigned long addr, int len, unsigned long maddr)
{
	int i;

	printf("0x%08lX  : ",maddr);
	
	for(i=0;i<len;i++)
	{
		printf(" %02X",memory[addr+i]);
	}
	printf("\n");
	
}


void show_wdata(unsigned long addr, int len, unsigned long maddr)
{
	int i;

	printf("0x%08lX  : ",maddr);
	
	for(i=0;i<len;i+=2)
	{
		printf(" %04X",memory[addr+i] + (memory[addr+i+1] << 8));
	}
	printf("\n");
	
}

void show_bdata1(unsigned short addr, int len, unsigned long maddr)
{
	int i;

	printf("0x%04X  : ",(unsigned short)maddr);
	
	for(i=0;i<len;i++)
	{
		printf(" %02X",memory[addr+i]);
	}
	printf("\n");
	
}


void show_wdata1(unsigned short addr, int len, unsigned long maddr)
{
	int i;

	printf("0x%04X  : ",(unsigned short)maddr);
	
	for(i=0;i<len;i+=2)
	{
		printf(" %04X",memory[addr+i] + (memory[addr+i+1] << 8));
	}
	printf("\n");
	
}


void show_ldata(unsigned long addr, int len, unsigned long maddr)
{
	int i;

	printf("0x%08lX  : ",maddr);
	
	for(i=0;i<len;i+=4)
	{
		printf(" %08lX",((unsigned long)memory[addr+i] + (unsigned long)(memory[addr+i+1] << 8) + (unsigned long)(memory[addr+i+2] << 16)+ (unsigned long)(memory[addr+i+3] << 24)) & 0xFFFFFFFF);
	}
	printf("\n");
	
}


void show_data4_b(long addr, int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		printf("ADDR= %08lX  DATA= %08lX\n",addr+4*i,	(unsigned long)((memory[addr+4*i]<<24) |
								(memory[addr+4*i+1]<<16) |
								(memory[addr+4*i+2]<<8) |
								(memory[addr+4*i+3])));
	}
}

void show_data4_l(long addr, int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		printf("ADDR= %08lX  DATA= %08lX\n",addr+4*i,	(unsigned long)((memory[addr+4*i]) |
								(memory[addr+4*i+1]<<8) |
								(memory[addr+4*i+2]<<16) |
								(memory[addr+4*i+3]<<24)));
	}
}



//----------------------------------------------------------------------------------
// flag check tests
//----------------------------------------------------------------------------------
int check_cmd_prog(char *cptr,char *tptr)
{
	if(strstr(cmd,cptr) && ((strstr(cmd,cptr)-cmd) %2 == 1))
	{
		if(file_found < 2)
		{
			printf("## Action: %s program !! DISABLED BECAUSE OF NO FILE !!\n",tptr);
			file_err=1;
			return 0;
		}
		else
		{
			printf("## Action: %s program using %s",tptr,sfile);
			if(file2_found ==2) printf(", %s",sfile2);
			if(file3_found ==2) printf(", %s",sfile3);
			if(file4_found ==2) printf(", %s",sfile4);
			printf("\n");
			return 1;
		}
	}
	return 0;
}

int check_cmd_verify(char *cptr,char *tptr)
{
	if(strstr(cmd,cptr) && ((strstr(cmd,cptr)-cmd) %2 == 1))
	{
		if(file_found < 2)
		{
			printf("## Action: %s verify !! DISABLED BECAUSE OF NO FILE !!\n",tptr);
			file_err=1;
			return 0;
		}
		else
		{
			printf("## Action: %s verify using %s",tptr,sfile);
			if(file2_found ==2) printf(", %s",sfile2);
			if(file3_found ==2) printf(", %s",sfile3);
			if(file4_found ==2) printf(", %s",sfile4);
			printf("\n");
			return 1;
		}
	}
	return 0;
}


int check_cmd_read(char *cptr,char *tptr,int *pptr,int *vptr)
{
	if(strstr(cmd,cptr) && ((strstr(cmd,cptr)-cmd) %2 == 1))
	{
		if((*pptr + *vptr) > 0)
		{
			printf("## Action: %s read !! DISABLED BECAUSE OF PROG/VERIFY !!\n",tptr);
			return 0;
		}
		
		if(file_found < 1)
		{
			printf("## Action: %s read !! DISABLED BECAUSE OF NO FILE !!\n",tptr);
			file_err=1;
			return 0;
		}
		else
		{
			printf("## Action: %s read to %s\n",tptr,sfile);
			return 1;
		}
	}
	return 0;
}


int check_cmd_read2(char *cptr,char *tptr)
{
	if(strstr(cmd,cptr) && ((strstr(cmd,cptr)-cmd) %2 == 1))
	{
		if(file_found < 1)
		{
			printf("## Action: %s read !! DISABLED BECAUSE OF NO FILE !!\n",tptr);
			file_err=1;
			return 0;
		}
		else
		{
			printf("## Action: %s read to %s\n",tptr,sfile);
			return 1;
		}
	}
	return 0;
}


int check_cmd_run(char *cptr)
{
	if(strstr(cmd,cptr) && ((strstr(cmd,cptr)-cmd) %2 == 1))
	{
		if(file_found < 2)
		{
			printf("## Action: run code !! DISABLED BECAUSE OF NO FILE !!\n");
			return 0;
		}
		else
		{
			printf("## Action: run code using %s",sfile);
			if(file2_found ==2) printf(", %s",sfile2);
			if(file3_found ==2) printf(", %s",sfile3);
			if(file4_found ==2) printf(", %s",sfile4);
			printf("\n");
			return 1;
		}
	}
	return 0;
}


void set_error(char *emessage,int errnum)
{
	int i,l;

	if(errnum > 9)
	{
		sprintf(error_line,"ERROR %02X (%d):  %s",errnum,errnum,emessage);
	}
	else if(file_err != 0)
	{
		sprintf(error_line,"FILE ERROR");
	}
	else
	{
		sprintf(error_line,"%s",emessage);
	}
	
	l=strlen(error_line);
	for(i=l;i<99;i++) error_line[i]=0x20;
	error_line[99]=0x00;
}

void set_error2(char *emessage,int errnum,unsigned long addr)
{
	int i,l;

	if(errnum < 10)
	{
		sprintf(error_line,"%s",emessage);
	}
	else
	{
		sprintf(error_line,"ERROR %02X (%d):  %s AT 0x%08lX",errnum,errnum,emessage,addr);
	}
	
	l=strlen(error_line);
	for(i=l;i<99;i++) error_line[i]=0x20;
	error_line[99]=0x00;
}

void print_error(void)
{
	printf("%s\n",error_line);
}


void paraprog_view(int offset)
{
	printf("CTRL= ");
	if(memory[offset+7] & 0x80) printf("w"); else printf("W");
	if(memory[offset+7] & 0x40) printf("o"); else printf("O");
	if(memory[offset+7] & 0x20) printf("c "); else printf("C ");
	if(memory[offset+7] & 0x10) printf("D"); else printf("d");
	if(memory[offset+7] & 0x02) printf("r"); else printf("R");
			
	printf("   ADDR= %02X%02X%02X%02X   DATA= %02X%02X\n",memory[offset+5],memory[offset+4],memory[offset+3],memory[offset+2],memory[offset+1],memory[offset+0]);
}


//----------------------------------------------------------------------------------
// logging functions
//----------------------------------------------------------------------------------
void addlog1(char *ltext)
{
	if(loglevel > 0)
	{
		strcat(logdata,"# ");
		strcat(logdata,ltext);
		strcat(logdata,"\n");
	}
}

void addlog2(char *ltext)
{
	if(loglevel > 1)
	{
		strcat(logdata,"# ");
		strcat(logdata,ltext);
		strcat(logdata,"\n");
	}
}

void addlog3(char *ltext)
{
	if(loglevel > 2)
	{
		strcat(logdata,"# ");
		strcat(logdata,ltext);
		strcat(logdata,"\n");
	}
}

void addlog4(char *ltext)
{
	if(loglevel > 3)
	{
		strcat(logdata,"# ");
		strcat(logdata,ltext);
		strcat(logdata,"\n");
	}
}

