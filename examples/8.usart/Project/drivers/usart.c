/*********************2012-2013, NJUT, Edu.********************* 
FileName: usart.c 
Author:  �ﶬ÷       Version :  1.0        Date: 2013.04.10
Description:    usart1����      
Version:         1.0 
History:         
      <author>  <time>   <version >   <desc> 
      Sundm    13/04/10     1.0     �ļ�����   
  *          STM32Board Key Pin assignment
  *          =============================
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+-----------------------------+
  *          |      FunctionPin            |     Port & Pin              |
  *          +-----------------------------+-----------------------------+
  *          |      USART1_TX              |        PA9                  |
  *          |      USART1_RX              |        PA10                 |
  *          +-----------------------------+-----------------------------+
***************************************************************/ 
#include "usart.h"
#include "led.h"

/*******************************************************************************
* Function Name  : UartConfig
* Description    : USART1��ʼ��,115200,��У�飬8���ݣ�1ֹͣ���жϽ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UartConfig(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
     
        /* Enable GPIO clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

        /* Enable the USART1 Pins Software Remapping */
        //IO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin   = UART1_GPIO_TX;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(UART1_GPIO, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin   = UART1_GPIO_RX;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(UART1_GPIO, &GPIO_InitStructure); 
        
	/* Enable the USART1 Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
        NVIC_Init(&NVIC_InitStructure); 

	/* USART configuration */
	USART_InitStructure.USART_BaudRate   = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
        
        /*�����ж�ʹ��*/
        //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
    
	/* Enable USART */
	USART_Cmd(USART1, ENABLE);	
	
	/*��֤���͵ĵ�һ���ֽڲ��ᶪʧ*/
	USART_ClearFlag(USART1, USART_FLAG_TC);
}

/*******************************************************************************
* Function Name  : PutChar
* Description    : USART1����һ�ֽ�
* Input          : ch Ҫ���͵��ֽ�
* Output         : None
* Return         : None
*******************************************************************************/
void PutChar(uint8_t ch)
{
	USART_SendData(USART1, ch);  
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 	
}

/*******************************************************************************
* Function Name  : PutString
* Description    : USART1�����ַ���
* Input          : str Ҫ���͵��ַ���
* Output         : None
* Return         : None
*******************************************************************************/
void PutString(uint8_t *str)
{
	while(*str)
	{
		USART_SendData(USART1, *str++); 
		/*Loop until the end of transmission*/
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 	
	}
}

/*******************************************************************************
* Function Name  : myitoa �ڲ�����
* Description    : ת���ַ�������ʽ���ַ�����
* Input          : buf Ҫת�����ַ��� dataת������
* Output         : None
* Return         : None
*******************************************************************************/
void myitoa(int data,char *buf )
{
	int temp,j=0,i=0;
 	while(data)    //�����������֣����Լ�ȡ�����ֲ��ԣ���123�������ַ������е�ֵΪ321
 	{
    	buf[i++] = data%10+'0';//��ת����������ַ�������ַ�������
    	data = data/10;    //ɾ���Ѿ�ת�������֣�Ϊȡ��һ����������׼��
 	}
 	buf[i--]='\0';    //ת�������Ҫ���ַ���������һ���ַ���������־'/0'��������һ���ַ���
 	while( j < i )    //�ո�ת���õ��ַ���������ı��������ת����
 	{
  		temp = buf[j];
  		buf[j] = buf[i];
  		buf[i] = temp;
  		i--,j++;
 	}
}

/*******************************************************************************
* Function Name  : UartPrintf 
* Description    : ���ڸ�ʽ����ӡ
* Input          : format ��ʽ���ַ� ��������printfʹ�ø�ʽ��ͬ��%c %s %d 
* Output         : None
* Return         : None
*******************************************************************************/
void UartPrintf(const char *format, ...)
{
	va_list ap;
	char c,nc;


	va_start(ap, format);	 //���ҵ��󽫲�����ջ,apָ��format
	while (c = *format++)		
	{
		
		if(c == '%'&&(nc = *format++) != '\0')
		{
			switch(nc)
	  		{
          		case 'c':  //���1���ַ�
		 	{
               		        char ch = va_arg(ap, int);  //���ú�ջλ��+1
               		        PutChar(ch);        //�����ַ�
               		        break;
          		}
          		case 's': //����ַ���
			{
               		        char *p = va_arg(ap, char *);
               		        PutString((u8 *)p);    //�����ַ���
               		        break;
          		}
			case 'd':
			{
				int data = va_arg(ap,int);
       				char buf[16];
       				myitoa(data,buf);
       				PutString((u8 *)buf);//�����ַ���
       				break;
			}
          		default:
               		    PutChar(nc); //�����ַ�
        	}
	    }else
	    {PutChar(c);}
	}
     va_end(ap);	//�ر�ָ��
}

/*******************************************************************************
* Function Name  : GetChar 
* Description    : ���ڽ���һ���ַ�
* Input          : None
* Output         : None
* Return         : ���յ����ַ�
*******************************************************************************/
uint8_t GetChar(void)
{ 
    uint8_t ch;	
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);    
    ch = USART_ReceiveData(USART1);	   									 
    return ch;   
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler 
* Description    : ���ڽ����жϳ��򣬽���һ���ַ���ͨ�����ڷ��ͣ�LED��˸һ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void) 

{ 
    unsigned char  ch; 
    if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET) 
    {               
        ch = USART_ReceiveData(USART1);
        USART_SendData(USART1,ch); 
        LEDTog(LED1);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) ;  
    }
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
    { 
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/*******************************************************************************
* Function Name  : fputc �ڲ�����
* Description    : �򴮿�д�뵥���ַ���printf��������ô˺���,���ø�ʽʡ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);  
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
	return ch;
}

/*******************************************************************************
* Function Name  : fgetc �ڲ�����
* Description    : ���ڻ�ȡ�����ַ���scanf��������ô˺��������ø�ʽʡ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int fgetc(FILE *f)
{
    int ch;
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);    
    ch = USART_ReceiveData(USART1);	   									 
    return ch;   
}	
