
//----------------------------------------------------------------------------------
// send a command to programmer
//----------------------------------------------------------------------------------
int prg_comm(int cmd,int txlen,int rxlen,long txaddr,long rxaddr,int p1,int p2,int p3,int p4);
void flush_serial_in(void);

//----------------------------------------------------------------------------------
// read voltages from programmer
//----------------------------------------------------------------------------------
int read_volt();

int read_info();

//----------------------------------------------------------------------------------
// set vpp
//----------------------------------------------------------------------------------
int set_vpp();

void progress(char *mystring, int v_max, int v_act);

int must_prog(long mad,int blen);
int must_prog_used(long mad,int blen);
int must_prog_pic16(long mad,int blen);

int kbhit(void);
void waitkey(void);
void waitkey_dbg2(void);
int abortkey(void);
int get_currentkey(void);

int find_cmd(char *cptr);

void show_data(long,int);
void show_bdata(unsigned long,int,unsigned long);
void show_wdata(unsigned long,int,unsigned long);
void show_ldata(unsigned long,int,unsigned long);
void show_bdata1(unsigned short,int,unsigned long);
void show_wdata1(unsigned short,int,unsigned long);
void show_data4_b(long,int);
void show_data4_l(long,int);

int check_cmd_prog(char *cptr,char *tptr);
int check_cmd_verify(char *cptr,char *tptr);
int check_cmd_read(char *cptr,char *tptr,int *pptr,int *vptr);
int check_cmd_run(char *cptr);

int getpids(char *pname);

void set_error(char *, int);
void fillup_119(char *);
void set_error2(char *, int,unsigned long);

void print_error(void);

int check_00(long,int);

void debug_avrjtag_t1(void);

void paraprog_view(int);

void addlog1(char *);
void addlog2(char *);
void addlog3(char *);
void addlog4(char *);
