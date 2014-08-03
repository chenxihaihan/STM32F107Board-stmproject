/*******************************2012-2013, NJUT, Edu.************************** 
FileName: touch_spi.c 
Author:  ������       Version :  2.0       Date: 2014.06.05
Description:    ����������  ����SPI���߱�д    
Version:         1.0 
History:         
      <author>  <time>   <version >   <desc> 
      ������    14/05/01     1.0     �ļ�����   
      Sundm     14/06/06     2.0     ����жϴ���   
  *          STM32Board Key Pin assignment
  *          =============================
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+-----------------------------+
  *          |       TOUCH_Pin             |     Port & Pin              |
  *          +-----------------------------+-----------------------------+
  *          |        CS                   |        C8                   |
  *          |        INT                  |        C6                   |
  *          |        MISO                 |        C11                  |
  *          |        MOSI                 |        C12                  |
  *          |        SCLK                 |        C10                  |
  *          +-----------------------------+-----------------------------+
 ******************************************************************************/ 
#include "touch.h"
#include "stm32f10x.h"
//#include "rtthread.h"

/*��̬�������� */
static uint8_t  spi_white_read_byte(uint8_t byte);
static uint16_t touch_read_ad(uint8_t cmd);
static uint16_t touch_read_xy(uint8_t cmd);

/*�궨�� */
#define TOUCH_SPI_X                 SPI3
#define TOUCH_SPI_RCC               RCC_APB1Periph_SPI3

#define TOUCH_PORT                  GPIOC                   
#define TOUCH_GPIO_RCC              RCC_APB2Periph_GPIOC

#define TOUCH_INT_PIN               GPIO_Pin_6 
#define TOUCH_CS_PIN                GPIO_Pin_8 
#define TOUCH_SCK_PIN               GPIO_Pin_10
#define TOUCH_MISO_PIN              GPIO_Pin_11
#define TOUCH_MOSI_PIN              GPIO_Pin_12     

#define TOUCH_EXTI_IRQn             EXTI9_5_IRQn 

#define CMD_READ_X                  0X90 
#define CMD_READ_Y                  0XD0 

/*******************************************************************************
*  Function Name  : ��TP_INT_config ���ڲ�������
*  Description    : ������INT�ܽ�����
*  Input          : None
*  Output         : None
*  Return         : None
*******************************************************************************/
static void TOUCH_INT_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;  
  /* Enable GPIOB, GPIOC and AFIO clock */
  RCC_APB2PeriphClockCmd(TOUCH_GPIO_RCC , ENABLE);  
  
  /* INT pins configuration */
  GPIO_InitStructure.GPIO_Pin = TOUCH_INT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   /*�������룬Ĭ����1*/
  GPIO_Init(TOUCH_PORT, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6); 

  /* Configure Button EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Set the Vector Table base address at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
  
  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Enable the EXTI5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TOUCH_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void) /* TouchScreen */
{
  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
    /* ���������º���� */  
    /* Clear the EXTI Line 6 */  
    EXTI_ClearITPendingBit(EXTI_Line6);
  }
}

