//###############################################################################
//#										#
//# UPROG2 universal programmer							#
//#										#
//# copyright (c) 2010-2022 Joerg Wolfram (joerg@jcwolfram.de)			#
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

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<termios.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#include <main.h>
#include <devices.h>
#include <algorithm.h>
#include <wires.h>

extern int daemon_task(int);
extern int check_usb(int);
extern void close_usb(void);

int main(int argc, char *argv[])
{
	int ii,jj,pids,shmid;
	pid_t mypid;
	int errcode=0;
	
	range_err=0;
	
	use_vanilla_pid=0;
	force_exit=0;
	is_error=0;
	file_err=0;
	jj=0;

	char logtime[100];
	char *logtime_ptr;
	time_t current_time;
	char *time_string;
	char *time_string_ptr;
	
	current_time = time(NULL);
	time_string = ctime(&current_time);
	time_string_ptr=time_string;
	logtime_ptr=logtime;
	while(*time_string_ptr > 31)
	{
		*logtime_ptr++ = *time_string_ptr++; 
	}
	*logtime_ptr = 0;
//	printf("TIME=<%s>\n",time_string);
	logdata[0]=0;
	
	do
	{
		jj++;
	}while(strncmp("END",valid_devices[jj].name,20) != 0);
	
	jj-=3;	//sub common functions

	//try to allocate memory for programming data
	memory = malloc(ROFFSET * 2 * sizeof(unsigned char));
	memory_used = malloc(ROFFSET * sizeof(unsigned char));

	if((memory == NULL) || (memory_used == NULL))
	{
		printf("No memory available\n");
		return 1;
	}
	
	pids=getpids("uprog2");

	printf("\n");
	printf("#################################################################################\n");
	printf("#                                                                               #\n");
	printf("#  UNI-Programmer UPROG2 V1.43                                                  #\n");
	printf("#                                                                               #\n");
	printf("#  (c) 2012-2022 Joerg Wolfram                                                  #\n");
	printf("#                                                                               #\n");
	printf("#  usage: uprog2 device -commands [up to 4 files/data]                          #\n");
	printf("#         uprog2 KILL                            kill active daemon             #\n");
	printf("#         uprog2 LIST                            for device list                #\n");
	printf("#         uprog2 device -help                    for device specific commands   #\n");
	printf("#                                                                               #\n");
	printf("# %5d types in database                                                       #\n",jj);
	printf("#                                                                               #\n");
	printf("#################################################################################\n");

	if(argc < 2)
	{
		printf("\nNot enough parameters. \nusage: uprog model -commands [file/data]\n\n");
		return 1;
	}

	if((pids > 2) && (interface_type != 1))
	{
		printf("\nuprog2 is already started. Cannot start another instance.\n\n");
		return 1;
	}

	if((pids > 2) && (interface_type == 1))
	{
		printf("\nuprog2 is already started. Cannot start another instance.\n\n");
		return 1;
	}

	if(strncmp(argv[1],"LIST4",5) == 0)
	{
		list4_devices();
		return(0);	
	}

	if(strncmp(argv[1],"LIST",4) == 0)
	{
		list_devices();
		return(0);	
	}

	interface_type=0;


		
	if((strncmp(argv[1],"KILL",4) != 0) && (force_exit == 0))
	{
		//printf("\n## NUMBER OF PIDs = %d\n",pids);					
		if(pids == 1)
		{

			if(argc > 2)
			{
				//search for cmd string
				if (strncmp(argv[2],"-",1) == 0)
				{
					strncpy(cmd,argv[2],80);
				}
			}

			if(find_cmd("vp"))
			{
				use_vanilla_pid = 1;
				printf("\n## TRY TO USE ORIGINAL FTDI PID\n");					
			
			}
			
			printf("CHECK USB PROGRAMMER... ");					
			if(check_usb(use_vanilla_pid) == 0)
			{ 
				interface_type=9;
				printf("FOUND, USING DIRECT MODE\n\n");
			}
			else
			{
				close_usb(); 
				printf("NOT FOUND!\n\n");
			}
				
			//use direct mode if we use the USB programmer
			if(interface_type != 9)
			{ 
				printf("\nuprog2 daemon is not active, starting daemon and searching for programmer...\n\n");		
				mypid=fork();
		
				if(mypid == 0)
				{
					return(daemon_task(use_vanilla_pid));
				}
				else
				{	//wait until daemon is active
					usleep(200000);
				}
			}
		}
	}

#if DEBUG_OUTPUT == 1
		printf("!!! DEBUG MODE ACTIVATED !!!\n\n");
#endif

	if(interface_type < 9)
	{

		shmid=shmget(1887699,SHM_SIZE,0);
		if(shmid < 0) 
		{
			printf("Shared memory segment is not available. Possible daemon is not active.\n\n"); return 1;
		}
	
		shm=shmat(shmid,NULL,0);
		if(shm == (unsigned char *) -1)
		{
			printf("Cannot attach memory segment.\n\n"); return 1;
		}

		//wait for deamon to find a programmer
		usleep(10000);
		while(shm[0] == 0) usleep(1000);
	
	
		if(shm[0] == 0x1f)
		{
			printf("No programmer found, aborting\n\n"); 
			shm[0]=1;					//force exit
			shmdt(shm);
			return 1;
		}

		if(shm[2] == 1)
		{
			printf(">> Bluetooth programmer is active.\n\n"); 
			shm[0]=1;					//send ACK to daemon
			interface_type=1;
		}
	
		if(shm[2] == 3)
		{
			printf(">> Parallel Bluetooth programmer is active.\n\n"); 
			shm[0]=1;					//send ACK to daemon
			interface_type=3;
		}
	}

	//now search par 1 for a valid device
	jj=0;
	dev_found=0;
	do
	{
		if(strncmp(argv[1],valid_devices[jj].name,20) == 0)
		{
			dev_found = 1;
			algo_nr=valid_devices[jj].algo;
			strncpy(name,valid_devices[jj].name,20);
			for(ii=0;ii<19;ii++)
			{
				param[ii]=valid_devices[jj].par[ii];
			}
		}
		jj++;
	}while(strncmp("END",valid_devices[jj].name,20) != 0);

	if(algo_nr == 200)
	{
		list_devices();
		return(0);
	}

	if(dev_found == 0)
	{
		printf("no supported model found\n\n");
		return 1;
	}


	if(algo_nr < 190)
	{
//		printf("ALGO: %d \n",algo_nr);
		printf("*** %s can be programmed with: %s ***\n\n",name,cables[algo_nr]);
	}	

	// (dummy)devices >99 (LIST,KILL,WSERVER) need no command
	if((algo_nr < 200) || (algo_nr > 209))
	{
		cmd_found = 0;
		if(argc > 2)
		{
			//search for cmd string
			if (strncmp(argv[2],"-",1) == 0)
			{
				strncpy(cmd,argv[2],80);
				cmd_found = 1;
			}
		}

		if(cmd_found == 0)
		{
			printf("no command selected\n\n");
			return 1;
		}
	}

	//check for file
	file_found = 0;		//no file
	have_expar = 0;		//no parameter
	
	if(argc > 3)
	{
		if(strncmp("0x",argv[3],2) == 0)
		{
			sscanf(strndup(argv[3],10),"%lx",&expar);
			printf("PARAM  =  0x%lx\n",expar);
			have_expar=1;
		}
		else if(strncmp("d:",argv[3],2) == 0)
		{
			sscanf(strndup(argv[3]+2,10),"%ld",&expar);
			printf("PARAM  =  %ld\n",expar);
			have_expar=1;
		}			
		else
		{
			file_found=1;				//we have a filename
			file_mode=0;				//Motorola s37
			strncpy(sfile,argv[3],280);
			if((sfile +strlen(sfile) - strstr(sfile,".hex")) == 4 ) file_mode=1;	//Intel hex	
			if((sfile +strlen(sfile) - strstr(sfile,".HEX")) == 4 ) file_mode=1;	//Intel hex	
			if((sfile +strlen(sfile) - strstr(sfile,".s28")) == 4 )	file_mode=2;	//Motorola S28	
			if((sfile +strlen(sfile) - strstr(sfile,".S28")) == 4 ) file_mode=2;	//Motorola S28
			if((sfile +strlen(sfile) - strstr(sfile,".s19")) == 4 ) file_mode=3;	//Motorola S19	
			if((sfile +strlen(sfile) - strstr(sfile,".S19")) == 4 ) file_mode=3;	//Motorola S19
				
			datei=fopen(sfile,"r");
			if(datei != NULL)
			{
				fclose(datei);
				file_found=2;		//we can read the file
			}
		}	
	}

	if(argc > 4)
	{
		if(strncmp("0x",argv[4],2) == 0)
		{
			sscanf(strndup(argv[4],10),"%lx",&expar2);
			printf("PARAM2 =  0x%lx\n",expar2);
			have_expar2=1;
		}
		else if(strncmp("d:",argv[4],2) == 0)
		{
			sscanf(strndup(argv[4]+2,10),"%ld",&expar2);
			printf("PARAM2 =  %ld\n",expar2);
			have_expar2=1;
		}			
		else
		{
			file2_found=1;				//we have a filename
			file2_mode=0;				//Motorola s37
			strncpy(sfile2,argv[4],280);
			if((sfile2 +strlen(sfile2) - strstr(sfile2,".hex")) == 4 ) file2_mode=1;	//Intel hex	
			if((sfile2 +strlen(sfile2) - strstr(sfile2,".HEX")) == 4 ) file2_mode=1;	//Intel hex	
			if((sfile2 +strlen(sfile2) - strstr(sfile2,".s28")) == 4 ) file2_mode=2;	//Motorola S28	
			if((sfile2 +strlen(sfile2) - strstr(sfile2,".S28")) == 4 ) file2_mode=2;	//Motorola S28
			if((sfile2 +strlen(sfile2) - strstr(sfile2,".s19")) == 4 ) file2_mode=3;	//Motorola S19	
			if((sfile2 +strlen(sfile2) - strstr(sfile2,".S19")) == 4 ) file2_mode=3;	//Motorola S19
				
			datei=fopen(sfile2,"r");
			if(datei != NULL)
			{
				fclose(datei);
				file2_found=2;		//we can read the file
			}
		}	
	}


	if(argc > 5)
	{
		if(strncmp("0x",argv[5],2) == 0)
		{
			sscanf(strndup(argv[5],10),"%lx",&expar3);
			printf("PARAM2 =  0x%lx\n",expar3);
			have_expar3=1;
		}
		else if(strncmp("d:",argv[5],2) == 0)
		{
			sscanf(strndup(argv[5]+2,10),"%ld",&expar3);
			printf("PARAM2 =  %ld\n",expar3);
			have_expar3=1;
		}			
		else
		{
			file3_found=1;				//we have a filename
			file3_mode=0;				//Motorola s37
			strncpy(sfile3,argv[5],280);
			if((sfile3 +strlen(sfile3) - strstr(sfile3,".hex")) == 4 ) file3_mode=1;	//Intel hex	
			if((sfile3 +strlen(sfile3) - strstr(sfile3,".HEX")) == 4 ) file3_mode=1;	//Intel hex	
			if((sfile3 +strlen(sfile3) - strstr(sfile3,".s28")) == 4 ) file3_mode=2;	//Motorola S28	
			if((sfile3 +strlen(sfile3) - strstr(sfile3,".S28")) == 4 ) file3_mode=2;	//Motorola S28
			if((sfile3 +strlen(sfile3) - strstr(sfile3,".s19")) == 4 ) file3_mode=3;	//Motorola S19	
			if((sfile3 +strlen(sfile3) - strstr(sfile3,".S19")) == 4 ) file3_mode=3;	//Motorola S19
				
			datei=fopen(sfile3,"r");
			if(datei != NULL)
			{
				fclose(datei);
				file3_found=2;		//we can read the file
			}
		}	
	}

	if(argc > 6)
	{
		if(strncmp("0x",argv[6],2) == 0)
		{
			sscanf(strndup(argv[6],10),"%lx",&expar4);
			printf("PARAM2 =  0x%lx\n",expar4);
			have_expar4=1;
		}
		else if(strncmp("d:",argv[6],2) == 0)
		{
			sscanf(strndup(argv[6]+2,10),"%ld",&expar4);
			printf("PARAM2 =  %ld\n",expar4);
			have_expar4=1;
		}			
		else
		{
			file4_found=1;				//we have a filename
			file4_mode=0;				//Motorola s37
			strncpy(sfile4,argv[6],280);
			if((sfile4 +strlen(sfile4) - strstr(sfile4,".hex")) == 4 ) file4_mode=1;	//Intel hex	
			if((sfile4 +strlen(sfile4) - strstr(sfile4,".HEX")) == 4 ) file4_mode=1;	//Intel hex	
			if((sfile4 +strlen(sfile4) - strstr(sfile4,".s28")) == 4 ) file4_mode=2;	//Motorola S28	
			if((sfile4 +strlen(sfile4) - strstr(sfile4,".S28")) == 4 ) file4_mode=2;	//Motorola S28
			if((sfile4 +strlen(sfile4) - strstr(sfile4,".s19")) == 4 ) file4_mode=3;	//Motorola S19	
			if((sfile4 +strlen(sfile4) - strstr(sfile4,".S19")) == 4 ) file4_mode=3;	//Motorola S19
				
			datei=fopen(sfile4,"r");
			if(datei != NULL)
			{
				fclose(datei);
				file4_found=2;		//we can read the file
			}
		}	
	}

	
	if(algo_nr == 210)
	{
		tfile_found=1;				//we have a filename
		tfile_mode=0;				//Motorola S-Record
		strncpy(tfile,argv[4],280);
		if((tfile +strlen(tfile) - strstr(tfile,".hex")) == 4 ) tfile_mode=1;	//Intel hex	
		if((tfile +strlen(tfile) - strstr(tfile,".HEX")) == 4 ) tfile_mode=1;	//Intel hex	
		if((tfile +strlen(tfile) - strstr(tfile,".s28")) == 4 )	tfile_mode=2;	//Motorola S28	
		if((tfile +strlen(tfile) - strstr(tfile,".S28")) == 4 ) tfile_mode=2;	//Motorola S28
		if((tfile +strlen(tfile) - strstr(tfile,".s19")) == 4 ) tfile_mode=3;	//Motorola S19	
		if((tfile +strlen(tfile) - strstr(tfile,".S19")) == 4 ) tfile_mode=3;	//Motorola S19
		datei=fopen(tfile,"r");
		if(tdatei != NULL)
		{
			fclose(tdatei);
			tfile_found=2;		//we can read the file
		}
	}

	if((algo_nr < 200) || (algo_nr == 202))
	{
		errcode=read_info();
		if(programmer_type == 0)
		{
			if(errcode == 0) read_volt();
			if(errcode == 0) 
			{
				if(check_update()== -1) read_volt();
			}
		}

		if(programmer_type == 1)
		{
			if(errcode == 0) read_volt();
			if(errcode == 0) 
			{
				if(check_update_par()== -1) read_volt();
			}
		}
	
	}

	loglevel=0;
	if(find_cmd("l1")) loglevel=1;
	if(find_cmd("l2")) loglevel=2;
	if(find_cmd("l3")) loglevel=3;
	if(find_cmd("l4")) loglevel=4;
	if(loglevel > 0) printf("\n## SET LOGLEVEL TO %d\n",loglevel);					



	//###############################################################################
	//# default UPROG2								#
	//###############################################################################	
	if((errcode == 0) && (programmer_type == 0))
	{
		switch (algo_nr)
		{
			case 1:		errcode=prog_s08();		break;
			case 2:		errcode=prog_r8c();		break;
			case 3:		errcode=prog_avr();		break;
			case 4:		errcode=prog_msp430a();		break;
			case 5:		errcode=prog_msp430b();		break;
			case 6:		errcode=prog_s12xe();		break;
			case 7:		errcode=prog_s12xd();		break;
			case 8:		errcode=prog_stm8();		break;
//			case 9:		errcode=prog_c2000();		break;
			case 10:	errcode=prog_dspic33();		break;
			case 11:	errcode=prog_s12xs();		break;
			case 12:	errcode=prog_nec2();		break;
			case 13:	errcode=prog_rl78();		break;
			case 14:	errcode=prog_pic16a();		break;
			case 15:	errcode=prog_pic16b();		break;
			case 16:	errcode=prog_ppcbam();		break;
			case 17:	errcode=prog_pic18a();		break;
			case 18:	errcode=prog_dspic30();		break;
			case 20:	errcode=prog_st7f();		break;
			case 21:	errcode=prog_i2c();		break;
			case 22:	errcode=prog_spiflash();	break;
			case 23:	errcode=prog_dataflash();	break;
			case 26:	errcode=prog_rh850();		break;


			case 30:	errcode=prog_xc9500();		break;
			case 31:	errcode=prog_cc25xx();		break;
			case 32:	errcode=prog_psoc4();		break;
			case 33:	errcode=prog_stm32f0();		break;	//f0
			case 34:	errcode=prog_stm32f1();		break;	//f1
			case 35:	errcode=prog_stm32f2();		break;	//f2
			case 36:	errcode=prog_stm32f3();		break;	//f3
			case 37:	errcode=prog_stm32f4();		break;	//f4
			case 38:	errcode=prog_pic16a();		break;	//pic12
			case 40:	errcode=prog_atxmega();		break;

			case 42:	errcode=prog_v850();		break;
			case 43:	errcode=prog_mlx363();		break;
			case 44:	errcode=prog_ppcjtag();		break;
			case 45:	errcode=prog_ppcjtag2();	break;
			case 46:	errcode=prog_tle986x();		break;
			case 47:	errcode=prog_spieeprom();	break;

			case 49:	errcode=read_lps25h();		break;
			case 50:	errcode=prog_mlx316();		break;
			case 51:	errcode=prog_cc2640();		break;
			case 52:	errcode=prog_stm32l4();		break;	//l4
			case 53:	errcode=prog_s32k11swd();	break;	//s32k11x
			case 54:	errcode=prog_ppcjtag3();	break;
			case 55:	errcode=prog_pic16c();		break;	//pic16 new
			case 56:	errcode=prog_kea64swd();	break;	//KEA64
			case 57:	errcode=prog_pic16d();		break;	// mod EEPROM
			case 58:	errcode=prog_rf430();		break;
			case 59:	errcode=prog_sici();		break;
			case 60:	errcode=prog_avr0();		break;
			case 61:	errcode=prog_at89s8252();	break;
			case 62:	errcode=prog_s12z();		break;
			case 63:	errcode=prog_ppcjtag4();	break;
			case 64:	errcode=prog_efm32swd();	break;	//EFM32
			case 65:	errcode=prog_stm32f7();		break;	//f7
			case 66:	errcode=prog_onewire();		break;	//onewire EEPROM
			case 67:	errcode=prog_avrjtag();		break;	//AVR over JTAG
//			case 68:	errcode=prog_samdswd();		break;	//ATSAMD over SWD
			case 69:	errcode=read_veml3328();	break;
			case 70:	errcode=prog_pic16e();		break;	//pic16 new + EEPROM
//			case 71:	errcode=prog_mb91();		break;	//MB51...
			case 72:	errcode=prog_ra6();		break;	//renesas RA6M1/M2/T1
			case 73:	errcode=prog_at24rf08();	break;	//AT24RF08
			case 74:	errcode=xspi();			break;	//AVR SPI-extension
			case 75:	errcode=prog_s32k3swd();	break;	//s32k3xx
			case 76:	errcode=prog_s32k14swd();	break;	//s32k14x
			case 77:	errcode=prog_pic18b();		break;	//pic18 new

			case 189:	errcode=prog_dgen();		break;
			case 197:	errcode=prog_fgen();		break;

			case 201:	printf("Shutting down daemon and disconnect programmer...\n");
					shm[0]=0x2f;
					break;

			case 199:	update();			//update
					break;

			case 210:	errcode=hex2hex();		//dataset converter mode
					break;

			default:	printf("WRONG ALGORITHM\n");
					errcode = 0xFD;
		}
		
	}

	//###############################################################################
	//# paraprog STM32F411								#
	//###############################################################################	
	if((errcode == 0) && (programmer_type == 1))
	{
		switch (algo_nr)
		{
			case 151:	errcode=paraprog_self_test();	break;

			case 152:	errcode=prog_pflash();		break;

			case 201:	printf("Shutting down daemon and disconnect programmer...\n");
					shm[0]=0x2f;
					break;

			case 210:	errcode=hex2hex();		//dataset converter mode
					break;

			case 199:	update_par();		//update
					break;

			default:	printf("WRONG ALGORITHM\n");
					errcode = 0xFD;
		}
		
	}
	
	if((algo_nr < 201) && (errcode != 0x9f))
	{
		prg_comm(0xfa,0,0,0,0,0,0,0,0);	//set back to 3,3V mode
	}

	if(errcode == 0x9f)
	{
		printf("FATAL: CONNECTION TO PROGRAMMER LOST!\n");
		printf("   <<< PLEASE RESET PROGRAMMER >>>\n");
	}

	if(range_err > 0)
	{
		printf("\n!!!! ERROR: Address was out of range for S19/S28 !!!!\n\n");
		errcode = 0xA0;
	}

	if(interface_type == 9) close_usb(); 

	if(loglevel > 0)
	{
		ldatei=fopen("uprog2.log","a");
		if(ldatei != NULL)
		{
				fprintf(ldatei,"-----------------------------------------------------------------------------------\n");
				fprintf(ldatei,"- UPROG2 V1.43 AT %s\n",logtime);
				fprintf(ldatei,"- CMD = <");
				for(ii=0;ii<argc;ii++)
				{ 
					if(ii > 0) fprintf(ldatei," ");
					fprintf(ldatei,"%s",argv[ii]);
				}
				fprintf(ldatei,">\n");
				fprintf(ldatei,logdata);
				if(errcode != 0)
				{
					fprintf(ldatei,"! FAILED WITH CODE 0x%02X\n",errcode);				
				}
				else if(file_err != 0)
				{
					fprintf(ldatei,"! FAILED WITH FILE ERROR\n");				
				}
				else
				{
					fprintf(ldatei,"* PASSED\n");				
				}
				fprintf(ldatei,"\n");
		
				fclose(ldatei);
		}
		else
		{
			printf("!!! CANNOT WRITE TO LOG FILE (uprog2.log) !!!\n");
		}
	}

	free(memory);

	return (errcode | file_err);
}
