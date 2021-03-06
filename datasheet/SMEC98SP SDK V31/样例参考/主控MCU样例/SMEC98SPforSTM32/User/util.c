#include "stm32f10x.h"
#include "config.h"
#include "util.h"
#include <stdarg.h>



#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


void USART1_Init(void)
{
    USART_InitTypeDef USART_InitStructure;

    /* USARTx configuration ------------------------------------------------------*/
    /* USARTx configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_2;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure the USARTx */
    USART11_Init(USART1, &USART_InitStructure);
    /* Enable the USARTx */
    USART_Cmd(USART1, ENABLE);


}


/*******************************************************************************
* Function Name  : Delay_us
* Description    : Delay per micro second.
* Input          : time_us
* Output         : None
* Return         : None
*******************************************************************************/
//test ok
void Delay_us( u8 time_us )
{
    u8 i;
    u8 j;
    for( i=0; i<time_us; i++ )

        for( j=0; j<7; j++ );        // 25CLK


}

/*******************************************************************************
* Function Name  : Delay_ms
* Description    : Delay per mili second.
* Input          : time_ms
* Output         : None
* Return         : None
*******************************************************************************/
//test ok
void Delay_ms( u16 time_ms )
{
    u16  j;
    for(; time_ms>0; time_ms--)
    {
        j = 8300;
        while(--j);
    }
}

/*******************************************************************************
* Function Name  : PUTCHAR_PROTOTYPE
* Description    : Retargets the C library printf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
PUTCHAR_PROTOTYPE
{
  // Write a character to the USART
  USART_SendData(USART1, (uint8_t) ch);

  //  Loop until the end of transmission
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {
  }

  return ch;
}
*/

int put_char(int ch)
{
    // Write a character to the USART
    USART_SendData(USART1, (uint8_t) ch);

    //  Loop until the end of transmission
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }

    return ch;
}


void PrintHex(unsigned char *str,unsigned int len)
{
    unsigned int i;
    unsigned char a1,a2;
    for(i=0; i<len; i++)
    {
        a1=str[i]>>4;
        if(a1<10)a1+='0';
        else a1=a1-10+'A';
        a2=str[i]&0x0f;
        if(a2<10)a2+='0';
        else a2=a2-10+'A';
        put_char(a1);
        put_char(a2);
    }
    put_char('\r');
    put_char('\n');

}


int get_char(void)
{
    int ch;

    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {
    }

    ch = USART_ReceiveData(USART1);

    return ch;
}
int fputc(int ch, FILE *f)
{
    /* ��Printf���ݷ������� */
    USART_SendData(USART1, (unsigned char) ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return (ch);
}
/*******************************************************************************
* Function Name  : time_return
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

// uint32_t time_return(void)
// {
//   ;
// }


#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif



