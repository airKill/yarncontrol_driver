#include "main.h"

long double  NULLMASS = 0.0;	    
u32 NULL_IDLE = 82838;  //0kg
u32 AD5 = 83363;        //0.5kg
u32 AD10 = 83891;       //1kg
u32 AD20 = 84418;       //1.5kg
u32 AD30 = 84931;       //2kg
u32 AD40 = 85420;     //2.5kg
u32 AD50 = 85950;   //3
u32 AD100 = 86473;//3.5
u32 AD150 = 86986;//4
u32 AD200 = 87508;//4.5

//u32 NULL_IDLE = 82040;  //0kg
//u32 AD5 = 83325;        //0.5kg
//u32 AD10 = 84553;       //1kg
//u32 AD20 = 85782;       //1.5kg
//u32 AD30 = 87000;       //2kg
//u32 AD40 = 88150;     //2.5kg
//u32 AD50 = 89362;   //3
//u32 AD100 = 90580;//3.5
//u32 AD150 = 91890;//4
//u32 AD200 = 93090;//4.5

u32 load_value = 0;

void InitMass(void)   
{	
  hx711_buf_Init();	//�����д�N������ֵ����ʼ��SUMΪN������ֵ�ĺ�
}

//�������
void SetNull(void)	 
{
  long double ad=0,mass=0;  
  
//  ad = hx711_filter();
  ad = Read_HX711_Count() / 100;
  if(ad < NULL_IDLE)	
    ad = NULL_IDLE;
  if((ad >= NULL_IDLE) && (ad <= AD5))	  
    mass = 500 * (ad - NULL_IDLE) / (AD5 - NULL_IDLE);
  else if((ad > AD5) && (ad <= AD10))		  
    mass = 500 * (ad - AD5) / (AD10 - AD5) + 500;
  else if((ad > AD10) && (ad <= AD20))		
    mass = 500 * (ad - AD10) / (AD20 - AD10) + 1000;
  else if((ad > AD20) && (ad <= AD30))		
    mass = 500 * (ad - AD20) / (AD30 - AD20) + 1500;
  else if((ad > AD30) && (ad <= AD40))		
    mass = 500 * (ad - AD30) / (AD40 - AD30) + 2000;
  else if((ad > AD40) && (ad <= AD50))		
    mass = 500 * (ad - AD40) / (AD50 - AD40) + 2500;
  else if((ad > AD50) && (ad <= AD100))		
    mass = 500 * (ad - AD50) / (AD100 - AD50) + 3000;
  else if((ad > AD100) && (ad <= AD150))	
    mass = 500 * (ad - AD100) / (AD150 - AD100) + 3500;
  else if(ad > AD150)				
    mass = 500 * (ad - AD150) / (AD200 - AD150) + 4000; 
  if(mass < 0)	mass = 0;//��֤mass��С��0		
  NULLMASS = mass;	
}

//��������
void get_mass(void)  
{
  long double ad=0,mass=0;     
  static u8 f0=0;
  u8 f=0;

//  ad = hx711_filter();
  ad = Read_HX711_Count() / 100;
//  printf("%f\r\n",ad);
//  if(ad > 10976927)	
//    f = 1;  
//  else			
//    f = 0;
//  if(f != f0)			  
//  {//�������ޱ���
//
//  }
//  f0 = f;
  
  if(ad < NULL_IDLE)	
    ad = NULL_IDLE;
  if((ad >= NULL_IDLE) && (ad <= AD5))	  
    mass = 500 * (ad - NULL_IDLE) / (AD5 - NULL_IDLE);
  else if((ad > AD5) && (ad <= AD10))		  
    mass = 500 * (ad - AD5) / (AD10 - AD5) + 500;
  else if((ad > AD10) && (ad <= AD20))		
    mass = 500 * (ad - AD10) / (AD20 - AD10) + 1000;
  else if((ad > AD20) && (ad <= AD30))		
    mass = 500 * (ad - AD20) / (AD30 - AD20) + 1500;
  else if((ad > AD30) && (ad <= AD40))		
    mass = 500 * (ad - AD30) / (AD40 - AD30) + 2000;
  else if((ad > AD40) && (ad <= AD50))		
    mass = 500 * (ad - AD40) / (AD50 - AD40) + 2500;
  else if((ad > AD50) && (ad <= AD100))		
    mass = 500 * (ad - AD50) / (AD100 - AD50) + 3000;
  else if((ad > AD100) && (ad <= AD150))	
    mass = 500 * (ad - AD100) / (AD150 - AD100) + 3500;
  else if(ad > AD150)				
    mass = 500 * (ad - AD150) / (AD200 - AD150) + 4000; 

  mass = mass - NULLMASS;	   
  if(mass < 0)	mass = 0;		//��֤mass��С��0
  //������õĽ�����ϴεĲ�ľ���ֵС��0.03ʱ����Ϊû�иı䣬���ν�����ϣ�ֱ�ӷ���
  if(((mass - load_value) > -0.03) && ((mass - load_value) < 0.03))	
    return;  
  load_value = (u32)mass; 
}


