; ---------------------------------------------------------
; MMU registers definition
; ---------------------------------------------------------

NVMCLR		DATA 0B1H;	NVM code limit register
CBS			DATA 09AH;
SYSPROT		DATA 09BH;
DBS16H		DATA 09CH;
DBS32K		DATA 09DH;
DBS16L		DATA 09EH;

; ---------------------------------------------------------
; service registers definition
; ---------------------------------------------------------
TRAP  		DATA 0B0H;	 TRAP status register		
SRVCON 		DATA 080H;	 services control register  	
TRNG  		DATA 0B5H;	 TRNG word 			
TRIMBG		DATA 0A4H;   Bandgap control register
TRIMOSC 	DATA 0B9H;	 TRIM oscillator register  	
TRIMVCC 	DATA 0BAH;	 TRIM regulator register 
TRIMTMP		DATA 0A3H;	 Bandgap correction temperature
TRIMFBIAS	DATA 0A2H;
TRIMCKCP	DATA 0A5H;
RDMON		DATA 0A1H;

; ---------------------------------------------------------
; ISO7816-3 serial interface sfr registers definition
; ---------------------------------------------------------
ISOCON  	DATA 0B2H;	ISO control/status register
ISOTMR		DATA 0B4H;	ISO timer
ETUCON		DATA 093H;	etu_counter control/status register
ETUCOUNT	DATA 094H;	etu_counter value
;SCON		DATA 098H

; ---------------------------------------------------------
; I2C interface
; ---------------------------------------------------------
I2CCON		DATA 0xF8;
I2CCON2		DATA 0xEB;
I2CCON3		DATA 0xEC;
I2CADR		DATA 0xFA;
I2CXAD		DATA 0xFB;
I2CBUF		DATA 0xFC;
I2CSTAT		DATA 0xFD;
I2CPAD		DATA 0xF9;
; ---------------------------------------------------------
; GPIO interface
; ---------------------------------------------------------
GPIODATA	DATA 0xE8;
GPIODIR		DATA 0xD8;
GPIOPUP		DATA 0xE9;
; ---------------------------------------------------------
; CRC CCITT V41 accelerator registers definition
; ---------------------------------------------------------
CRCINIH		DATA 095h	;  Initial value MSB
CRCINIL		DATA 096h  	;  Initial value MSB
CRCIO 		DATA 097h  	;  Data in/out register 

; ---------------------------------------------------------
; Test controller sfr registers definition
; ---------------------------------------------------------
TESTREG0 	DATA 0BBH;	 TEST status register		
TESTREG1 	DATA 0BCH;	 TEST control register	
EXPCON		DATA 0B6H;
IDCNT 		DATA 0B7H;	 Authentication counter register

; ---------------------------------------------------------
; Non Volatile memories sfr registers definition
; ---------------------------------------------------------
GPNVM 		DATA 090H;	 OTP memories control register
NVMCTRL		DATA 0A0H;	 NVM memories control register 2
NVMTEST		DATA 0BDH;
;NVMIREF		DATA 0C2H;
MASSRD		DATA 0BFH;	 Mass read register	
CPHV		DATA 0C6H;	 Charge pump HV register
CPTRIM		DATA 0C7h;	 New Charge pump HV register to have fine trimming in erase operation
TIMECON		DATA 0C4H; 
SMARTTIME	DATA 0C3H
SMARTRD	 	DATA 0C5H
FLASHTEST	DATA 0C6H
SPAREREG	DATA 0C7H

; ---------------------------------------------------------
; Default values for ISO7816 SFR registers
; ---------------------------------------------------------
D_SBUF		EQU	00H
D_SCON		EQU	00H
D_ETUCON	EQU	00H
D_ETUCOUNT	EQU	00H
D_ISOCON	EQU	04H
D_ISOTMRH	EQU	01H
D_ISOTMRL	EQU	73H
D_EXPCON	EQU	00H
D_IDCNT		EQU	00H
D_TESTREG0	EQU	00H
D_TESTREG1	EQU	00H

