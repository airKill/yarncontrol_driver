//-----------------------------------------------------------------------------
// COPYRIGHT (c) 2001-2008	Sinormous Technology
// File: SMEC98SP_I2C_hard_api.c
//-----------------------------------------------------------------------------
// ABSTRACT:
// Test application for SMEC98SP-I2C
//-----------------------------------------------------------------------------
// MODIFICATIONS:
// Author	Date		Description
// 
//-----------------------------------------------------------------------------

 #include <dw8051.h>
#include <SMEC98SP_I2C.h>

///////////////////////////////////////////////////////////////////////////////
// Set the main clock frequency
///////////////////////////////////////////////////////////////////////////////
void Set_Frequency(unsigned char Freq)
{
	EA=0;
	SRVCON|=(0x07&Freq);// Set bits for New Freq
	SRVCON&=(0xF8|Freq);// Clr bits for New Freq
	EA=1;
}




						 