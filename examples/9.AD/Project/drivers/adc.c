/******************************2012-2013, NJTECH, Edu.************************** 
FileName: adc.c 
Author:  �ﶬ÷       Version :  1.0        Date: 2013.03.30
Description:    adc����      
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
  *          |       AIN                   |         C0                  |
  *          +-----------------------------+-----------------------------+
*******************************************************************************/ 

#include "adc.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include <stdio.h>

#define ADC1_DR_Address    ((uint32_t)0x4001244C)  //DR_Address�Ĵ�������ַ

static u16  ADCConvertedValue;

/*******************************************************************************
* Function Name  : ADC_Config
* Description    : ADC ����DMA��ʽ����ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;      
  DMA_InitTypeDef DMA_InitStructure;      
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure PC.00 (ADC Channel10) as analog input*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;    //DMA��Ӧ���������ַ
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADCConvertedValue;   //�ڴ�洢����ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//DMA��ת��ģʽΪSRCģʽ����������Ƶ��ڴ�
  DMA_InitStructure.DMA_BufferSize = 1;		   //DMA�����С��1��,��λΪDMA_MemoryDataSize
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//����һ�����ݺ��豸��ַ��ֹ����
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;	//�رս���һ�����ݺ�Ŀ���ڴ��ַ����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //�����������ݿ��Ϊ16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //DMA�����ݳߴ磬HalfWord����Ϊ16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   //ת��ģʽ��ѭ������ģʽ��
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//DMA���ȼ���
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		  //M2Mģʽ����
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);   

  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  

  /* Enable ADC1  */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);
  
  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  //������ת��ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;		  //����ɨ��ģʽ
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   //��������ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ADC�ⲿ���أ��ر�״̬
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //���뷽ʽ,ADCΪ12λ�У��Ҷ��뷽ʽ
  ADC_InitStructure.ADC_NbrOfChannel = 1;	 //����ͨ������1��
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel 10configuration ADCͨ���飬 ��10��ͨ�� ����˳��1��ת��ʱ�� */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);	  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);  
  
  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);	  
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));  
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);		
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));	   
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
}

/*******************************************************************************
* Function Name  : getadcvalue
* Description    : ��ȡ��ǰADCֵ
* Input          : None
* Output         : advalue
* Return         : None
*******************************************************************************/
void getadcvalue(uint16_t *advalue)
{
  *advalue = ADCConvertedValue;
  
}


