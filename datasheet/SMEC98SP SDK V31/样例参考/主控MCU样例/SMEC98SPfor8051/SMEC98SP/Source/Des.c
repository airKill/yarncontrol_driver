
unsigned char bdata DesChangeAreaOut[8];
xdata unsigned char DesChangeAreaIn[8];

#define DesInBit00 DesChangeAreaIn[0]&0x80;
#define DesInBit01 DesChangeAreaIn[0]&0x40;
#define DesInBit02 DesChangeAreaIn[0]&0x20;
#define DesInBit03 DesChangeAreaIn[0]&0x10;
#define DesInBit04 DesChangeAreaIn[0]&0x08;
#define DesInBit05 DesChangeAreaIn[0]&0x04;
#define DesInBit06 DesChangeAreaIn[0]&0x02;
#define DesInBit07 DesChangeAreaIn[0]&0x01;

#define DesInBit10 DesChangeAreaIn[1]&0x80;
#define DesInBit11 DesChangeAreaIn[1]&0x40;
#define DesInBit12 DesChangeAreaIn[1]&0x20;
#define DesInBit13 DesChangeAreaIn[1]&0x10;
#define DesInBit14 DesChangeAreaIn[1]&0x08;
#define DesInBit15 DesChangeAreaIn[1]&0x04;
#define DesInBit16 DesChangeAreaIn[1]&0x02;
#define DesInBit17 DesChangeAreaIn[1]&0x01;

#define DesInBit20 DesChangeAreaIn[2]&0x80;
#define DesInBit21 DesChangeAreaIn[2]&0x40;
#define DesInBit22 DesChangeAreaIn[2]&0x20;
#define DesInBit23 DesChangeAreaIn[2]&0x10;
#define DesInBit24 DesChangeAreaIn[2]&0x08;
#define DesInBit25 DesChangeAreaIn[2]&0x04;
#define DesInBit26 DesChangeAreaIn[2]&0x02;
#define DesInBit27 DesChangeAreaIn[2]&0x01;

#define DesInBit30 DesChangeAreaIn[3]&0x80;
#define DesInBit31 DesChangeAreaIn[3]&0x40;
#define DesInBit32 DesChangeAreaIn[3]&0x20;
#define DesInBit33 DesChangeAreaIn[3]&0x10;
#define DesInBit34 DesChangeAreaIn[3]&0x08;
#define DesInBit35 DesChangeAreaIn[3]&0x04;
#define DesInBit36 DesChangeAreaIn[3]&0x02;
#define DesInBit37 DesChangeAreaIn[3]&0x01;

#define DesInBit40 DesChangeAreaIn[4]&0x80;
#define DesInBit41 DesChangeAreaIn[4]&0x40;
#define DesInBit42 DesChangeAreaIn[4]&0x20;
#define DesInBit43 DesChangeAreaIn[4]&0x10;
#define DesInBit44 DesChangeAreaIn[4]&0x08;
#define DesInBit45 DesChangeAreaIn[4]&0x04;
#define DesInBit46 DesChangeAreaIn[4]&0x02;
#define DesInBit47 DesChangeAreaIn[4]&0x01;

#define DesInBit50 DesChangeAreaIn[5]&0x80;
#define DesInBit51 DesChangeAreaIn[5]&0x40;
#define DesInBit52 DesChangeAreaIn[5]&0x20;
#define DesInBit53 DesChangeAreaIn[5]&0x10;
#define DesInBit54 DesChangeAreaIn[5]&0x08;
#define DesInBit55 DesChangeAreaIn[5]&0x04;
#define DesInBit56 DesChangeAreaIn[5]&0x02;
#define DesInBit57 DesChangeAreaIn[5]&0x01;

#define DesInBit60 DesChangeAreaIn[6]&0x80;
#define DesInBit61 DesChangeAreaIn[6]&0x40;
#define DesInBit62 DesChangeAreaIn[6]&0x20;
#define DesInBit63 DesChangeAreaIn[6]&0x10;
#define DesInBit64 DesChangeAreaIn[6]&0x08;
#define DesInBit65 DesChangeAreaIn[6]&0x04;
#define DesInBit66 DesChangeAreaIn[6]&0x02;
#define DesInBit67 DesChangeAreaIn[6]&0x01;

