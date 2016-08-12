/******************************2016-2016, NJTECH, Edu.*************************** 
FileName: i2c_cpal/main.c
Author:  �ﶬ÷       Version :  1.0        Date: 2016.07.30
Description:   ʹ��CPAL�����I2C�� two_board ͨѶ
 ʹ�÷����� 1. ˫��� I2C ���ߣ�SDA SCL GND������
            2. ˫���ϵ��һ�尴��key1�󣬷������ݡ���һ��ɽ��ա�
Version:         1.0 
History:         
      <author>  <time>   <version >   <desc> 
      Sundm    16/07/30     1.0     �ļ�����   
*******************************************************************************/ 

#include "stm32f10x.h"
#include <stdio.h>
#include "led.h"
#include "key.h"
#include "delay.h"
#include "cpal_user_i2c.h"
#include "usart.h"

int main(void)
{
  Key_Def key;
  LED_Init();
  Key_Init();
  //Set_Keyint();//���ð����жϷ�ʽ
  UartConfig();
  /*1. ��CPAL�� ����˫��ͨѶ,�� CPAL_I2c_Config ��ʼ��*/
  CPAL_I2c_Config();
  
  while (1)
  {
/*  ������ѯ��ʽ   */
    key = KEY_Scan();
    switch (key)
    {
      case KEY1:
        LEDTog(LED1);
        printf("\n\rKEY_1");

        /* Write operations CPAL��I2C -----------------------------------------*/
   
        {
          User_I2c_Write();
        }

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
    
      /* 1. Read Operations CPAL��I2C  ----------------------------------------*/
      if ((I2C_DevStructure.CPAL_State == CPAL_STATE_READY) || \
        (I2C_DevStructure.CPAL_State == CPAL_STATE_DISABLED)) 
      {
        User_I2c_Read();
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


