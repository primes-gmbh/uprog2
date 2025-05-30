const char * const cables[]=
{
	// 0 
	"no cable defined",

	// 1 (HCS08-BDM)
	"(1=VSS  2=VDD  3=RESET  4=BKGD)",

	// 2 (R8C)
	"(1=VSS  2=VDD  3=RESET  4=MODE)",

	// 3 (AVR)
	"(1=VSS  2=VDD  3=RST  4=SCK  5=MISO  6=MOSI)",

	// 4 (MSP430-SBW)
	"(1=VSS  2=VDD  3=TEST  4=RESET)",

	// 5 (MSP430-SBW)
	"(1=VSS  2=VDD  3=TEST  4=RESET)",

	// 6 (S12XE-BDM)
	"(1=VSS  2=VDD  3=RESET  4=BKGD)",

	// 7 (S12XD-BDM)
	"(1=VSS  2=VDD  3=RESET  4=BKGD)",

	// 8 (STM8-SWIM)
	"(1=VSS  2=VDD  3=RESET  4=SWIM)",

	// 9 
	"no connections",

	// 10 (dsPIC33)
	"(1=VSS  2=VDD  3=PGD  4=PGC)",

	// 11 (S12XS-BDM)
	"(1=VSS  2=VDD  3=RESET  4=BKGD)",

	// 12 (78K0R)
	"(1=VSS  2=VDD  3=RESET  4=TOOL0  5=FLMD0)",

	// 13 (RL78)
	"(1=VSS  2=VDD  3=RESET  4=TOOL0)",

	// 14 (PIC16)
	"(1=VSS  2=VDD  3=PGD  4=PGC  9=MCLR)",

	// 15 (PIC16)
	"(1=VSS  2=VDD  3=PGD  4=PGC  9=MCLR)",

	// 16 (PPC-BAM)
	"(1=VSS  2=VDD  3=RESET  4=RX  5=TX  7=FAB)",

	// 17 (PIC18)
	"(1=VSS  2=VDD  3=PGD  4=PGC  9=MCLR)",

	// 18 (dsPIC30)
	"(1=VSS  2=VDD  3=PGD  4=PGC  9=MCLR)",

	// 19 
	"no connections",

	// 20 (ST7FLITE)
	"(1=VSS  2=VDD  3=ICCCLK  4=ICCDATA  5=RESET)",

	// 21 (I2C)
	"(1=VSS  2=VDD  3=SCL  4=SDA)",

	// 22 (SPI flash)
	"(1=VSS  2=VDD  3=/CS  4=SCK  5=SI  6=SO (7=IO2  8=IO3)",

	// 23 (dataflash)
	"(1=VSS  2=VDD  3=/CS  4=SCK  5=MOSI  6=MISO",

	// 24 
	"no connections",

	// 25 (SPI)
	"(1=VSS  2=VDD  3=SEL  4=SCK  5=MOSI  6=MISO)",

	// 26 (RH850)
	"(1=VSS  2=VDD  3=RESET  4=FPCK  5=FPDR  6=FPDT  7=FLMD0",

	// 27 (MRK3)
	"(1=VSS  2=VDD  3=MSCL  4=MSDA)",

	// 28 (Elmos SBW)	
	"(1=VSS  2=VDD  3=TCK  4=TDA  5=TST)",

	// 29
	"no connections",

	// 30 (XC95xx JTAG)
	"(1=VSS  2=VDD  3=TMS  4=TCK  5=TDI  6=TDO)",

	// 31 (CC25xx)
	"(1=VSS  2=VDD  3=RST  4=DC  5=DD)",

	// 32 (PSOC4 SWD)
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",

	// 33 (STM32 F0xx SWD)
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",

	// 34 (STM32 F1xx SWD) 
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 35  (STM32 F2xx SWD)
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 36  (STM32 F3xx SWD)
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 37  (STM32 F4xx SWD)
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 38 
	"no connections",
	
	// 39 
	"no connections",
	
	// 40 
	"(1=VSS  2=VDD  3=RST  4=PDI)",
	
	// 41 (I2C)
	"(1=VSS  2=VDD  3=SCL  4=SDA)",

	// 42 (V850) 
	"(1=VSS  2=VDD  3=RESET  4=SCK  5=SI  6=SO  7=FLMD0)",
	
	// 43 (MLX90363) 
	"(1=VSS  2=VDD  3=SEL  4=SCK  5=MOSI  6=MISO)",
	
	// 44 (PPC JTAG) 
	"(1=VSS  2=VDD  3=TMS  4=TCK  5=TDI  6=TDO  [8=RESET])",
	
	// 45 (PPC JTAG2)
	"(1=VSS  2=VDD  3=TMS  4=TCK  5=TDI  6=TDO  7=JCOMP  [8=RESET])",
	
	// TLE986x
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 47 
	"(1=VSS  2=VDD  3=SEL  4=SCK  5=MOSI  6=MISO)",
	
	// 48 (SP40) 
	"(1=VSS  2=VDD  3=SCL  4=SDA)",

	// 49 (LPS25H) 
	"(1=VSS  2=VDD  3=SCL  4=SDA)",

	// 50 (MLX90316) 
	"(1=VSS  2=VDD  3=SEL  4=MOSI/MISO  5=SCK)",
	
	// 51 (CC2640) 
	"(1=VSS  2=VDD  3=TMS  4=TCK  5=TDI  6=TDO  [8=RESET])",
	
	// 52 (STM32L4xx)
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 53 (S32K) 
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 54 (PPC JTAG 3)
	"(1=VSS  2=VDD  3=TMS  4=TCK  5=TDI  6=TDO  7=JCOMP  [8=RESET])",
	
	// 55 PIC16
	"(1=VSS  2=VDD  3=PGD  4=PGC  9=MCLR)",
	
	// 56 S9KEA 
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 57 PIC16 
	"(1=VSS  2=VDD  3=PGD  4=PGC  9=MCLR)",
	
	// 58 RF430
	"(1=VSS  2=VDD  3=TEST  4=RESET  5=TEN  6=TCLK  7=TDAT)",

	// 59 SICI 
	"(1=VSS  2=VDD  3=IFB)",

	// 60 AVR0
	"(1=VSS  2=VDD  3=UPDI)",
	
	// 61 AT89S8252
	"(1=VSS  2=VDD  3=RST  4=SCK  5=MOSI  6=MISO)",
	
	// 62 (S12Z-BDM)
	"(1=VSS  2=VDD  3=RESET  4=BKGD)",
	
	// 63 (MPC57xx) 
	"(1=VSS  2=VDD  3=TMS  4=TCK  5=TDI  6=TDO  7=JCOMP  [8=RESET])",
	
	// 64 EFM32 
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 65 STM32F7 
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 66 DS28E07
	"(1=VSS  3=IO",
	
	// 67 AVR JTAG 
	"(1=VSS  2=VDD  3=TMS  4=TCK  5=TDI  6=TDO)",
	
	// 68 
	"no connections",

	// 69 VEML3328
	"(1=VSS  2=VDD  3=SCL  4=SDA)",

	// 70 PIC16
	"(1=VSS  2=VDD  3=PGD  4=PGC  9=MCLR)",
	
	// 71 MB91
	"(1=VSS  2=VDD  3=RST  4=TXD  5=RXD)",
	
	// 72 RA6 
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 73 AT24RF08
	"(1=VSS  2=VDD  3=SCL  4=SDA)",
	
	// 74 
	"no connections",
	
	// 75 S32K3
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 76 S32K14x 
	"(1=VSS  2=VDD  3=RST  4=SWDCK  5=SWDIO)",
	
	// 77 PIC18 
	"(1=VSS  2=VDD  3=PGD  4=PGC  9=MCLR)",
	
	// 78 
	"no connections",

	// 79 
	"no connections",

	// 80 
	"no connections",
	
	// 81 
	"no connections",

	// 82 
	"no connections",
	
	// 83 
	"no connections",
	
	// 84 
	"no connections",
	
	// 85 
	"no connections",
	
	// 86 
	"no connections",
	
	// 87 
	"no connections",
	
	// 88 
	"no connections",

	// 89 
	"no connections",

	// 90 
	"no connections",
	
	// 91 
	"no connections",

	// 92 
	"no connections",
	
	// 93 
	"no connections",
	
	// 94 
	"no connections",
	
	// 95 
	"no connections",
	
	// 96 
	"no connections",
	
	// 97 
	"no connections",
	
	// 98 
	"no connections",

	// 99 
	"no connections",

	// 100 
	"no connections",
	
	// 101 
	"no connections",

	// 102 
	"no connections",
	
	// 103 
	"no connections",
	
	// 104 
	"no connections",
	
	// 105 
	"no connections",
	
	// 106 
	"no connections",
	
	// 107 
	"no connections",
	
	// 108 
	"no connections",

	// 109 
	"no connections",

	// 110 
	"no connections",
	
	// 111 
	"no connections",

	// 112 
	"no connections",
	
	// 113 
	"no connections",
	
	// 114 
	"no connections",
	
	// 115 
	"no connections",
	
	// 116 
	"no connections",
	
	// 117 
	"no connections",
	
	// 118 
	"no connections",

	// 119 
	"no connections",

	// 120 
	"no connections",
	
	// 121 
	"no connections",

	// 122 
	"no connections",
	
	// 123 
	"no connections",
	
	// 124 
	"no connections",
	
	// 125 
	"no connections",
	
	// 126 
	"no connections",
	
	// 127 
	"no connections",
	
	// 128 
	"no connections",

	// 129 
	"no connections",

	// 130 
	"no connections",
	
	// 131 
	"no connections",

	// 132 
	"no connections",
	
	// 133 
	"no connections",
	
	// 134 
	"no connections",
	
	// 135 
	"no connections",
	
	// 136 
	"no connections",
	
	// 137 
	"no connections",
	
	// 138 
	"no connections",

	// 139 
	"no connections",

	// 140 
	"no connections",
	
	// 141 
	"no connections",

	// 142 
	"no connections",
	
	// 143 
	"no connections",
	
	// 144 
	"no connections",
	
	// 145 
	"no connections",
	
	// 146 
	"no connections",
	
	// 147 
	"no connections",
	
	// 148 
	"no connections",

	// 149 
	"no connections",

	// 150 
	"no connections",
	
	// 151 
	"no connections",

	// 152 
	"PARA-PROG, 16 Bits data bus, /RESET /CS /OE /WE /RESET-MCU",
	
	// 153 
	"no connections",
	
	// 154 
	"no connections",
	
	// 155 
	"no connections",
	
	// 156 
	"no connections",
	
	// 157 
	"no connections",
	
	// 158 
	"no connections",

	// 159 
	"no connections",

	// 160 
	"no connections",
	
	// 161 
	"no connections",

	// 162 
	"no connections",
	
	// 163 
	"no connections",
	
	// 164 
	"no connections",
	
	// 165 
	"no connections",
	
	// 166 
	"no connections",
	
	// 167 
	"no connections",
	
	// 168 
	"no connections",

	// 169 
	"no connections",

	// 170 
	"no connections",
	
	// 171 
	"no connections",

	// 172 
	"no connections",
	
	// 173 
	"no connections",
	
	// 174 
	"no connections",
	
	// 175 
	"no connections",
	
	// 176 
	"no connections",
	
	// 177 
	"no connections",
	
	// 178 
	"no connections",

	// 179 
	"no connections"

};	
