#ifndef __FILTER_H
#define __FILTER_H

extern u32 NULL_IDLE;  
extern u32 AD5;
extern u32 AD10;
extern u32 AD20;
extern u32 AD30;
extern u32 AD40;
extern u32 AD50;
extern u32 AD100;
extern u32 AD150;
extern u32 AD200;

extern long double  NULLMASS;
extern u32 load_value;

void InitMass(void);
void get_mass(void); 
void SetNull(void);
#endif