#define DesInBit70 DesChangeAreaIn[7]&0x80;
#define DesInBit71 DesChangeAreaIn[7]&0x40;
#define DesInBit72 DesChangeAreaIn[7]&0x20;
#define DesInBit73 DesChangeAreaIn[7]&0x10;
#define DesInBit74 DesChangeAreaIn[7]&0x08;
#define DesInBit75 DesChangeAreaIn[7]&0x04;
#define DesInBit76 DesChangeAreaIn[7]&0x02;
#define DesInBit77 DesChangeAreaIn[7]&0x01;


///////////////////
sbit DesOutBit00=DesChangeAreaOut[0]^7;sbit DesOutBit01=DesChangeAreaOut[0]^6;sbit DesOutBit02=DesChangeAreaOut[0]^5;sbit DesOutBit03=DesChangeAreaOut[0]^4;
sbit DesOutBit04=DesChangeAreaOut[0]^3;sbit DesOutBit05=DesChangeAreaOut[0]^2;sbit DesOutBit06=DesChangeAreaOut[0]^1;sbit DesOutBit07=DesChangeAreaOut[0]^0;

sbit DesOutBit10=DesChangeAreaOut[1]^7;sbit DesOutBit11=DesChangeAreaOut[1]^6;sbit DesOutBit12=DesChangeAreaOut[1]^5;sbit DesOutBit13=DesChangeAreaOut[1]^4;
sbit DesOutBit14=DesChangeAreaOut[1]^3;sbit DesOutBit15=DesChangeAreaOut[1]^2;sbit DesOutBit16=DesChangeAreaOut[1]^1;sbit DesOutBit17=DesChangeAreaOut[1]^0;

sbit DesOutBit20=DesChangeAreaOut[2]^7;sbit DesOutBit21=DesChangeAreaOut[2]^6;sbit DesOutBit22=DesChangeAreaOut[2]^5;sbit DesOutBit23=DesChangeAreaOut[2]^4;
sbit DesOutBit24=DesChangeAreaOut[2]^3;sbit DesOutBit25=DesChangeAreaOut[2]^2;sbit DesOutBit26=DesChangeAreaOut[2]^1;sbit DesOutBit27=DesChangeAreaOut[2]^0;

sbit DesOutBit30=DesChangeAreaOut[3]^7;sbit DesOutBit31=DesChangeAreaOut[3]^6;sbit DesOutBit32=DesChangeAreaOut[3]^5;sbit DesOutBit33=DesChangeAreaOut[3]^4;
sbit DesOutBit34=DesChangeAreaOut[3]^3;sbit DesOutBit35=DesChangeAreaOut[3]^2;sbit DesOutBit36=DesChangeAreaOut[3]^1;sbit DesOutBit37=DesChangeAreaOut[3]^0;

sbit DesOutBit40=DesChangeAreaOut[4]^7;sbit DesOutBit41=DesChangeAreaOut[4]^6;sbit DesOutBit42=DesChangeAreaOut[4]^5;sbit DesOutBit43=DesChangeAreaOut[4]^4;
sbit DesOutBit44=DesChangeAreaOut[4]^3;sbit DesOutBit45=DesChangeAreaOut[4]^2;sbit DesOutBit46=DesChangeAreaOut[4]^1;sbit DesOutBit47=DesChangeAreaOut[4]^0;

sbit DesOutBit50=DesChangeAreaOut[5]^7;sbit DesOutBit51=DesChangeAreaOut[5]^6;sbit DesOutBit52=DesChangeAreaOut[5]^5;sbit DesOutBit53=DesChangeAreaOut[5]^4;
sbit DesOutBit54=DesChangeAreaOut[5]^3;sbit DesOutBit55=DesChangeAreaOut[5]^2;sbit DesOutBit56=DesChangeAreaOut[5]^1;sbit DesOutBit57=DesChangeAreaOut[5]^0;

sbit DesOutBit60=DesChangeAreaOut[6]^7;sbit DesOutBit61=DesChangeAreaOut[6]^6;sbit DesOutBit62=DesChangeAreaOut[6]^5;sbit DesOutBit63=DesChangeAreaOut[6]^4;
sbit DesOutBit64=DesChangeAreaOut[6]^3;sbit DesOutBit65=DesChangeAreaOut[6]^2;sbit DesOutBit66=DesChangeAreaOut[6]^1;sbit DesOutBit67=DesChangeAreaOut[6]^0;

