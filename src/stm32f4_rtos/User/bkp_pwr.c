#include "includes.h"

void bkp_init(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//那1?邦PWR那㊣?車
	PWR_BackupAccessCmd(ENABLE);	//那1?邦o車㊣???∩??¯﹞??那 
  
  if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x5050)	
  {
    RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);
    
    MotorProcess.current_seg = 0;
    MotorProcess.songwei_seg[0] = 0;
    MotorProcess.songwei_seg[1] = 0;
    MotorProcess.songwei_seg[2] = 0;
    
    MotorProcess.current_wei = 0;
    MotorProcess.song_current_wei[0] = 0;
    MotorProcess.song_current_wei[1] = 0;
    MotorProcess.song_current_wei[2] = 0;
    
    servomotor_guodu = 0;
    stepmotor_guodu[0] = 0;
    stepmotor_guodu[1] = 0;
    stepmotor_guodu[2] = 0;
    
    write_bkp_para(&MotorProcess);
  }
  else
  {
    read_bkp_para(&MotorProcess);
  }
}

void read_bkp_para(MOTOR_PROCESS *motor)
{
  motor->current_seg = RTC_ReadBackupRegister(RTC_BKP_DR1);
  motor->songwei_seg[0] = RTC_ReadBackupRegister(RTC_BKP_DR2);
  motor->songwei_seg[1] = RTC_ReadBackupRegister(RTC_BKP_DR3);
  motor->songwei_seg[2] = RTC_ReadBackupRegister(RTC_BKP_DR4);
  motor->current_wei = RTC_ReadBackupRegister(RTC_BKP_DR5);
  motor->song_current_wei[0] = RTC_ReadBackupRegister(RTC_BKP_DR6);
  motor->song_current_wei[1] = RTC_ReadBackupRegister(RTC_BKP_DR7);
  motor->song_current_wei[2] = RTC_ReadBackupRegister(RTC_BKP_DR8);
  servomotor_guodu = RTC_ReadBackupRegister(RTC_BKP_DR9);
  stepmotor_guodu[0] = RTC_ReadBackupRegister(RTC_BKP_DR10);
  stepmotor_guodu[1] = RTC_ReadBackupRegister(RTC_BKP_DR11);
  stepmotor_guodu[2] = RTC_ReadBackupRegister(RTC_BKP_DR12);
}

void write_bkp_para(MOTOR_PROCESS *motor)
{
  RTC_WriteBackupRegister(RTC_BKP_DR1,motor->current_seg);
  RTC_WriteBackupRegister(RTC_BKP_DR2,motor->songwei_seg[0]);
  RTC_WriteBackupRegister(RTC_BKP_DR3,motor->songwei_seg[1]);
  RTC_WriteBackupRegister(RTC_BKP_DR4,motor->songwei_seg[2]);
  RTC_WriteBackupRegister(RTC_BKP_DR5,motor->current_wei);
  RTC_WriteBackupRegister(RTC_BKP_DR6,motor->song_current_wei[0]);
  RTC_WriteBackupRegister(RTC_BKP_DR7,motor->song_current_wei[1]);
  RTC_WriteBackupRegister(RTC_BKP_DR8,motor->song_current_wei[2]);
  RTC_WriteBackupRegister(RTC_BKP_DR9,servomotor_guodu);
  RTC_WriteBackupRegister(RTC_BKP_DR10,stepmotor_guodu[0]);
  RTC_WriteBackupRegister(RTC_BKP_DR11,stepmotor_guodu[1]);
  RTC_WriteBackupRegister(RTC_BKP_DR12,stepmotor_guodu[2]);
}
