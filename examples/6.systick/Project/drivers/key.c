/*********************2012-2013, NJUT, Edu.********************* 
FileName: key.c 
Author:  �ﶬ÷       Version :  1.0        Date: 2013.03.30
Description:    key����      
Version:         1.0 
History:         
      <author>  <time>   <version >   <desc> 
      Sundm    13/03/30     1.0     �ļ�����   
  *          STM32Board Key Pin assignment
  *          =============================
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+-----------------------------+
  *          |      Keys                   |     Port & Pin              |
  *          +-----------------------------+-----------------------------+
  *          |      KEY1 (Tamper)          |        A0                   |
  *          |      KEY2 (Wakeup)          |        C13                  |
  *          +-----------------------------+-----------------------------+
***************************************************************/ 
#include "key.h"
#include "led.h"
/*******************************************************************************
* Function Name  : Key_Init
* Description    : ����GPIO��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
       
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
        
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

/*******************************************************************************
* Function Name  : Set_Keyint
* Description    : ���������ó��жϷ�ʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Set_Keyint(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); 
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13); 
  
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;		 
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	
  
  EXTI_InitStructure.EXTI_Line = EXTI_Line13;		 
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : Get_Key���ڲ�����
* Description    : ��ȡ���밴���Ƿ��£����·���1
* Input          : -key:ĳ������ 
* Output         : None
* Return         : 1������İ������� 0������İ���û�а���
*******************************************************************************/
uint8_t Get_Key(Key_Def key)
{
  	if ((KEY_PORT[key]->IDR&KEY_PIN[key])==0)return 1;
  	else return 0;
}

/*******************************************************************************
* Function Name  : KEY_Scan
* Description    : ��ȡ����ֵ
* Input          : None 
* Output         : None
* Return         : ���ذ��µļ�ֵ
*******************************************************************************/
Key_Def KEY_Scan(void)
{	 
  static uint8_t key_up=1;
  if(key_up&&(Get_Key(KEY1)||Get_Key(KEY2)))
  {
    Delay(0xFFFFF); 
    key_up=0;
    if(Get_Key(KEY1))return KEY1;
    else if(Get_Key(KEY2))return KEY2;
  }
  else if(!Get_Key(KEY1)&&!Get_Key(KEY2))key_up=1; 	    
  return KEYNULL;
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : �ⲿ�ж�0������,LED4��˸
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
          LEDTog(LED1);
          EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

/*******************************************************************************
* Function Name  : EXTI13_IRQHandler
* Description    : �ⲿ�ж�13������,LED2��˸
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
  	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
  	{
          LEDTog(LED2);
          EXTI_ClearITPendingBit(EXTI_Line13);
  	}
}