sbit DesOutBit70=DesChangeAreaOut[7]^7;sbit DesOutBit71=DesChangeAreaOut[7]^6;sbit DesOutBit72=DesChangeAreaOut[7]^5;sbit DesOutBit73=DesChangeAreaOut[7]^4;
sbit DesOutBit74=DesChangeAreaOut[7]^3;sbit DesOutBit75=DesChangeAreaOut[7]^2;sbit DesOutBit76=DesChangeAreaOut[7]^1;sbit DesOutBit77=DesChangeAreaOut[7]^0;

/************************************************************/
unsigned char code DesKeyMoveBit[16]={1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

unsigned char code DesReplaceT1[4][16]={
			14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
          	0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
          	4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
          	15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13};

unsigned char code DesReplaceT2[4][16]={
			15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
          	3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
          	0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
          	13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9};

unsigned char code DesReplaceT3[4][16]={
			10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
          	13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
          	13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
          	1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12};

unsigned char code DesReplaceT4[4][16]={
			7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
          	13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
          	10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
          	3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14};

unsigned char code DesReplaceT5[4][16]={
			2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
          	14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
          	4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
          	11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3};

unsigned char code DesReplaceT6[4][16]={
			12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
          	10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
          	9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
          	4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13};

unsigned char code DesReplaceT7[4][16]={
			4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
          	13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
          	1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
          	6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12};

unsigned char code DesReplaceT8[4][16]={
			13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
          	1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
          	7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
          	2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11};

/************************************************************/

#define DesKey64To56() {\
	 DesOutBit04=DesInBit70;DesOutBit05=DesInBit60;\
	 DesOutBit06=DesInBit50;DesOutBit07=DesInBit40;\
	 DesOutBit10=DesInBit30;DesOutBit11=DesInBit20;\
	 DesOutBit12=DesInBit10;DesOutBit13=DesInBit00;\
	 DesOutBit14=DesInBit71;DesOutBit15=DesInBit61;\
	 DesOutBit16=DesInBit51;DesOutBit17=DesInBit41;\
	 DesOutBit20=DesInBit31;DesOutBit21=DesInBit21;\
	 DesOutBit22=DesInBit11;DesOutBit23=DesInBit01;\
	 DesOutBit24=DesInBit72;DesOutBit25=DesInBit62;\
	 DesOutBit26=DesInBit52;DesOutBit27=DesInBit42;\
	 DesOutBit30=DesInBit32;DesOutBit31=DesInBit22;\
	 DesOutBit32=DesInBit12;DesOutBit33=DesInBit02;\
	 DesOutBit34=DesInBit73;DesOutBit35=DesInBit63;\
	 DesOutBit36=DesInBit53;DesOutBit37=DesInBit43;\
	\
	 DesOutBit44=DesInBit76;DesOutBit45=DesInBit66;\
	 DesOutBit46=DesInBit56;DesOutBit47=DesInBit46;\
	 DesOutBit50=DesInBit36;DesOutBit51=DesInBit26;\
	 DesOutBit52=DesInBit16;DesOutBit53=DesInBit06;\
	 DesOutBit54=DesInBit75;DesOutBit55=DesInBit65;\
	 DesOutBit56=DesInBit55;DesOutBit57=DesInBit45;\
	 DesOutBit60=DesInBit35;DesOutBit61=DesInBit25;\
	 DesOutBit62=DesInBit15;DesOutBit63=DesInBit05;\
	 DesOutBit64=DesInBit74;DesOutBit65=DesInBit64;\
	 DesOutBit66=DesInBit54;DesOutBit67=DesInBit44;\
	 DesOutBit70=DesInBit34;DesOutBit71=DesInBit24;\
	 DesOutBit72=DesInBit14;DesOutBit73=DesInBit04;\
	 DesOutBit74=DesInBit33;DesOutBit75=DesInBit23;\
	 DesOutBit76=DesInBit13;DesOutBit77=DesInBit03;\
}

