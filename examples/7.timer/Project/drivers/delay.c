/*********************2012-2013, NJUT, Edu.********************* 
FileName: delay.c 
Author:  �ﶬ÷       Version :  1.0        Date: 2013.04.05
Description:    ��systick���о�ȷ��ʱ      
Version:         1.0 
History:         
      <author>  <time>   <version >   <desc> 
      Sundm    13/04/05     1.0     �ļ�����   
***************************************************************/   
#include "delay.h"

__IO u16 ntime;								    

/*******************************************************************************
* Function Name  : delay_ms
* Description    : ��ʱms
* Input          : nms
* Output         : None
* Return         : None
*******************************************************************************/
void delay_ms(uint16_t nms)
{	 		  	  
	ntime=nms;
	SysTick_Config(SystemCoreClock/1000);
	while(ntime);
	SysTick->CTRL=0x00;			  	    
}   
		    								   
/*******************************************************************************
* Function Name  : delay_us
* Description    : ��ʱus
* Input          : nus
* Output         : None
* Return         : None
*******************************************************************************/
void delay_us(uint32_t nus)
{		
	ntime=nus;
	SysTick_Config(SystemCoreClock/1000000);
	while(ntime);
	SysTick->CTRL=0x00;
}
/*******************************************************************************
* Function Name  : Delay
* Description    : ����ʱ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(unsigned int t)
{ 
	while(t--) ;	
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : ϵͳ�δ�ʱ���жϷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
	ntime--;
}
