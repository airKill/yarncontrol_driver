;/*  BYTE Registers  */

;PSW    DATA  0D0H
;ACC    DATA  0E0H
;B      DATA  0F0H
;SP     DATA  081H
DPL0   DATA  082H
DPH0   DATA  083H
;PCON   DATA  087H
;TCON   DATA  088H
;TMOD   DATA  089H
;TL0    DATA  08AH
;TL1    DATA  08BH
;TH0    DATA  08CH
;TH1    DATA  08DH
;IE     DATA  0A8H
;IP     DATA  0B8H

;/*  DS80C320 Extensions  */
DPL1   DATA  084H
DPH1   DATA  085H
DPS    DATA  086H
CKCON  DATA  08EH

;/* DW8051 EXTENSIONS */
MPAGE  	DATA  092H
;SPC_FNC	DATA  08FH
;EICON  	DATA  0D8H

;/*  BIT Registers  */
;/*  PSW  */
;CY    BIT  0D7H
;AC    BIT  0D6H
;F0    BIT  0D5H
;RS1   BIT  0D4H
;RS0   BIT  0D3H
;OV    BIT  0D2H
;FL    BIT  0D1H
;P     BIT  0D0H

;/*  TCON  */
;TF1   BIT  08FH
;TR1   BIT  08EH
;TF0   BIT  08DH
;TR0   BIT  08CH
;IE1   BIT  08BH
;IT1   BIT  08AH
;IE0   BIT  089H
;IT0   BIT  088H

;/*  IE  */
;EA    BIT  0AFH
;ES1   BIT  0AEH
;ET2   BIT  0ADH
;ES0   BIT  0ACH
;ET1   BIT  0ABH
;EX1   BIT  0AAH
;ET0   BIT  0A9H
;EX0   BIT  0A8H

;/*  IP  */
;PS1   BIT  0BEH
;PT2   BIT  0BDH
;PS0   BIT  0BCH
;PT1   BIT  0BBH
;PX1   BIT  0BAH
;PT0   BIT  0B9H
;PX0   BIT  0B8H

;/*  SCON0  */
;SM0   BIT  09FH
;SM1   BIT  09EH
;SM2   BIT  09DH
;REN   BIT  09CH
;TB8   BIT  09BH
;RB8   BIT  09AH
;TI    BIT  099H
;RI    BIT  098H





