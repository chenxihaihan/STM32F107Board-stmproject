/******************************2012-2013, NJUT, Edu.*************************** 
FileName: adc.h 
Author:  �ﶬ÷       Version :  1.0        Date: 2013.03.30
Description:    adc���� ͷ�ļ�     
Version:         1.0 
History:         
*******************************************************************************/ 
#ifndef __adc_H
#define __adc_H
#include "stm32f10x.h"

void ADC_Config(void);
void getadcvalue(uint16_t *advalue);

#endif
