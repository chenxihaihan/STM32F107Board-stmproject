/******************************2015-2016, NJTECH, Edu.*************************** 
FileName: i2c_cpal_eeprom/main.c
Author:  �ﶬ÷       Version :  1.0        Date: 2016.07.30
Description:   ������ַΪ0xA0 ��e2prom 24LC02 ÿҳ8�ֽڣ���256ҳ
ʹ�÷����� 1.��key1��д��e2prom 2.��key2�󣬶�������ӡ��
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
#include "usart.h"
#include "stm32_eval_i2c_ee_cpal.h"

#define EEPROM_ADDRESS        0xA0
#define EEPROM_PAGE_SIZE      32 
#define sEE_DevStructure sEE1_DevStructure 

#define BUFFER1_SIZE             (countof(Tx1Buffer)-1)
#define BUFFER2_SIZE             (countof(Tx2Buffer)-1)

#define EEPROM_READ_ADDR1               ((uint16_t)0x0010)
#define EEPROM_WRITE_ADDR1              ((uint16_t)0x0010)	

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
const uint8_t Tx1Buffer[] = "abcdef";

uint8_t Rx1Buffer[BUFFER1_SIZE];
/* Private function prototypes -----------------------------------------------*/
int main(void)
{
  Key_Def key;
  LED_Init();
  Key_Init();
  //Set_Keyint();//���ð����жϷ�ʽ
  UartConfig();
  
  /*------------- Initialize sEE_DevStructure -------------*/
  sEE1_DevStructure.sEEAddress = EEPROM_ADDRESS;
  sEE_DevStructure.sEEPageSize = EEPROM_PAGE_SIZE;
  sEE_DevStructure.sEEMemoryAddrMode = sEE_OPT_16BIT_REG;
  
  /*-------------- Initialize sEE peripheral -------------*/
  sEE_StructInit(&sEE_DevStructure);
  sEE_Init(&sEE_DevStructure); 
  
  while (1)
  {
/*  ������ѯ��ʽ   */
    key = KEY_Scan();
    switch (key)
    {
      case KEY1:
        LEDTog(LED1);
        printf("\n\rKEY_1\r\n");
        /* Write Data in EEPROM */
        sEE_WriteBuffer(&sEE_DevStructure, (uint8_t*)Tx1Buffer, EEPROM_WRITE_ADDR1, BUFFER1_SIZE);
        
        /* Wail until communication is complete */
        while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && 
              (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR))
        { 
          /* Application may perform other tasks while CPAL write operation is ongoing */
        }	 
        
        break;
      case KEY2:
        LEDTog(LED2);
        printf("\n\rKEY_2\r\n");
        
        /* Read Data from EEPROM */
        sEE_ReadBuffer(&sEE_DevStructure, Rx1Buffer, EEPROM_READ_ADDR1, BUFFER1_SIZE);
        
        /* Wail until communication is complete */
        while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && 
              (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR))
        {
          /* Application may perform other tasks while CPAL read operation is ongoing */
        }
        printf(Rx1Buffer);
        
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