#define DesKey56To48() {\
	 DesOutBit00=DesInBit21;DesOutBit01=DesInBit24;\
	 DesOutBit02=DesInBit16;DesOutBit03=DesInBit33;\
	 DesOutBit04=DesInBit04;DesOutBit05=DesInBit10;\
	 DesOutBit06=DesInBit06;DesOutBit07=DesInBit37;\
	 DesOutBit10=DesInBit22;DesOutBit11=DesInBit11;\
	 DesOutBit12=DesInBit30;DesOutBit13=DesInBit15;\
	 DesOutBit14=DesInBit32;DesOutBit15=DesInBit26;\
	 DesOutBit16=DesInBit17;DesOutBit17=DesInBit07;\
	 DesOutBit20=DesInBit35;DesOutBit21=DesInBit13;\
	 DesOutBit22=DesInBit23;DesOutBit23=DesInBit12;\
	 DesOutBit24=DesInBit36;DesOutBit25=DesInBit27;\
	 DesOutBit26=DesInBit20;DesOutBit27=DesInBit05;\
	\
	 DesOutBit30=DesInBit60;DesOutBit31=DesInBit73;\
	 DesOutBit32=DesInBit46;DesOutBit33=DesInBit54;\
	 DesOutBit34=DesInBit66;DesOutBit35=DesInBit76;\
	 DesOutBit36=DesInBit45;DesOutBit37=DesInBit57;\
	 DesOutBit40=DesInBit72;DesOutBit41=DesInBit64;\
	 DesOutBit42=DesInBit50;DesOutBit43=DesInBit67;\
	 DesOutBit44=DesInBit63;DesOutBit45=DesInBit70;\
	 DesOutBit46=DesInBit56;DesOutBit47=DesInBit77;\
	 DesOutBit50=DesInBit51;DesOutBit51=DesInBit74;\
	 DesOutBit52=DesInBit65;DesOutBit53=DesInBit61;\
	 DesOutBit54=DesInBit71;DesOutBit55=DesInBit53;\
	 DesOutBit56=DesInBit44;DesOutBit57=DesInBit47;\
}

#define DesDat64IP() {\
	 DesOutBit00=DesInBit71;DesOutBit01=DesInBit61;\
	 DesOutBit02=DesInBit51;DesOutBit03=DesInBit41;\
	 DesOutBit04=DesInBit31;DesOutBit05=DesInBit21;\
	 DesOutBit06=DesInBit11;DesOutBit07=DesInBit01;\
	 DesOutBit10=DesInBit73;DesOutBit11=DesInBit63;\
	 DesOutBit12=DesInBit53;DesOutBit13=DesInBit43;\
	 DesOutBit14=DesInBit33;DesOutBit15=DesInBit23;\
	 DesOutBit16=DesInBit13;DesOutBit17=DesInBit03;\
	 DesOutBit20=DesInBit75;DesOutBit21=DesInBit65;\
	 DesOutBit22=DesInBit55;DesOutBit23=DesInBit45;\
	 DesOutBit24=DesInBit35;DesOutBit25=DesInBit25;\
	 DesOutBit26=DesInBit15;DesOutBit27=DesInBit05;\
	 DesOutBit30=DesInBit77;DesOutBit31=DesInBit67;\
	 DesOutBit32=DesInBit57;DesOutBit33=DesInBit47;\
	 DesOutBit34=DesInBit37;DesOutBit35=DesInBit27;\
	 DesOutBit36=DesInBit17;DesOutBit37=DesInBit07;\
	\
	 DesOutBit40=DesInBit70;DesOutBit41=DesInBit60;\
	 DesOutBit42=DesInBit50;DesOutBit43=DesInBit40;\
	 DesOutBit44=DesInBit30;DesOutBit45=DesInBit20;\
	 DesOutBit46=DesInBit10;DesOutBit47=DesInBit00;\
	 DesOutBit50=DesInBit72;DesOutBit51=DesInBit62;\
	 DesOutBit52=DesInBit52;DesOutBit53=DesInBit42;\
	 DesOutBit54=DesInBit32;DesOutBit55=DesInBit22;\
	 DesOutBit56=DesInBit12;DesOutBit57=DesInBit02;\
	 DesOutBit60=DesInBit74;DesOutBit61=DesInBit64;\
	 DesOutBit62=DesInBit54;DesOutBit63=DesInBit44;\
	 DesOutBit64=DesInBit34;DesOutBit65=DesInBit24;\
	 DesOutBit66=DesInBit14;DesOutBit67=DesInBit04;\
	 DesOutBit70=DesInBit76;DesOutBit71=DesInBit66;\
	 DesOutBit72=DesInBit56;DesOutBit73=DesInBit46;\
	 DesOutBit74=DesInBit36;DesOutBit75=DesInBit26;\
	 DesOutBit76=DesInBit16;DesOutBit77=DesInBit06;\
}

