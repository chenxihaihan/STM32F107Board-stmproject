/*******************************2012-2013, NJUT, Edu.************************** 
FileName: lcd_ssd1289.c 
Author:  �ﶬ÷       Version :  1.0        Date: 2013.12.30
Description:    lcd����      
Version:         1.0 
History:         
      <author>  <time>   <version >   <desc> 
      Sundm    13/12/30     1.0     �ļ�����   
  *          STM32Board Key Pin assignment
  *          =============================
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+-----------------------------+
  *          |      LCD_Pin                |     Port & Pin              |
  *          +-----------------------------+-----------------------------+
  *          |        CS                   |        C7                   |
  *          |        RD                   |        D13                  |
  *          |        WR                   |        D14                  |
  *          |        RS                   |        D15                  |
  *          |        LCD_D0-D15           |        E0-E15               |
  *          +-----------------------------+-----------------------------+
 ******************************************************************************/ 
#include "lcd_ssd1289.h"

/* LCD Control pins */
#define LCD_Pin_WR      GPIO_Pin_14
#define LCD_PORT_WR     GPIOD
#define LCD_CLK_WR      RCC_APB2Periph_GPIOD

#define LCD_Pin_CS      GPIO_Pin_7
#define LCD_PORT_CS     GPIOC
#define LCD_CLK_CS      RCC_APB2Periph_GPIOC

#define LCD_Pin_RS      GPIO_Pin_15
#define LCD_PORT_RS     GPIOD
#define LCD_CLK_RS      RCC_APB2Periph_GPIOD

#define LCD_Pin_RD      GPIO_Pin_13
#define LCD_PORT_RD     GPIOD
#define LCD_CLK_RD      RCC_APB2Periph_GPIOD

#define LCD_DATA_PORT   	GPIOE
#define LCD_DATA_APB2Periph	RCC_APB2Periph_GPIOE 

/** ���ſ��ƺ궨�� */
#define lcd_set_cs()  GPIO_SetBits(LCD_PORT_CS, LCD_Pin_CS)
#define lcd_clr_cs()  GPIO_ResetBits(LCD_PORT_CS, LCD_Pin_CS)

#define lcd_set_rs()  GPIO_SetBits(LCD_PORT_RS, LCD_Pin_RS)
#define lcd_clr_rs()  GPIO_ResetBits(LCD_PORT_RS, LCD_Pin_RS)

#define lcd_set_wr()  GPIO_SetBits(LCD_PORT_WR, LCD_Pin_WR)
#define lcd_clr_wr()  GPIO_ResetBits(LCD_PORT_WR, LCD_Pin_WR)

#define lcd_set_rd()  GPIO_SetBits(LCD_PORT_RD, LCD_Pin_RD)
#define lcd_clr_rd()  GPIO_ResetBits(LCD_PORT_RD, LCD_Pin_RD)

#define LCD_Write(LCD_DATA)  GPIO_Write(LCD_DATA_PORT, LCD_DATA)
#define LCD_Read()  GPIO_ReadInputData(LCD_DATA_PORT)


/*��̬�������� */
static void lcd_pin_config(void);
static void lcd_write_reg(u16 reg,u16 value);
static void lcd_ram_prepare(void);
static u16 lcd_read_ram(void);
static void lcd_write_ram(u16 value);
static u16  lcd_bgr2rgb(u16 c);
static u16 lcd_read_sta(void);
static void lcd_data_as_input(void);
static void lcd_data_as_output(void);

/*�������� */
unsigned long const LCD_POW10[10] = 
{
  1 , 10, 100, 1000, 10000,
  100000, 1000000, 10000000, 100000000, 1000000000
};