/*******************************************************************************
* Function Name  : touch_config
* Description    : ���������ã��˿ڳ�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void touch_config(void)
{ 
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB1PeriphClockCmd(TOUCH_SPI_RCC, ENABLE);			
  RCC_APB2PeriphClockCmd(TOUCH_GPIO_RCC | RCC_APB2Periph_AFIO, ENABLE);

  GPIO_PinRemapConfig(GPIO_Remap_SPI3,ENABLE);

  GPIO_InitStructure.GPIO_Pin   = TOUCH_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;   
  GPIO_Init(TOUCH_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = TOUCH_MISO_PIN;
  GPIO_Init(TOUCH_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = TOUCH_MOSI_PIN;
  GPIO_Init(TOUCH_PORT, &GPIO_InitStructure);

  GPIO_SetBits(TOUCH_PORT, TOUCH_CS_PIN);
  GPIO_InitStructure.GPIO_Pin   = TOUCH_CS_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 
  GPIO_Init(TOUCH_PORT, &GPIO_InitStructure);    

  SPI_Cmd(TOUCH_SPI_X, DISABLE);                                            /**< �Ƚ���,���ܸı�MODE */ 
  SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;/**< ����ȫ˫�� */
  SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                /**< ���� */                                
  SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                /**< 8λ */  
  SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;                  /**< ����Ϊ�ߵ�ƽ */
  SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;                 /**< �����ز��񣨿���Ϊ�ߵ�ƽʱ����1������Ϊ�½��أ� */      
  SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                   /**< NSS�ź���������� */ 
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;       /**< 72M / 16 */               
  SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;               /**< ���ݴ����MSBλ��ʼ */
  SPI_InitStructure.SPI_CRCPolynomial     = 7;                              /**< CRC7 */  
  SPI_Init(TOUCH_SPI_X, &SPI_InitStructure);
  SPI_Cmd(TOUCH_SPI_X, ENABLE); 
  
  /*�򿪴������жϺ󣬰�����Ļ������ͨ�����ڴ�ӡ*/
  //TOUCH_INT_config();
}

/*******************************************************************************
* Function Name  : touch_read_x
* Description    : ��ȡX����AD����ֵ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t touch_read_x(void)
{
  return touch_read_xy(CMD_READ_X);
} 

/*******************************************************************************
* Function Name  : touch_read_y
* Description    : ��ȡY����AD����ֵ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t touch_read_y(void)
{
  return touch_read_xy(CMD_READ_Y);
}

/*******************************************************************************
* Function Name  : touch_read_xy(�ڲ�����)
* Description    : ��ȡX����Y����AD����ֵ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static uint16_t touch_read_xy(uint8_t cmd)
{
  uint16_t i, j;
  uint16_t buf[4];
  uint16_t temp;
  
  /** ��4���� */   
  for(i = 0; i < 4; i++)
  {
      buf[i]=touch_read_ad(cmd);
  }	    
  /** �������� */    
  for(i = 0; i < 3; i++)
  {
    for(j = i + 1; j < 4; j++)
    {
      if(buf[i] > buf[j])
      {
        temp   = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }	
  /** ���ƫ��̫������ */
  if((buf[2] - buf[1]) > 5)   
  {
    return 0;
  }   
  /** ȡ�м�2������ƽ��ֵ */
  else  
  {
    return ((buf[2] + buf[1]) / 2);
  } 
} 

/*******************************************************************************
* Function Name  : spi_white_read_byte(�ڲ�����)
* Description    : ����SPI��д1���ֽ�
* Input          : д���1���ֽ�
* Output         : None
* Return         : ������1���ֽ�
*******************************************************************************/
static uint8_t spi_white_read_byte(uint8_t byte)
{
  /** �ȴ�д��� */
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI_X, SPI_I2S_FLAG_TXE) == RESET);
  /** д1���ֽڵ�MOSI, ͬʱͨ��MISO����1���ֽ� */
  SPI_I2S_SendData(TOUCH_SPI_X, byte);
  /** �ȴ�MISO������� */
  while(SPI_I2S_GetFlagStatus(TOUCH_SPI_X, SPI_I2S_FLAG_RXNE) == RESET);
  /** ����MISO����ֵ */
  return SPI_I2S_ReceiveData(TOUCH_SPI_X);  
}

/*******************************************************************************
* Function Name  : touch_read_ad(�ڲ�����)
* Description    : ��adֵ
* Input          : cmd ��ȡ����
* Output         : None
* Return         : None
*******************************************************************************/
static uint16_t touch_read_ad(uint8_t cmd)	  
{ 
  uint16_t ad;

  GPIO_ResetBits(TOUCH_PORT, TOUCH_CS_PIN);
  spi_white_read_byte(cmd);
  ad = spi_white_read_byte(0);
  ad <<= 8;
  ad |= spi_white_read_byte(0);
  ad >>= 3;
  GPIO_SetBits(TOUCH_PORT, TOUCH_CS_PIN);
  return ad;     
}