#define DesDat64IP_R() {\
	 DesOutBit00=DesInBit47;DesOutBit01=DesInBit07;\
	 DesOutBit02=DesInBit57;DesOutBit03=DesInBit17;\
	 DesOutBit04=DesInBit67;DesOutBit05=DesInBit27;\
	 DesOutBit06=DesInBit77;DesOutBit07=DesInBit37;\
	 DesOutBit10=DesInBit46;DesOutBit11=DesInBit06;\
	 DesOutBit12=DesInBit56;DesOutBit13=DesInBit16;\
	 DesOutBit14=DesInBit66;DesOutBit15=DesInBit26;\
	 DesOutBit16=DesInBit76;DesOutBit17=DesInBit36;\
	 DesOutBit20=DesInBit45;DesOutBit21=DesInBit05;\
	 DesOutBit22=DesInBit55;DesOutBit23=DesInBit15;\
	 DesOutBit24=DesInBit65;DesOutBit25=DesInBit25;\
	 DesOutBit26=DesInBit75;DesOutBit27=DesInBit35;\
	 DesOutBit30=DesInBit44;DesOutBit31=DesInBit04;\
	 DesOutBit32=DesInBit54;DesOutBit33=DesInBit14;\
	 DesOutBit34=DesInBit64;DesOutBit35=DesInBit24;\
	 DesOutBit36=DesInBit74;DesOutBit37=DesInBit34;\
	\
	 DesOutBit40=DesInBit43;DesOutBit41=DesInBit03;\
	 DesOutBit42=DesInBit53;DesOutBit43=DesInBit13;\
	 DesOutBit44=DesInBit63;DesOutBit45=DesInBit23;\
	 DesOutBit46=DesInBit73;DesOutBit47=DesInBit33;\
	 DesOutBit50=DesInBit42;DesOutBit51=DesInBit02;\
	 DesOutBit52=DesInBit52;DesOutBit53=DesInBit12;\
	 DesOutBit54=DesInBit62;DesOutBit55=DesInBit22;\
	 DesOutBit56=DesInBit72;DesOutBit57=DesInBit32;\
	 DesOutBit60=DesInBit41;DesOutBit61=DesInBit01;\
	 DesOutBit62=DesInBit51;DesOutBit63=DesInBit11;\
	 DesOutBit64=DesInBit61;DesOutBit65=DesInBit21;\
	 DesOutBit66=DesInBit71;DesOutBit67=DesInBit31;\
	 DesOutBit70=DesInBit40;DesOutBit71=DesInBit00;\
	 DesOutBit72=DesInBit50;DesOutBit73=DesInBit10;\
	 DesOutBit74=DesInBit60;DesOutBit75=DesInBit20;\
	 DesOutBit76=DesInBit70;DesOutBit77=DesInBit30;\
}