/****************************************************************************
* ��    �ƣ�   lcd_config
* ��    �ܣ�  LCD��ʼ������
* ��ڲ�������       
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void lcd_config(void)
{
	int num;
	lcd_pin_config();	
	
	/** �������ų�ʼ�� */
	lcd_set_cs();	
	lcd_set_wr();
	lcd_set_rd();
	lcd_set_rs();
	
	/** �����ڲ����� */
 	lcd_write_reg(0x0000,0x0001);		

        num = 100000; 
        while(--num);/**< �ȴ�osc�ȶ� */

	/** ִ���ϵ����� */
	lcd_write_reg(0x0003,0xA8A4);		
	lcd_write_reg(0x000C,0x0000);        
	lcd_write_reg(0x000D,0x080C);        
	lcd_write_reg(0x000E,0x2B00);        
	lcd_write_reg(0x001E,0x00B0);  
        
        num = 100000; 
        while(--num);
	
	/*
          * .13  0��RGB��ֵԽСԽ����1��RGB��ֵԽ��Խ��
	  * .11  0��RGB��1: BGR
          * .9   0��319-->0��1: 0-->319
          * .14  0��719-->0��1: 0-->719
          * .0 ~.8  ��������к� (0x13f = 319)
	*/
	lcd_write_reg(0x0001,0x293F);		
	
	lcd_write_reg(0x0002,0x0600);     	/**< LCD Driving Waveform control */
	lcd_write_reg(0x0010,0x0000);     	/**< .0  0: �ر�˯��ģʽ; 1: ��˯��ģʽ */
	lcd_write_reg(0x0011,0x6070);	        /**< .13-.14  11��16λRGBģʽ��10��18λRGBģʽ */	
	lcd_write_reg(0x0016,0xEF1C);           /**< .15-.8   ����ÿ�е���������0xef: ��Ϊ240 */
	lcd_write_reg(0x0017,0x0003);           /**< Vertical Porch */
	lcd_write_reg(0x0007,0x0233);           /**< Display Control */  
	lcd_write_reg(0x000B,0x0000);           /**< Frame Cycle Control */  
	lcd_write_reg(0x000F,0x0000);	        /**< Gate Scan Position */  
	lcd_write_reg(0x0041,0x0000);     	/**< Vertical Scroll Control */  
	lcd_write_reg(0x0042,0x0000);     	/**< Vertical Scroll Control */  
	lcd_write_reg(0x0048,0x0000);     	/**< Screen driving position */  
	lcd_write_reg(0x0049,0x013F);     	/**< Screen driving position */  
	lcd_write_reg(0x004A,0x0000);     	/**< Screen driving position */  
	lcd_write_reg(0x004B,0x0000);     	/**< Screen driving position */  
	lcd_write_reg(0x0044,0xEF00);     	/**< Horizontal RAM address position */  
	lcd_write_reg(0x0045,0x0000);           /**< Horizontal RAM address position */   
	lcd_write_reg(0x0046,0x013F);     	/**< Horizontal RAM address position */  
	lcd_write_reg(0x0030,0x0707);     	/**< Gamma Control */  
	lcd_write_reg(0x0031,0x0204);     	/**< Gamma Control */  
	lcd_write_reg(0x0032,0x0204);     	/**< Gamma Control */  
	lcd_write_reg(0x0033,0x0502);     	/**< Gamma Control */  
	lcd_write_reg(0x0034,0x0507);     	/**< Gamma Control */  
	lcd_write_reg(0x0035,0x0204);     	/**< Gamma Control */  
	lcd_write_reg(0x0036,0x0204);           /**< Gamma Control */   
	lcd_write_reg(0x0037,0x0502);           /**< Gamma Control */    
	lcd_write_reg(0x003A,0x0302);           /**< Gamma Control */    
	lcd_write_reg(0x003B,0x0302);           /**< Gamma Control */    
	lcd_write_reg(0x0023,0x0000);           /**< RAM write data mask */   
	lcd_write_reg(0x0024,0x0000);     	/**< RAM write data mask */  
	lcd_write_reg(0x0025,0x8000);     	/**< Frame Frequency Control; 0X8000: ��Ӧ��Ļ��ӦƵ��Ϊ65Hz */  
	lcd_write_reg(0x004e,0);                /**< ��(X)�׵�ַ���� */  
	lcd_write_reg(0x004f,0);                /**< ��(Y)�׵�ַ���� */		       		   
}

/****************************************************************************
* ��    �ƣ�lcd_clr_screen
* ��    �ܣ�LCD��������
* ��ڲ�����������ɫ����
* ���ڲ�������
* ˵    ����
* ���÷�����lcd_clr_screen�� GREEN �� �ο�ͷ�ļ�����ɫ����
****************************************************************************/
void lcd_clr_screen(u16 color)
{
	u32 index=0;
	lcd_set_cursor(0,0); 
	lcd_ram_prepare(); 	

	lcd_clr_cs();
	lcd_set_rs();

	for(index=0;index<76800;index++)        /** 320*240 = 76800 */
	{
		lcd_clr_wr();
		LCD_Write(color);
		lcd_set_wr();		
	}
	lcd_set_cs();   
}	

