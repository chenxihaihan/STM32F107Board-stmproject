/******************************2012-2014, NJTECH, Edu.*************************** 
FileName: lcd/main.c
Author:  �ﶬ÷       Version :  1.0        Date: 2014.01.30
Description:    LCD���ܲ��������� �ڴ��ڳ���������γ� ��LCD����ʾͼƬ���ַ� 
Version:         1.0 
History:         
      <author>  <time>   <version >   <desc> 
      Sundm    14/01/30     1.0     �ļ�����   
*******************************************************************************/ 

#include "stm32f10x.h"
#include <stdio.h>
#include "led.h"
#include "usart.h"
#include "key.h"
#include "lcd.h"

extern void testlcd();

int main(void)
{
  Key_Def key;
  uint8_t str[10];
  uint8_t * p_str = str;   
  LED_Init();
  Key_Init();
  UartConfig();
  UartPrintf("abc\n"); //�����ַ���
  UartPrintf("%d %d %d\n", 987654321,22,33);//������������
  UartPrintf("%c%c%d\n" ,0x32,0x38,0x12);//����16����������������
  UartPrintf("abcedg\n");//�����ַ���
  testlcd();
  while (1)
  {
/*  ������ѯ��ʽ */  
    key = KEY_Scan();
    switch (key)
    {
      case KEY1:
        LEDTog(LED1);
        printf("\n\rKEY_1");
        printf("please input string: \n");
        scanf("%s",p_str); 
        printf("%s\n",p_str);  	
        break;
      case KEY2:
        LEDTog(LED2);
        printf("\n\rKEY_2");
        break;
      case KEYNULL:
        break;
      default :
        break;
    
    }
    
  }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
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

/**
  * @}
  */