#define DesDat32To48() {\
	 DesOutBit00=DesInBit37;DesOutBit01=DesInBit00;\
	 DesOutBit02=DesInBit01;DesOutBit03=DesInBit02;\
	 DesOutBit04=DesInBit03;DesOutBit05=DesInBit04;\
	 DesOutBit06=DesInBit03;DesOutBit07=DesInBit04;\
	 DesOutBit10=DesInBit05;DesOutBit11=DesInBit06;\
	 DesOutBit12=DesInBit07;DesOutBit13=DesInBit10;\
	 DesOutBit14=DesInBit07;DesOutBit15=DesInBit10;\
	 DesOutBit16=DesInBit11;DesOutBit17=DesInBit12;\
	 DesOutBit20=DesInBit13;DesOutBit21=DesInBit14;\
	 DesOutBit22=DesInBit13;DesOutBit23=DesInBit14;\
	 DesOutBit24=DesInBit15;DesOutBit25=DesInBit16;\
	 DesOutBit26=DesInBit17;DesOutBit27=DesInBit20;\
	 DesOutBit30=DesInBit17;DesOutBit31=DesInBit20;\
	 DesOutBit32=DesInBit21;DesOutBit33=DesInBit22;\
	 DesOutBit34=DesInBit23;DesOutBit35=DesInBit24;\
	 DesOutBit36=DesInBit23;DesOutBit37=DesInBit24;\
	 DesOutBit40=DesInBit25;DesOutBit41=DesInBit26;\
	 DesOutBit42=DesInBit27;DesOutBit43=DesInBit30;\
	 DesOutBit44=DesInBit27;DesOutBit45=DesInBit30;\
	 DesOutBit46=DesInBit31;DesOutBit47=DesInBit32;\
	 DesOutBit50=DesInBit33;DesOutBit51=DesInBit34;\
	 DesOutBit52=DesInBit33;DesOutBit53=DesInBit34;\
	 DesOutBit54=DesInBit35;DesOutBit55=DesInBit36;\
	 DesOutBit56=DesInBit37;DesOutBit57=DesInBit00;\
}
/*---*/
#define DesDatBitSwap() {\
	\
	 DesOutBit02=DesInBit00;DesOutBit03=DesInBit05;\
	 DesOutBit04=DesInBit01;DesOutBit05=DesInBit02;\
	 DesOutBit06=DesInBit03;DesOutBit07=DesInBit04;\
	\
	 DesOutBit12=DesInBit06;DesOutBit13=DesInBit13;\
	 DesOutBit14=DesInBit07;DesOutBit15=DesInBit10;\
	 DesOutBit16=DesInBit11;DesOutBit17=DesInBit12;\
	\
	 DesOutBit22=DesInBit14;DesOutBit23=DesInBit21;\
	 DesOutBit24=DesInBit15;DesOutBit25=DesInBit16;\
	 DesOutBit26=DesInBit17;DesOutBit27=DesInBit20;\
	\
	 DesOutBit32=DesInBit22;DesOutBit33=DesInBit27;\
	 DesOutBit34=DesInBit23;DesOutBit35=DesInBit24;\
	 DesOutBit36=DesInBit25;DesOutBit37=DesInBit26;\
	\
	 DesOutBit42=DesInBit30;DesOutBit43=DesInBit35;\
	 DesOutBit44=DesInBit31;DesOutBit45=DesInBit32;\
	 DesOutBit46=DesInBit33;DesOutBit47=DesInBit34;\
	\
	 DesOutBit52=DesInBit36;DesOutBit53=DesInBit43;\
	 DesOutBit54=DesInBit37;DesOutBit55=DesInBit40;\
	 DesOutBit56=DesInBit41;DesOutBit57=DesInBit42;\
	\
	 DesOutBit62=DesInBit44;DesOutBit63=DesInBit51;\
	 DesOutBit64=DesInBit45;DesOutBit65=DesInBit46;\
	 DesOutBit66=DesInBit47;DesOutBit67=DesInBit50;\
	\
	 DesOutBit72=DesInBit52;DesOutBit73=DesInBit57;\
	 DesOutBit74=DesInBit53;DesOutBit75=DesInBit54;\
	 DesOutBit76=DesInBit55;DesOutBit77=DesInBit56;\
}