/****************************************************************************
* ��    �ƣ�lcd_set_cursor
* ��    �ܣ��������λ��
* ��ڲ�����x ��  y ��
* ���ڲ�������
* ˵    ����
* ���÷�����lcd_set_cursor��0��0�� �������λ��Ϊ��0��0��
****************************************************************************/
void lcd_set_cursor(u16 x,u16 y)
{
 	lcd_write_reg(0x004e,y);		
	lcd_write_reg(0x004f,x);		
}

/****************************************************************************
* ��    �ƣ�lcd_set_windows
* ��    �ܣ����ô�����ʼ��ͽ�����λ��
* ��ڲ�����[in]������ʼ��ͽ�����λ��
* ���ڲ�������
* ˵    ����
* ���÷�����lcd_set_windows��0��0��20��20��
****************************************************************************/
void lcd_set_windows(u16 start_x,u16 start_y,u16 end_x,u16 end_y)
{
	lcd_set_cursor(start_x, start_y);
	lcd_write_reg(0x0050, start_x);
	lcd_write_reg(0x0052, start_y);
	lcd_write_reg(0x0051, end_x);
	lcd_write_reg(0x0053, end_y);
}

/****************************************************************************
* ��    �ƣ�lcd_set_point
* ��    �ܣ�ָ�����ص㸳ֵ
* ��ڲ����� x �� y �� value ���ص㴦����ɫֵ
* ���ڲ�������
* ˵    ����
* ���÷�����lcd_set_point��0��0��GREEN��
****************************************************************************/
void lcd_set_point(u16 x, u16 y, u16 value)
{
	lcd_set_cursor(x, y);
	lcd_ram_prepare();
	lcd_write_ram(value);	
}

/****************************************************************************
* ��    �ƣ�u16 lcd_get_point(u16 x,u16 y)
* ��    �ܣ���ȡָ���������ɫֵ
* ��ڲ�����x      ������
*           y      ������
* ���ڲ�������ǰ������ɫֵ
* ˵    ����
* ���÷�����i=lcd_set_point(10,10);
****************************************************************************/
u16 lcd_get_point(u16 x,u16 y)
{
  lcd_set_cursor(x,y);
  return (lcd_bgr2rgb(lcd_read_ram()));
}

/**************************************************************************
* ��    ��  : LCD_Pins_Config
* ��    ��  : ����LCD�˿� �������ģʽ
* ��ڲ���    ��
* ���ڲ���  : ��
* ˵    ��  : ��
* ���÷���  : ��
**************************************************************************/
static void lcd_pin_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(LCD_CLK_RS | LCD_DATA_APB2Periph |
                         LCD_CLK_WR | LCD_CLK_RD |
                         LCD_CLK_CS, ENABLE);

  // DB15--0
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);

  //LCD_Pin_WR
  GPIO_InitStructure.GPIO_Pin = LCD_Pin_WR;
  GPIO_Init(LCD_PORT_WR, &GPIO_InitStructure);

  //LCD_Pin_CS
  GPIO_InitStructure.GPIO_Pin = LCD_Pin_CS;
  GPIO_Init(LCD_PORT_CS, &GPIO_InitStructure);

  //LCD_Pin_RS
  GPIO_InitStructure.GPIO_Pin = LCD_Pin_RS;
  GPIO_Init(LCD_PORT_RS, &GPIO_InitStructure);

  //LCD_Pin_RD
  GPIO_InitStructure.GPIO_Pin = LCD_Pin_RD;
  GPIO_Init(LCD_PORT_RD, &GPIO_InitStructure);
}	