; ---------------------------------------------------------
; Default values for MMU SFR registers
; ---------------------------------------------------------
D_NVMCLR	EQU	00H
D_TRAP		EQU	00H
D_CBS		EQU	03H
D_SYSPROT	EQU	0FH
D_DBS16H	EQU	08H
D_DBS16L	EQU	07H
D_DBS32K	EQU	02H
D_MEMLIMIT	EQU	028H

; ---------------------------------------------------------
; Default values for Control Logic SFR registers
; ---------------------------------------------------------
D_TIMECON	EQU	00H
D_CPHV		EQU	00H
D_CPTRIM	EQU	00H
D_GPNVM		EQU	00H
D_NVMTEST	EQU	04H	;force NVMTEST.2 = 1
D_NVMIREF	EQU	00H
D_MASSRD	EQU	08H

; ---------------------------------------------------------
; Default values for SERV function registers
; ---------------------------------------------------------
D_TRIMOSC	EQU	0FFH
D_TRIMVCC	EQU	20H
D_SRVCON	EQU	00H

; ---------------------------------------------------------
; Interrupt flag
; ---------------------------------------------------------
DEFAULT_FLAG	EQU	0EDH
COB_FLAG		EQU	01EH
DOB_FLAG		EQU	00FH
ETUCOUNT_FLAG	EQU	005H
UVD_FLAG		EQU	04BH
OVD_FLAG		EQU	05AH
ALL_FLAG		EQU	06BH
GPNVM_FLAG		EQU	0E4H
IDLE_FLAG		EQU	0D5H
VPP_FLAG		EQU	0C3H
RI_FLAG			EQU	002H
TI_FLAG			EQU	003H
START_BIT_FLAG	EQU	002H

;----------------------------------------------------------------------
; General Constant definitions for NVM Manager & Bootrom
;----------------------------------------------------------------------
F1									EQU PSW.1
ROM_REVISION_ID						EQU	050h
GPNVM_REWORK_CODE_ADDRESS			EQU 0375H		; MLT change because of size of OtpMassEraseFromRomCode function
ROM_CODE_END_ADRESSS				EQU 0400H
PARAM_START_ADDRESS 	  			EQU 8000H
PARAM_SIZE							EQU 0040H
DBS32K_SYSTEM 						EQU 001FH
CBS_SYSTEM 							EQU 001FH		; HIDE ROM (ROM CODE IN GPNVM FOR TEST) and select code bank 4
RAM_BUFFER_START_PARAM_ADDRESS		EQU 0000H
RAM_BUFFER_START_NVMM_ADDRESS		EQU RAM_BUFFER_START_PARAM_ADDRESS + PARAM_SIZE
GPNVM_USER_START_ADDRESS			EQU 0000H
GPNVM_USER_START_AT_0X400			EQU 0400H
GPNVM_FLASH_REGISTER_START_ADDRESS	EQU 8100H
HIDE_ROM_SIG_MSB_CONTENT			EQU	02AH
HIDE_ROM_SIG_LSB_CONTENT			EQU	0C8H
TEST_SIG_MSB_CONTENT				EQU	00CH
TEST_SIG_LSB_CONTENT				EQU	0BDH
NVMM_SIG_MSB_CONTENT				EQU	025H
NVMM_SIG_LSB_CONTENT				EQU	0D4H
DISTEST_SIG_MSB_CONTENT				EQU	0D6H
DISTEST_SIG_LSB_CONTENT				EQU	029H
CNT_SIG_MSB_CONTENT					EQU	043H
CNT_SIG_LSB_CONTENT					EQU	0BCH
SKIP_EM_NVM_MANAGER_VAL				EQU 0A5H
SRVCON_INIT_VAL						EQU 044H
TRIMOSC_INIT_VAL					EQU 060H
TRIMFBIAS_INIT_VAL					EQU 008H
NUMBER_OF_PARAM_TO_CHECK			EQU 00BH
GPNVM_USER_START					EQU 0000H