#define DesDat32To32() {\
	DesOutBit00=DesInBit17;DesOutBit01=DesInBit06;\
	DesOutBit02=DesInBit23;DesOutBit03=DesInBit24;\
	DesOutBit04=DesInBit34;DesOutBit05=DesInBit13;\
	DesOutBit06=DesInBit33;DesOutBit07=DesInBit20;\
	DesOutBit10=DesInBit00;DesOutBit11=DesInBit16;\
	DesOutBit12=DesInBit26;DesOutBit13=DesInBit31;\
	DesOutBit14=DesInBit04;DesOutBit15=DesInBit21;\
	DesOutBit16=DesInBit36;DesOutBit17=DesInBit11;\
	DesOutBit20=DesInBit01;DesOutBit21=DesInBit07;\
	DesOutBit22=DesInBit27;DesOutBit23=DesInBit15;\
	DesOutBit24=DesInBit37;DesOutBit25=DesInBit32;\
	DesOutBit26=DesInBit02;DesOutBit27=DesInBit10;\
	DesOutBit30=DesInBit22;DesOutBit31=DesInBit14;\
	DesOutBit32=DesInBit35;DesOutBit33=DesInBit05;\
	DesOutBit34=DesInBit25;DesOutBit35=DesInBit12;\
	DesOutBit36=DesInBit03;DesOutBit37=DesInBit30;\
}
/************************************************************/
void DesKeyGroup(unsigned char *pDesKeyIn,unsigned char *pDesKeyOut)
//void DesKeyGroup(unsigned char *pDesKeyOut)
{
	xdata unsigned char i,j,k;
	xdata unsigned long t1,t2;
 
	for(i=0;i<8;i++)
		DesChangeAreaIn[i]=*(pDesKeyIn+i);

	DesKey64To56();/*L 4 bytes="28bits//R" 4 bytes="28bit//右对齐"*/

	DesChangeAreaOut[0]&=0xf;t1=DesChangeAreaOut[0];
	t1<<=8;t1+=DesChangeAreaOut[1];
	t1<<=8;t1+=DesChangeAreaOut[2];
	t1<<=8;t1+=DesChangeAreaOut[3];

	DesChangeAreaOut[4]&=0xf;t2=DesChangeAreaOut[4];
	t2<<=8;t2+=DesChangeAreaOut[5];
	t2<<=8;t2+=DesChangeAreaOut[6];
	t2<<=8;t2+=DesChangeAreaOut[7];

	for(i=0;i<16;i++)
	{  
		for(j=0;j<DesKeyMoveBit[i];j++)
		{		/*循环左移位数*/
			t1<<=1;
			if(t1&0x10000000)
				t1++;
			t2<<=1;
			if(t2&0x10000000)
				t2++;   
		}
  
		DesChangeAreaIn[0]=(unsigned char)(t1>>24);
		DesChangeAreaIn[1]=(unsigned char)(t1>>16);
		DesChangeAreaIn[2]=(unsigned char)(t1>>8);
		DesChangeAreaIn[3]=(unsigned char)t1;
		DesChangeAreaIn[4]=(unsigned char)(t2>>24);
		DesChangeAreaIn[5]=(unsigned char)(t2>>16);
		DesChangeAreaIn[6]=(unsigned char)(t2>>8);
		DesChangeAreaIn[7]=(unsigned char)t2;

		DesKey56To48();/*L 6 bytes*/

		for(k=0;k<6;k++)
			*(pDesKeyOut+i*6+k)=DesChangeAreaOut[k];
	}
}
/************************************************************/
void DesDatReplace(unsigned char *pReplaceDat,unsigned char *pReturnDat)
{
	xdata unsigned char replaceDat[8];
	xdata unsigned char i;

	for(i=0;i<6;i++)
	{ 
		DesChangeAreaIn[i]=*(pReplaceDat+i);
	}
	/*6bits * 8*/
	DesDatBitSwap();
	for(i=0;i<8;i++)
	{
		replaceDat[i]=DesChangeAreaOut[i];
		replaceDat[i]&=0x3f;
	}
	/*6bits--->4bits*/
	replaceDat[0]=DesReplaceT1[replaceDat[0]>>4][replaceDat[0]&0xf];
	replaceDat[1]=DesReplaceT2[replaceDat[1]>>4][replaceDat[1]&0xf];
	replaceDat[2]=DesReplaceT3[replaceDat[2]>>4][replaceDat[2]&0xf];
	replaceDat[3]=DesReplaceT4[replaceDat[3]>>4][replaceDat[3]&0xf];
	replaceDat[4]=DesReplaceT5[replaceDat[4]>>4][replaceDat[4]&0xf];
	replaceDat[5]=DesReplaceT6[replaceDat[5]>>4][replaceDat[5]&0xf];
	replaceDat[6]=DesReplaceT7[replaceDat[6]>>4][replaceDat[6]&0xf];
	replaceDat[7]=DesReplaceT8[replaceDat[7]>>4][replaceDat[7]&0xf];
	/*4 bytes*/
	replaceDat[0]=(replaceDat[0]<<4)+replaceDat[1];
	replaceDat[1]=(replaceDat[2]<<4)+replaceDat[3];
	replaceDat[2]=(replaceDat[4]<<4)+replaceDat[5];
	replaceDat[3]=(replaceDat[6]<<4)+replaceDat[7];
 
	for(i=0;i<4;i++) 
		DesChangeAreaIn[i]=replaceDat[i];

	DesDat32To32();

	for(i=0;i<4;i++) 
		*(pReturnDat+i)=DesChangeAreaOut[i];
}
/************************************************************/
void DES_Encrypt(unsigned char *pDesDatIn,unsigned char *pDesKey,unsigned char *pDesDatOut)
//void DesExeEncrypt(void)
{
	xdata unsigned char desNewKey[16][6];
	xdata unsigned char i,t;
	xdata unsigned char desDatL[4],desDatR[4];
	xdata unsigned char desDatLn[4],desDatRn[4];
	xdata unsigned char desDatTmp[6];

	DesKeyGroup(pDesKey,&desNewKey[0][0]);
	//DesKeyGroup(&desNewKey[0][0]);

	for(i=0;i<8;i++)
	DesChangeAreaIn[i]=*(pDesDatIn+i);

	DesDat64IP();/*初始置换*/

	for(i=0;i<4;i++)/*左半部分*/
		desDatL[i]=DesChangeAreaOut[i];
	for(i=0;i<4;i++)/*右半部分*/
		desDatR[i]=DesChangeAreaOut[4+i];

	/////
	for(i=0;i<16;i++)
	{
		for(t=0;t<4;t++)
		{
			desDatLn[t]=desDatR[t];
			DesChangeAreaIn[t]=desDatR[t];
		}
		/*32-->48 bits*/
		DesDat32To48();
		for(t=0;t<6;t++) desDatTmp[t]=DesChangeAreaOut[t];
		/*xor Key*/
		for(t=0;t<6;t++)
		{
			/*加密*/desDatTmp[t]^=desNewKey[i][t];
		}
		/*replace 48-->32 bits*/
		DesDatReplace(desDatTmp,desDatR);
	  
		for(t=0;t<4;t++)
		{
			desDatRn[t]=desDatR[t]^desDatL[t];
		}
		/*ready for next*/
		for(t=0;t<4;t++)
		{
			desDatR[t]=desDatRn[t];
			desDatL[t]=desDatLn[t];
		}
	}
	/////
	for(i=0;i<4;i++) DesChangeAreaIn[i]=desDatR[i];
	for(i=0;i<4;i++) DesChangeAreaIn[4+i]=desDatL[i];

	DesDat64IP_R();/*重新换位*/

	for(i=0;i<8;i++)
		*(pDesDatOut+i)=DesChangeAreaOut[i];
}


