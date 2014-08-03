/******************************2012-2014, NJTECH, Edu.*************************** 
FileName: led/main.c
Author:  �ﶬ÷       Version :  1.0        Date: 2014.01.30
Description:    ���ÿ⣬��ѧϰ����ʵ��1��LED��˸
Version:         1.0 
History:         
      <author>  <time>   <version >   <desc> 
      Sundm    14/01/30     1.0     �ļ�����   
*******************************************************************************/ 

#include <stm32f10x.h> 	 

void Delay(unsigned int t);	

main()
{  
	RCC -> APB2ENR = 1 << 5;//1����5λ [2]P71   IOPDEN��IO�˿�Dʱ��ʹ�� (I/O port D clock enable) λ5 ������á�1�����塯0�� 0��IO�˿�Dʱ�ӹرգ� 1��IO�˿�Dʱ�ӿ�����
	GPIOD -> CRL = 2 << 8;  //2����8λ(D��) [2]P113 01���������ģʽ 10�����ģʽ������ٶ�2MHz
 	while(1)
	{
		GPIOD ->BSRR = 1 << 2;
		Delay(500000); 		
		GPIOD ->BRR  = 1 << 2;  
		Delay(500000); 
	}
}

void Delay(unsigned int t)
{ 
	while(t--) ;	
}

