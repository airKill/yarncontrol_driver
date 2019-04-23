/*  BYTE Registers  */
sfr PSW    = 0xD0;
sfr ACC    = 0xE0;
sfr B      = 0xF0;
sfr SP     = 0x81;
sfr DPL0   = 0x82;
sfr DPL    = 0x82;
sfr DPH0   = 0x83;
sfr DPH    = 0x83;
sfr PCON   = 0x87;
sfr IE     = 0xA8;
sfr IP     = 0xB8;
sfr TCON   = 0x88;
/*  DS80C320 Extensions  */
sfr DPL1   = 0x84;
sfr DPH1   = 0x85;
sfr DPS    = 0x86;
sfr EXIF   = 0x91;

/* sfr WDCON  = 0xD8; Not part of DW8051 architecture */
sfr EIE    = 0xE8;
sfr EIP    = 0xF8;

/* DW8051 EXTENSIONS */
sfr MPAGE  = 0x92;
sfr SPC_FNC= 0x8F;
sfr EICON  = 0xD8;

/*  BIT Registers  */
/*  PSW  */
sbit CY    = 0xD7;
sbit AC    = 0xD6;
sbit F0    = 0xD5;
sbit RS1   = 0xD4;
sbit RS0   = 0xD3;
sbit OV    = 0xD2;
sbit FL    = 0xD1;
sbit P     = 0xD0;

/*  TCON  */
sbit TF1   = 0x8F;
sbit TR1   = 0x8E;
sbit TF0   = 0x8D;
sbit TR0   = 0x8C;
sbit IE1   = 0x8B;
sbit IT1   = 0x8A;
sbit IE0   = 0x89;
sbit IT0   = 0x88;

/*  IE  */
sbit EA    = 0xAF;
sbit ES1   = 0xAE;
sbit ET2   = 0xAD;
sbit ES0   = 0xAC;
sbit ET1   = 0xAB;
sbit EX1   = 0xAA;
sbit ET0   = 0xA9;
sbit EX0   = 0xA8;

/*  IP  */
sbit PS1   = 0xBE;
sbit PT2   = 0xBD;
sbit PS0   = 0xBC;
sbit PT1   = 0xBB;
sbit PX1   = 0xBA;
sbit PT0   = 0xB9;
sbit PX0   = 0xB8;



