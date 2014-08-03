/*********************2012-2013, NJUT, Edu.********************* 
FileName: timer.c 
Author:  �ﶬ÷       Version :  1.0        Date: 2013.04.10
Description:    timer����      
Version:         1.0 
History:         
      <author>  <time>   <version >   <desc> 
      Sundm    13/04/10     1.0     �ļ�����   
***************************************************************/ 
#include "stm32f10x.h"
#include "led.h"
	 

//////////////////////////////////////////////////////////////////////
/*******************************************************************************
* Function Name  : Timer5Config
* Description    : Timer5��ʱ�����ú���,��ʱ���ڣ�1S ��Ƶ��1/7200 ���ϼ���10000
*                  72M * 1/7200 *10000 = 1S
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#include "timer.h"
void Timer5Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;  	
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

  TIM_TimeBaseStructure.TIM_Period = (10000 - 1);		 	
  TIM_TimeBaseStructure.TIM_Prescaler = (7200 - 1);	
  TIM_TimeBaseStructure.TIM_ClockDivision = 0 ;			
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);		
  
  TIM_ClearITPendingBit(TIM5, TIM_IT_Update);		
  TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);		  	
  TIM_Cmd(TIM5, ENABLE);				       

  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
  NVIC_Init(&NVIC_InitStructure);	                       
}	

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : ��ʱ��5�жϷ�����
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    /*
      ��������Լ��Ĵ���
    */
    LEDTog(LED1);
    
  }
}