void DES_Decrypt(unsigned char *pDesDatIn, unsigned char *pDesKey, unsigned char *pDesDatOut)
{
	xdata unsigned char desNewKey[16][6];
	xdata unsigned char i,t;
	xdata unsigned char desDatL[4],desDatR[4];
	xdata unsigned char desDatLn[4],desDatRn[4];
	xdata unsigned char desDatTmp[6];

	DesKeyGroup(pDesKey,&desNewKey[0][0]);
	//DesKeyGroup(&desNewKey[0][0]);

	for(i=0;i<8;i++)
		DesChangeAreaIn[i]=*(pDesDatIn+i);

	DesDat64IP();/*初始置换*/

	for(i=0;i<4;i++)/*左半部分*/
		desDatL[i]=DesChangeAreaOut[i];
	for(i=0;i<4;i++)/*右半部分*/
		desDatR[i]=DesChangeAreaOut[4+i];

 /////
	for(i=0;i<16;i++)
	{
		for(t=0;t<4;t++)
		{
   			desDatLn[t]=desDatR[t];
   			DesChangeAreaIn[t]=desDatR[t];
  		}
		/*32-->48 bits*/
		DesDat32To48();
		for(t=0;t<6;t++) desDatTmp[t]=DesChangeAreaOut[t];
		/*xor Key*/
		for(t=0;t<6;t++)
		{
			/*解密*/desDatTmp[t]^=desNewKey[15-i][t];
		}
		/*replace 48-->32 bits*/
		DesDatReplace(desDatTmp,desDatR);
  
		for(t=0;t<4;t++)
		{
			desDatRn[t]=desDatR[t]^desDatL[t];
		}
		/*ready for next*/
		for(t=0;t<4;t++)
		{
		   desDatR[t]=desDatRn[t];
		   desDatL[t]=desDatLn[t];
		}
	}
	 /////
	for(i=0;i<4;i++) DesChangeAreaIn[i]=desDatR[i];
	for(i=0;i<4;i++) DesChangeAreaIn[4+i]=desDatL[i];

	DesDat64IP_R();/*重新换位*/

	for(i=0;i<8;i++)
		*(pDesDatOut+i)=DesChangeAreaOut[i];
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void encrypt(unsigned char *input, unsigned char *key, unsigned char *output)
{
	DES_Encrypt(input, key, output);
}

void decrypt(unsigned char *input, unsigned char *key, unsigned char *output)
{
	DES_Decrypt(input, key, output);
}

/////////////////////////////3DES//////////////////////////////////////////
void TripleDES(unsigned char *input, unsigned char *key, unsigned char *output)
{
	unsigned char tmpbuf1[16];
	unsigned char tmpbuf2[16];

	encrypt(input,key,tmpbuf1);
	decrypt(tmpbuf1,&key[8],tmpbuf2);
	encrypt(tmpbuf2,key,output);
}

//void TripleDES_1(unsigned char *input, unsigned char *key, unsigned char *output)
//{
//	unsigned char tmpbuf1[16];
//	unsigned char tmpbuf2[16];
//
//	decrypt(input,key,tmpbuf1);
//	encrypt(tmpbuf1,&key[8],tmpbuf2);
//	decrypt(tmpbuf2,key,output);
//}



