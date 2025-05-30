	memory[i++]=SWD_READ_IDCODE;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ IDCODE->0x6BA02477 (WPOS= 0, RPOS= 0)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// (WPOS= 5)
	memory[i++]=SWD_WRITE_ABORT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x1E;	// (WPOS= 10)
	memory[i++]=SWD_WRITE_CTRLSTAT;	memory[i++]=0x50; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// (WPOS= 15)
	memory[i++]=SWD_READ_IDCODE;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ IDCODE->0x6BA02477 (WPOS= 20, RPOS= 4)
	memory[i++]=SWD_READ_CTRLSTAT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CTRLSTAT->0xF0000000 (WPOS= 25, RPOS= 8)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x00000000 (WPOS= 30, RPOS= 12)
	memory[i++]=SWD_READ_CTRLSTAT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CTRLSTAT->0xF0000040 (WPOS= 35, RPOS= 16)
	memory[i++]=SWD_WRITE_ABORT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x1E;	// (WPOS= 40)
	memory[i++]=SWD_WRITE_CTRLSTAT;	memory[i++]=0x50; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x32;	// (WPOS= 45)
	memory[i++]=SWD_READ_CTRLSTAT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CTRLSTAT->0xF0000040 (WPOS= 50, RPOS= 20)
	memory[i++]=SWD_WRITE_CTRLSTAT;	memory[i++]=0xF4; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x40;	// (WPOS= 55)
	memory[i++]=SWD_READ_CTRLSTAT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CTRLSTAT->0xFC000040 (WPOS= 60, RPOS= 24)
	memory[i++]=SWD_WRITE_CTRLSTAT;	memory[i++]=0xF0; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// (WPOS= 65)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x07; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x90;	//select SDA AP, Reset Control (SDAAPRSTCTRL) (WPOS= 70)
	memory[i++]=SWD_WRITE_CSW;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// (WPOS= 75)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x07; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x80;	//select SDA AP, Debug Enable Control (DBGENCTRL) (WPOS= 80)
	memory[i++]=SWD_WRITE_CSW;	memory[i++]=0x30; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0xF0;	// (WPOS= 85)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 90)
	memory[i++]=SWD_READ_CSW;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CSW->0x00000000 (WPOS= 95, RPOS= 28)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x43800042 (WPOS= 100, RPOS= 32)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 105)
	memory[i++]=SWD_WRITE_CSW;	memory[i++]=0x6B; memory[i++]=0x80; memory[i++]=0x00; memory[i++]=0x42;	// (WPOS= 110)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 115)
	memory[i++]=SWD_WRITE_TAR;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//set Adress register to 0x00000000 (WPOS= 120)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x07; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x90;	//select SDA AP, Reset Control (SDAAPRSTCTRL) (WPOS= 125)
	memory[i++]=SWD_READ_CSW;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CSW->0x43800042 (WPOS= 130, RPOS= 36)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x00000000 (WPOS= 135, RPOS= 40)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 140)
	memory[i++]=SWD_WRITE_TAR;	memory[i++]=0xE0; memory[i++]=0x00; memory[i++]=0xED; memory[i++]=0xF0;	//set Adress register to 0xE000EDF0 (DHCSR) (WPOS= 145)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 150)
	memory[i++]=SWD_READ_DRW;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ DRW->0x00000000 (WPOS= 155, RPOS= 44)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x02010000 (WPOS= 160, RPOS= 48)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x02010000 (WPOS= 165, RPOS= 52)
	memory[i++]=SWD_READ_CTRLSTAT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CTRLSTAT->0xF0000040 (WPOS= 170, RPOS= 56)
	memory[i++]=SWD_WRITE_ABORT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x1E;	// (WPOS= 175)
	memory[i++]=SWD_WRITE_CTRLSTAT;	memory[i++]=0x50; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x32;	// (WPOS= 180)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 185)
	memory[i++]=SWD_WRITE_TAR;	memory[i++]=0xE0; memory[i++]=0x00; memory[i++]=0xED; memory[i++]=0xF0;	//set Adress register to 0xE000EDF0 (DHCSR) (WPOS= 190)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 195)
	memory[i++]=SWD_WRITE_DRW;	memory[i++]=0xA0; memory[i++]=0x5F; memory[i++]=0x00; memory[i++]=0x01;	//write data: 0xA05F0001 (WPOS= 200)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x02010000 (WPOS= 205, RPOS= 60)
	memory[i++]=SWD_READ_CTRLSTAT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CTRLSTAT->0xF0000040 (WPOS= 210, RPOS= 64)
	memory[i++]=SWD_WRITE_ABORT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x1E;	// (WPOS= 215)
	memory[i++]=SWD_WRITE_CTRLSTAT;	memory[i++]=0x50; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x32;	// (WPOS= 220)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 225)
	memory[i++]=SWD_WRITE_TAR;	memory[i++]=0xE0; memory[i++]=0x00; memory[i++]=0xED; memory[i++]=0xFC;	//set Adress register to 0xE000EDFC (DEMCR) (WPOS= 230)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 235)
	memory[i++]=SWD_WRITE_DRW;	memory[i++]=0x01; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//write data: 0x01000000 (WPOS= 240)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x02010000 (WPOS= 245, RPOS= 68)
	memory[i++]=SWD_READ_CTRLSTAT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CTRLSTAT->0xF0000040 (WPOS= 250, RPOS= 72)
	memory[i++]=SWD_WRITE_ABORT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x1E;	// (WPOS= 255)
	memory[i++]=SWD_WRITE_CTRLSTAT;	memory[i++]=0x50; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x32;	// (WPOS= 260)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 265)
	memory[i++]=SWD_WRITE_TAR;	memory[i++]=0xE0; memory[i++]=0x00; memory[i++]=0xED; memory[i++]=0x00;	//set Adress register to 0xE000ED00 (CPUID) (WPOS= 270)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 275)
	memory[i++]=SWD_READ_DRW;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ DRW->0x02010000 (WPOS= 280, RPOS= 76)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x411FC272 (WPOS= 285, RPOS= 80)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x411FC272 (WPOS= 290, RPOS= 84)
	memory[i++]=SWD_READ_CTRLSTAT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CTRLSTAT->0xF0000040 (WPOS= 295, RPOS= 88)
	memory[i++]=SWD_WRITE_ABORT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x1E;	// (WPOS= 300)
	memory[i++]=SWD_WRITE_CTRLSTAT;	memory[i++]=0x50; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x32;	// (WPOS= 305)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 310)
	memory[i++]=SWD_WRITE_TAR;	memory[i++]=0xE0; memory[i++]=0x00; memory[i++]=0x20; memory[i++]=0x00;	//set Adress register to 0xE0002000 (FP_CTRL - Flash Patch Control register) (WPOS= 315)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 320)
	memory[i++]=SWD_READ_DRW;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ DRW->0x411FC272 (WPOS= 325, RPOS= 92)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x10000080 (WPOS= 330, RPOS= 96)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x10000080 (WPOS= 335, RPOS= 100)
	memory[i++]=SWD_READ_CTRLSTAT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CTRLSTAT->0xF0000040 (WPOS= 340, RPOS= 104)
	memory[i++]=SWD_WRITE_ABORT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x1E;	// (WPOS= 345)
	memory[i++]=SWD_WRITE_CTRLSTAT;	memory[i++]=0x50; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x32;	// (WPOS= 350)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 355)
	memory[i++]=SWD_WRITE_TAR;	memory[i++]=0xE0; memory[i++]=0x00; memory[i++]=0xED; memory[i++]=0xFC;	//set Adress register to 0xE000EDFC (DEMCR) (WPOS= 360)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 365)
	memory[i++]=SWD_READ_DRW;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ DRW->0x10000080 (WPOS= 370, RPOS= 108)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x01000000 (WPOS= 375, RPOS= 112)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x01000000 (WPOS= 380, RPOS= 116)
	memory[i++]=SWD_READ_CTRLSTAT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CTRLSTAT->0xF0000040 (WPOS= 385, RPOS= 120)
	memory[i++]=SWD_WRITE_ABORT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x1E;	// (WPOS= 390)
	memory[i++]=SWD_WRITE_CTRLSTAT;	memory[i++]=0x50; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x32;	// (WPOS= 395)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 400)
	memory[i++]=SWD_WRITE_TAR;	memory[i++]=0xE0; memory[i++]=0x00; memory[i++]=0xED; memory[i++]=0xFC;	//set Adress register to 0xE000EDFC (DEMCR) (WPOS= 405)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x04; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	//select CORTEX M7_0 AHB (WPOS= 410)
	memory[i++]=SWD_WRITE_DRW;	memory[i++]=0x01; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x01;	//write data: 0x01000001 (WPOS= 415)
	memory[i++]=SWD_READ_RDBUFF;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ RDBUFF->0x01000000 (WPOS= 420, RPOS= 124)
	memory[i++]=SWD_READ_CTRLSTAT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ CTRLSTAT->0xF0000040 (WPOS= 425, RPOS= 128)
	memory[i++]=SWD_WRITE_ABORT;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x1E;	// (WPOS= 430)
	memory[i++]=SWD_WRITE_CTRLSTAT;	memory[i++]=0x50; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x32;	// (WPOS= 435)
	memory[i++]=SWD_READ_IDCODE;	memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// READ IDCODE->0x6BA02477 (WPOS= 440, RPOS= 132)
	memory[i++]=SWD_WRITE_SELECT;	memory[i++]=0x07; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x90;	//select SDA AP, Reset Control (SDAAPRSTCTRL) (WPOS= 445)
	memory[i++]=SWD_WRITE_CSW;	memory[i++]=0x0E; memory[i++]=0x00; memory[i++]=0x00; memory[i++]=0x00;	// (WPOS= 450)
