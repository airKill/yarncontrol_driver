#include "main.h"

KEY_T s_tBtn[KEY_COUNT];
KEY_FIFO_T s_tKey;		/* ����FIFO����,�ṹ�� */
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKey
*	����˵��: ��ʼ������. �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
  bsp_InitKeyVar();		/* ��ʼ���������� */
  KEY_Config();		/* ��ʼ������Ӳ�� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_PutKey
*	����˵��: ��1����ֵѹ�밴��FIFO��������������ģ��һ��������
*	��    ��:  _KeyCode : ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
  s_tKey.Buf[s_tKey.Write] = _KeyCode;
  
  if (++s_tKey.Write  >= KEY_FIFO_SIZE)
  {
    s_tKey.Write = 0;
  }
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKey
*	����˵��: �Ӱ���FIFO��������ȡһ����ֵ��
*	��    ��:  ��
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
  uint8_t ret;
  
  if (s_tKey.Read == s_tKey.Write)
  {
    return KEY_NONE;
  }
  else
  {
    ret = s_tKey.Buf[s_tKey.Read];
    
    if (++s_tKey.Read >= KEY_FIFO_SIZE)
    {
      s_tKey.Read = 0;
    }
    return ret;
  }
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKeyState
*	����˵��: ��ȡ������״̬
*	��    ��:  _ucKeyID : ����ID����0��ʼ
*	�� �� ֵ: 1 ��ʾ���£� 0 ��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
  return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SetKeyParam
*	����˵��: ���ð�������
*	��    �Σ�_ucKeyID : ����ID����0��ʼ
*			_LongTime : �����¼�ʱ��
*			 _RepeatSpeed : �����ٶ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
  s_tBtn[_ucKeyID].LongTime = _LongTime;			/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
  s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;			/* �����������ٶȣ�0��ʾ��֧������ */
  s_tBtn[_ucKeyID].RepeatCount = 0;						/* ���������� */
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_ClearKey
*	����˵��: ��հ���FIFO������
*	��    �Σ���
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
void bsp_ClearKey(void)
{
  s_tKey.Read = s_tKey.Write;
}

void KEY_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  /* Enable the PAx Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = KEY1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(KEY1_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = KEY2_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(KEY2_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = KEY3_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(KEY3_PORT, &GPIO_InitStructure);
}

u8 IsKeyDown1(void)
{
  if(READ_KEY1() == 0)
    return 1;
  else 
    return 0;
}
u8 IsKeyDown2(void) 
{
  if(READ_KEY2() == 0)
    return 1;
  else 
    return 0;
}
u8 IsKeyDown3(void)
{
  if(READ_KEY3() == 0)
    return 1;
  else 
    return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKeyVar
*	����˵��: ��ʼ����������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitKeyVar(void)
{
  uint8_t i;
  
  /* �԰���FIFO��дָ������ */
  s_tKey.Read = 0;
  s_tKey.Write = 0;
  s_tKey.Read2 = 0;
  
  /* ��ÿ�������ṹ���Ա������һ��ȱʡֵ */
  for (i = 0; i < KEY_COUNT; i++)
  {
    s_tBtn[i].LongTime = KEY_LONG_TIME;
    s_tBtn[i].Count = KEY_FILTER_TIME / 2;		/* ����������Ϊ�˲�ʱ���һ�� */
    s_tBtn[i].State = 0;							/* ����ȱʡ״̬��0Ϊδ���� */
    //s_tBtn[i].KeyCodeDown = 3 * i + 1;				/* �������µļ�ֵ���� */
    //s_tBtn[i].KeyCodeUp   = 3 * i + 2;				/* ��������ļ�ֵ���� */
    //s_tBtn[i].KeyCodeLong = 3 * i + 3;				/* �������������µļ�ֵ���� */
    s_tBtn[i].RepeatSpeed = 10;						/* �����������ٶȣ�0��ʾ��֧������ */
    s_tBtn[i].RepeatCount = 0;						/* ���������� */
  }
  
  /* �жϰ������µĺ��� */
  s_tBtn[0].IsKeyDownFunc = IsKeyDown1;
  s_tBtn[1].IsKeyDownFunc = IsKeyDown2;
  s_tBtn[2].IsKeyDownFunc = IsKeyDown3;
  
  s_tBtn[2].RepeatSpeed = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DetectKey
*	����˵��: ���һ��������������״̬�����뱻�����Եĵ��á�
*	��    ��:  �����ṹ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_DetectKey(uint8_t i)
{
  KEY_T *pBtn;
  
  /*
  ���û�г�ʼ�������������򱨴�
  if (s_tBtn[i].IsKeyDownFunc == 0)
  {
  printf("Fault : DetectButton(), s_tBtn[i].IsKeyDownFunc undefine");
}
  */
  
  pBtn = &s_tBtn[i];
  if (pBtn->IsKeyDownFunc())
  {
    if (pBtn->Count < KEY_FILTER_TIME)
    {
      pBtn->Count = KEY_FILTER_TIME;
    }
    else if(pBtn->Count < 2 * KEY_FILTER_TIME)
    {
      pBtn->Count++;
    }
    else
    {
      if (pBtn->State == 0)
      {
        pBtn->State = 1;
        
        /* ���Ͱ�ť���µ���Ϣ */
        bsp_PutKey((uint8_t)(3 * i + 1));
      }
      
      if (pBtn->LongTime > 0)
      {
        if (pBtn->LongCount < pBtn->LongTime)
        {
          /* ���Ͱ�ť�������µ���Ϣ */
          if (++pBtn->LongCount == pBtn->LongTime)
          {
            /* ��ֵ���밴��FIFO */
            bsp_PutKey((uint8_t)(3 * i + 3));
          }
        }
        else
        {
          if (pBtn->RepeatSpeed > 0)
          {
            if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
            {
              pBtn->RepeatCount = 0;
              /* ��������ÿ��10ms����1������ */
              bsp_PutKey((uint8_t)(3 * i + 1));
            }
          }
        }
      }
    }
  }
  else
  {
    if(pBtn->Count > KEY_FILTER_TIME)
    {
      pBtn->Count = KEY_FILTER_TIME;
    }
    else if(pBtn->Count != 0)
    {
      pBtn->Count--;
    }
    else
    {
      if (pBtn->State == 1)
      {
        pBtn->State = 0;
        
        /* ���Ͱ�ť�������Ϣ */
        bsp_PutKey((uint8_t)(3 * i + 2));
      }
    }
    
    pBtn->LongCount = 0;
    pBtn->RepeatCount = 0;
  }
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_KeyScan
*	����˵��: ɨ�����а���������������systick�ж������Եĵ���
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_KeyScan(void)
{
  uint8_t i;
  
  for (i = 0; i < KEY_COUNT; i++)
  {
    bsp_DetectKey(i);
  }
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
