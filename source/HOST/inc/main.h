#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<termios.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include<subfunct.h>
#include<readhex.h>
#include<writehex.h>
#include<debug.h>

#define ROFFSET 0x2000000			//max 32MB
#define SHM_SIZE 32768+16

#define DEBUG_OUTPUT 0

#define SWD_READ_IDCODE 0xA5
#define SWD_WRITE_ABORT 0x81

#define SWD_READ_CTRLSTAT 0xB1
#define SWD_WRITE_CTRLSTAT 0x95

#define SWD_READ_STAT 0xA9
#define SWD_WRITE_SELECT 0x8D

#define SWD_READ_RDBUFF 0xBD

#define SWD_READ_CSW 0xE1
#define SWD_WRITE_CSW 0xC5

#define SWD_READ_TAR 0xF5
#define SWD_WRITE_TAR 0xD1

#define SWD_READ_IDR 0xED
#define SWD_WRITE_BASE 0xC9

#define SWD_READ_DRW 0xF9
#define SWD_WRITE_DRW 0xDD

unsigned char *memory;		//global memory map
unsigned char *memory_used;	//global memory map (used memory)
unsigned char *shm;
unsigned long shmkey;
char sfile[300];		//data file
char sfile2[300];		//data file
char sfile3[300];		//data file
char sfile4[300];		//data file
char tfile[300];		//temporary data file
char logdata[10000];		//additional logdata
char cmd[100];			//command
char stype[300];
char name[50];			//device name
char error_line[100];
char *web_buffer;
long max_post_data;
long post_data_start;
int hold_vdd;
int range_err;
int file_err;
int loglevel;
unsigned char com_buf[33000];

unsigned long loaddr,hiaddr;
unsigned long s1,e1,s2,e2,s3,e3,s4,e4,devid;
unsigned long err_addr;
char err_mdata[10];
char err_rdata[10];
int save_data,dev_found,cmd_found,file_found,tfile_found,file2_found,file3_found,file4_found;
//int fd;
int is_error;
int pmode;
int pvalue;
unsigned int have_expar,have_expar2,have_expar3,have_expar4;
unsigned long expar,expar2,expar3,expar4;
unsigned long param[20];
int algo_nr;
FILE *datei;
FILE *tdatei;
FILE *ldatei;
float v_batt,v_ext,v_prog;
unsigned char rwbuffer[4096];
int max_blocksize;
int interface_type;
int file_mode,blver,tfile_mode,file2_mode,file3_mode,file4_mode;
unsigned int sysversion;

int cpld_datasize;

typedef struct typelist
{
	char name[40];
	int algo;
	unsigned long par[19];
} devicedat;

int force_exit;
int use_vanilla_pid;
int programmer_type;
