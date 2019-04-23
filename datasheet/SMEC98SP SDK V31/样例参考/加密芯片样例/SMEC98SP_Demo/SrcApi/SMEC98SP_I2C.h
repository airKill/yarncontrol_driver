// ---------------------------------------------------------
// MMU registers definition
// ---------------------------------------------------------

sfr NVMCLR		= 0xB1; 	//NVM code limit register
sfr CBS			= 0x9A;
sfr SYSPROT		= 0x9B;
sfr DBS16H		= 0x9C;
sfr DBS32K		= 0x9D;
sfr DBS16L		= 0x9E;

// ---------------------------------------------------------
// service registers definition
// ---------------------------------------------------------
sfr TRAP  		= 0xB0;		//TRAP status register		
sfr SRVCON 		= 0x80;		//services control register  	
sfr TRNG  		= 0xB5;		//TRNG word 			
sfr TRIMOSC 	= 0xB9;		//TRIM oscillator register  	
sfr TRIMVCC 	= 0xBA;		//TRIM regulator register 
sfr TRIMTMP		= 0xA3;		//Bandgap correction temperature
sfr TRIMFBIAS	= 0xA2;
sfr TRIMCKCP	= 0xA5;
sfr RDMON		= 0xA1; 	

// ---------------------------------------------------------
// ISO7816-3 serial interface sfr registers definition
// ---------------------------------------------------------
sfr ISOCON  	= 0xB2;		//ISO control/status register
sfr ISOTMR		= 0xB4;		//ISO timer	
sfr SBUF		= 0x99;		//ISO serial RX/TX buffer		
sfr SCON		= 0x98;		//ISO control/status register
sfr ETUCON		= 0x93;		//etu_counter control/status register
sfr ETUCOUNT	= 0x94;		//etu_counter value

sbit ISO_SB_INT_EN	= 0x9E;

/*  SCON0  */
//sbit SM0   		= 0x9F;
//sbit SM1   		= 0x9E;
//sbit SM2   		= 0x9D;
//sbit REN   		= 0x9C;
//sbit TB8   		= 0x9B;
//sbit RB8   		= 0x9A;
sbit TI    		= 0x99;
sbit RI   		= 0x98;
sbit ISO_BUSY	= 0x9F;

sfr I2CCON		= 0xF8;
sfr I2CCON2		= 0xEB;
sfr I2CCON3		= 0xEC;
sfr I2CADR		= 0xFA;
sfr I2CXAD		= 0xFB;
sfr I2CBUF		= 0xFC;
sfr I2CSTAT		= 0xFD;
sfr I2CPAD		= 0xF9;
sfr GPIODATA	= 0xE8;
sfr GPIODIR		= 0xD8;
sfr GPIOPUP		= 0xE9;
//---------------------------------------------------------
//CRC CCITT V41 accelerator registers definition
//---------------------------------------------------------
sfr CRCINIH		= 0x95;  	//Initial value MSB
sfr CRCINIL		= 0x96;  	//Initial value LSb
sfr CRCIO 		= 0x97;  	//Data in/out register 

// ---------------------------------------------------------
// Test controller sfr registers definition
// ---------------------------------------------------------
sfr TESTREG0	= 0xBB;		//TEST status register		
sfr TESTREG1	= 0xBC;		//TEST control register	
sfr EXPCON 		= 0xB6;		//Authentication register
sfr IDCNT 		= 0xB7;		//Authentication counter register

// ---------------------------------------------------------
// Non Volatile memories sfr registers definition
// ---------------------------------------------------------
sfr GPNVM 		= 0x90;		//NVM memories control register
sfr	NVMCTRL		= 0xA0;		//NVM memories control register 2
sfr NVMTEST		= 0xBD;		//
sfr NVMIREF		= 0xBE;		//
sfr MASSRD		= 0xBF;		//Mass read register	
sfr CPHV		= 0xC6;		//Carge pump HV register
sfr CPTRIM		= 0xC7;	 	//New Charge pump HV register to have fine trimming in erase operation
sfr TIMECON 	= 0xC4;
sfr SMARTTIME	= 0xC3;
sfr SMARTREAD	= 0xC5;
sfr	FLASHTEST	= 0xC6;
sfr SPAREREG	= 0xC7;
//SRVCON bit definition
sbit TRNG_EN	= 0x87;
sbit VMON_EN	= 0x86;

//---------------------------------------------------------
// MEMORY CONFIGURATION 
//---------------------------------------------------------
#define START_RAM 				0x0000
#define END_RAM					0x0700

#define START_GPNVM_CODE		0x0000
#define END_OTP_CODE			0x0FFF
#define START_GPNVMSYS_CODE		0x8000
#define END_GPNVMSYS_CODE		0x8800

#define DEVICEID_ADDR			0x8C22

//---------------------------------------------------------
// Default values for ISO7816 SFR registers
//---------------------------------------------------------
#define D_SBUF					0x00
#define D_SCON					0x00
#define D_ETUCON				0x00
#define D_ETUCOUNT				0x00
#define D_ISOCON				0x04
#define D_ISOTMRH				0x01
#define D_ISOTMRL				0x73
#define D_EXPCON				0x00
#define D_IDCNT					0x00
#define D_TESTREG0				0x00
#define D_TESTREG1				0x00

//---------------------------------------------------------
// Default values for MMU SFR registers
//---------------------------------------------------------
#define D_NVMCLR				0x00
#define D_TRAP					0x00
#define D_CBS					0x03
#define D_SYSPROT				0x0F
#define D_DBS16H				0x08
#define D_DBS16L				0x07
#define D_DBS32K				0x02
#define D_MEMLIMIT				0x28

//---------------------------------------------------------
// Default values for Control Logic SFR registers
//---------------------------------------------------------
#define D_TIMECON				0x00
#define D_CPHV					0x00
#define D_CPTRIM				0x00
#define D_GPNVM					0x00
#define D_NVMTEST				0x04	//force NVMTEST.2 = 1
#define D_NVMIREF				0x00
#define D_MASSRD				0x08

//---------------------------------------------------------
// Default values for SERV function registers
//---------------------------------------------------------
#define D_TRIMOSC				0xFF
#define D_TRIMVCC				0x20
#define D_SRVCON				0x00

//---------------------------------------------------------
// Interrupt Flags
//---------------------------------------------------------
#define DEFAULT_FLAG			0xED
#define COB_FLAG				0x1E
#define DOB_FLAG				0x0F
#define ETUCOUNT_FLAG			0x05
#define UVD_FLAG				0x4B
#define OVD_FLAG				0x5A
#define ALL_FLAG				0x6B
#define GPNVM_FLAG				0xE4
#define IDLE_FLAG				0xD5
#define VPP_FLAG				0xC3
#define RI_FLAG					0x02
#define TI_FLAG					0x03
#define START_BIT_FLAG			0x02