/****************************************************************************
* ��    �ƣ�lcd_write_reg���ڲ�������
* ��    �ܣ���ָ���Ĵ���д����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
static void lcd_write_reg(u16 reg,u16 value)
{
	lcd_clr_cs();
	lcd_clr_rs();
	lcd_clr_wr();
	LCD_Write(reg);
	lcd_set_wr();

	lcd_set_rs();
	lcd_clr_wr();
	LCD_Write(value);
	lcd_set_wr();
	lcd_set_cs();
}

/****************************************************************************
* ��    �ƣ� lcd_read_reg ���ڲ�������
* ��    �ܣ����Ĵ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
u16 lcd_read_reg(u16 reg)
{
      u16 data;
  /* Write 16-bit Index (then Read Reg) */
	lcd_clr_cs();
	lcd_clr_rs();
	lcd_clr_wr();
	LCD_Write(reg);
	lcd_set_wr();

  /* Read 16-bit Reg */
	lcd_set_rs();
	lcd_clr_rd();
	lcd_set_rd();
	data = LCD_Read(); 
	lcd_set_cs();
       return    data;
}
    
/****************************************************************************
* ��    �ƣ� lcd_readsta ���ڲ�������
* ��    �ܣ���״̬
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
static u16 lcd_read_sta(void)
{
  u16 data;
  /* Write 16-bit Index, then Write Reg */
  lcd_set_rs();
  lcd_clr_rd() ;
  lcd_set_rd() ;
  data = LCD_Read(); 
  lcd_set_cs() ;    
  return    data;
}

/****************************************************************************
* ��    �ƣ�lcd_ram_prepare ���ڲ�������
* ��    �ܣ�׼����дRAM
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
static void lcd_ram_prepare(void)
{
	lcd_clr_cs();
	lcd_clr_rs();
	lcd_clr_wr();
	LCD_Write(0x0022);
	lcd_set_wr();
	lcd_set_cs();
}

/****************************************************************************
* ��    �ƣ�lcd_write_ram ���ڲ�������
* ��    �ܣ�д����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
static void lcd_write_ram(u16 value)
{
	lcd_clr_cs();
	lcd_set_rs();
	lcd_clr_wr();
	LCD_Write(value);
	lcd_set_wr();
	lcd_set_cs();
}

/****************************************************************************
* ��    �ƣ�lcd_data_as_input ���ڲ�������
* ��    �ܣ�data�˿���Ϊ�����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
static void lcd_data_as_input(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);
}

/****************************************************************************
* ��    lcd_data_as_output ���ڲ�������
* ��    �ܣ�data�˿���Ϊ�����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
static void lcd_data_as_output(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);
}

/****************************************************************************
* ��    �ƣ�lcd_read_ram ���ڲ�������
* ��    �ܣ���RAM
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
static u16 lcd_read_ram(void)
{
	uint16_t dummy;
	uint16_t value;
	lcd_ram_prepare();
	lcd_data_as_input();
	dummy = lcd_read_sta();
	dummy ++;
	value = lcd_read_sta();
	lcd_data_as_output();
	
	return value;
}

/****************************************************************************
* ��    lcd_bgr2rgb ���ڲ�������
* ��    �ܣ������ص��BGR��ʽת��ΪRGB��ʽ(BBBBBGGGGGGRRRRR -> RRRRRGGGGGGBBBBB)
* ��ڲ�����c BGR��ʽ
* ���ڲ�����RGB��ʽ
* ˵    ����
* ���÷�����
****************************************************************************/
static u16 lcd_bgr2rgb(u16 c)
{
	u16  b, g, r, rgb;
	
	r = (c>>0)  & 0x1f;
	g = (c>>5)  & 0x3f;
	b = (c>>11) & 0x1f;
	
	rgb =  r << 11 + g << 5 + b << 0;
	
	return( rgb );
}


/*-----------------------------emWin ���ú���---------------------------------*/
/****************************************************************************
* ��    �ƣ�lcd_write_reg1��lcd_write_data��lcd_read_data_multiple
* ��    �ܣ� ��LCDConf.c�ļ��е���
****************************************************************************/
void lcd_write_reg1(u16 reg)
{
	lcd_clr_cs();
	lcd_clr_rs();
	lcd_clr_wr();
	LCD_Write(reg);
	lcd_set_wr();
	lcd_set_cs();
}

void lcd_write_data(uint16_t dat)
{
    lcd_write_ram(dat);
}

void lcd_read_data_multiple(uint16_t *p_dat, uint16_t num_items)
{
    lcd_data_as_input();
    lcd_clr_cs();
    lcd_set_rs();
    lcd_clr_rd(); 
    lcd_set_rd();
    while(num_items--)
    {
        *p_dat++ = LCD_Read(); 
    }
    lcd_set_cs();
    lcd_data_as_output();
}


