/*******************************2012-2013, NJUT, Edu.************************** 
FileName: lcd.c 
Author:  �ﶬ÷       Version :  1.0        Date: 2013.12.30
Description:    lcd����  ����ili9325 ili9320 ssd1289    
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
#include "lcd.h"
#include "lcd_font.h"
#include "hanzi.h"
#include "picture.h"

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
static u16 lcd_read_reg(u16 reg);
static void lcd_ram_prepare(void);
static u16 lcd_read_ram(void);
static void lcd_write_ram(u16 value);
static u16  lcd_bgr2rgb(u16 c);
static u16 lcd_read_sta(void);
static void lcd_data_as_input(void);
static void lcd_data_as_output(void);
static void lcd_data2string(long dat, u8 len, char* str) ;

/*�������� */
unsigned long const LCD_POW10[10] = 
{
  1 , 10, 100, 1000, 10000,
  100000, 1000000, 10000000, 100000000, 1000000000
};

/*LCDģ�����Ͷ��� 0x0��ili9320 0x8989��SSD1289 0x9325��ili9325*/
u16 DeviceIdCode;

void lcd_delay(int n)
{
  int num;
  num = 100000*n; 
  while(--num);                 
}
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
  int trytimes=0;
	lcd_pin_config();	
	
	/** �������ų�ʼ�� */
	lcd_set_cs();	
	lcd_set_wr();
	lcd_set_rd();
	lcd_set_rs();
	
	/** �����ڲ����� */
 	lcd_write_reg(0x0000,0x0001);		

        lcd_delay(10); 
        
        /** ��ID */
        DeviceIdCode = lcd_read_reg(0x0000);
        while (((0 == DeviceIdCode)||(1 == DeviceIdCode))&&trytimes<10)
        {
          lcd_delay(1);       
          DeviceIdCode = lcd_read_reg(0x0000);
        }
        //printf("DeviceIdCode = 0X%X \n", DeviceIdCode);
        if(DeviceIdCode==0x8989)//SSD1289
        {
          /** ִ���ϵ����� */
          lcd_write_reg(0x0003,0xA8A4);		
          lcd_write_reg(0x000C,0x0000);        
          lcd_write_reg(0x000D,0x080C);        
          lcd_write_reg(0x000E,0x2B00);        
          lcd_write_reg(0x001E,0x00B0);  
          
          lcd_delay(1); 
          
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
        else if(DeviceIdCode==0x9325)//ili9325 
        {
          /* time control*/
          lcd_write_reg(0x00e3,0x3008);   // Set internal timing
          lcd_write_reg(0x00e7,0x0012);
          lcd_write_reg(0x00ef,0x1231);   //Set the internal vcore voltage
          /* display control*/
          lcd_write_reg(0x0001,0x0100);   // set SS and SM bit    
          lcd_write_reg(0x0002,0x0700);   // set 1 line inversion  				                 
          lcd_write_reg(0x0003,0x1030 );  // set GRAM write direction and BGR=1. 
          lcd_write_reg(0x0004,0x0000);   // Resize register                                  
          lcd_write_reg(0x0008,0x0207);   // set the back porch and front porch           
          lcd_write_reg(0x0009,0x0000);   // set non-display area refresh cycle ISC[3:0]       
          lcd_write_reg(0x000a,0x0000);   // FMARK function 
          /*RGB control*/
          lcd_write_reg(0x000c,0x0000);   // RGB interface setting    
          lcd_write_reg(0x000d,0x0000);   // Frame marker Position         
          lcd_write_reg(0x000f,0x0000);   // RGB interface polarity
          /*power control*/
          lcd_write_reg(0x0010,0x0000);   // SAP, BT[3:0], AP, DSTB, SLP, STB
          lcd_write_reg(0x0011,0x0007);   // DC1[2:0], DC0[2:0], VC[2:0] 
          lcd_write_reg(0x0012,0x0000);   // Internal reference voltage= Vci;               
          lcd_write_reg(0x0013,0x0000);   // Set VDV[4:0] for VCOM amplitude
          lcd_write_reg(0x0007,0x0000);   
          
          /*power control*/
          lcd_write_reg(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB 
          lcd_write_reg(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0] 
          lcd_write_reg(0x0012, 0x009D); // Internal reference voltage= Vci; 

          lcd_write_reg(0x0013, 0x1900); // Set VDV[4:0] for VCOM amplitude 
          lcd_write_reg(0x0029,0x0025);   // Set VCM[5:0] for VCOMH 
          lcd_write_reg(0x002b,0x000d);   // Set Frame Rate and color
          
          lcd_write_reg(0x0020,0x0000);  // GRAM horizontal Address                                                          
          lcd_write_reg(0x0021,0x0000);  // GRAM Vertical Address          

          /*Adjust the Gamma Curve  */
          lcd_write_reg(0x0030,0x0000);   
          lcd_write_reg(0x0031,0x0506);   
          lcd_write_reg(0x0032,0x0104);   
          lcd_write_reg(0x0035,0x0207);   
          lcd_write_reg(0x0036,0x000f);   
          lcd_write_reg(0x0037,0x0306);   
          lcd_write_reg(0x0038,0x0102);    
          lcd_write_reg(0x0039,0x0707);   
          lcd_write_reg(0x003c,0x0702);   
          lcd_write_reg(0x003d,0x1604);  

          /*Set GRAM area */ 
          lcd_write_reg(0x0050,0x0000);        
          lcd_write_reg(0x0051,0x00ef);   
          lcd_write_reg(0x0052,0x0000);     
          lcd_write_reg(0x0053,0x013f);
          lcd_write_reg(0x0060,0xa700);        
          lcd_write_reg(0x0061,0x0001); 
          lcd_write_reg(0x006a,0x0000);
          /*Partial Display Control*/
          lcd_write_reg(0x0080,0x0000);
          lcd_write_reg(0x0081,0x0000);
          lcd_write_reg(0x0082,0x0000);
          lcd_write_reg(0x0083,0x0000);
          lcd_write_reg(0x0084,0x0000);
          lcd_write_reg(0x0085,0x0000);
          /*Panel Control */
          lcd_write_reg(0x0090,0x0010);     
          lcd_write_reg(0x0092,0x0600);  
          /*262K color and display ON */
          lcd_write_reg(0x0007,0x0133);
        }
        else if(DeviceIdCode==0x9320)//ili9320
        {
		lcd_write_reg(0x00,0x0000);
		lcd_write_reg(0x01,0x0100);	//Driver Output Contral.
		lcd_write_reg(0x02,0x0700);	//LCD Driver Waveform Contral.
		lcd_write_reg(0x03,0x1030);//Entry Mode Set.
		//lcd_write_reg(0x03,0x1018);	//Entry Mode Set.
	
		lcd_write_reg(0x04,0x0000);	//Scalling Contral.
		lcd_write_reg(0x08,0x0202);	//Display Contral 2.(0x0207)
		lcd_write_reg(0x09,0x0000);	//Display Contral 3.(0x0000)
		lcd_write_reg(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		lcd_write_reg(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		lcd_write_reg(0x0d,0x0000);	//Frame Maker Position.
		lcd_write_reg(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
		lcd_delay(5); 
		lcd_write_reg(0x07,0x0101);	//Display Contral.
		lcd_delay(5); 								  
		lcd_write_reg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));//Power Control 1.(0x16b0)
		lcd_write_reg(0x11,0x0007);				//Power Control 2.(0x0001)
		lcd_write_reg(0x12,(1<<8)|(1<<4)|(0<<0));		//Power Control 3.(0x0138)
		lcd_write_reg(0x13,0x0b00);				//Power Control 4.
		lcd_write_reg(0x29,0x0000);				//Power Control 7.
	
		lcd_write_reg(0x2b,(1<<14)|(1<<4));	    
		lcd_write_reg(0x50,0);	//Set X Star
		//ˮƽGRAM��ֹλ��Set X End.
		lcd_write_reg(0x51,239);	//Set Y Star
		lcd_write_reg(0x52,0);	//Set Y End.t.
		lcd_write_reg(0x53,319);	//
	
		lcd_write_reg(0x60,0x2700);	//Driver Output Control.
		lcd_write_reg(0x61,0x0001);	//Driver Output Control.
		lcd_write_reg(0x6a,0x0000);	//Vertical Srcoll Control.
	
		lcd_write_reg(0x80,0x0000);	//Display Position? Partial Display 1.
		lcd_write_reg(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		lcd_write_reg(0x82,0x0000);	//RAM Address End-Partial Display 1.
		lcd_write_reg(0x83,0x0000);	//Displsy Position? Partial Display 2.
		lcd_write_reg(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		lcd_write_reg(0x85,0x0000);	//RAM Address End? Partial Display 2.
	
		lcd_write_reg(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		lcd_write_reg(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		lcd_write_reg(0x93,0x0001);	//Panel Interface Contral 3.
		lcd_write_reg(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		lcd_write_reg(0x97,(0<<8));	//
		lcd_write_reg(0x98,0x0000);	//Frame Cycle Contral.	   
		lcd_write_reg(0x07,0x0173);	//(0x0173)
        }
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
  if(DeviceIdCode==0x8989)
  {
 	lcd_write_reg(0x004e,y);		
	lcd_write_reg(0x004f,x);	
  }	
  else if(DeviceIdCode==0x9325)
  {
 	lcd_write_reg(0x0020,y);		
	lcd_write_reg(0x0021,0x13f-x);	
  }	
  else 
  {
 	lcd_write_reg(0x0020,y);		
	lcd_write_reg(0x0021,0x13f-x);	
  }	
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

/****************************************************************************
* ��    �ƣ�void lcd_draw_hanzi(u16 x,u16 y,u8 *hanziaddr,u16 charColor,u16 bkColor)
* ��    �ܣ���ָ��������ʾһ������ģ������ɵĺ����ַ�
* ��ڲ�����x          ������
*           y          ������
            hanziaddr  ����ͷָ��
*           charColor  �ַ�����ɫ
*           bkColor    �ַ�������ɫ
* ���ڲ�������
* ˵    ������ʾ��Χ�޶�Ϊ����ͷ�ļ��У��ú���ȡģ�ĵ�ɫ����Һ����ģ������ȡģ���ֽ�����
* ���÷�����lcd_draw_hanzi(10,10,(u8*)Str_table1,White, HYALINE);
****************************************************************************/
void lcd_draw_hanzi(u16 x,u16 y,u8 *hanziaddr,u16 charColor,u16 bkColor)  // 
{
  u16 i=0;
  u16 j=0;
  u16 m=0;
  u8 width,high, wbyte; 
  
  u8 *hanzipoint = hanziaddr;
  u8 tmp_char = 0;
  
  width = *hanzipoint++;
  high = *hanzipoint++;
  wbyte = *hanzipoint++;
  if(HYALINE == bkColor)
  {
    for (i=0;i<(high);i++)
    {
      for (j=0;j<wbyte;j++)
      {
        tmp_char=*hanzipoint++;
        for(m=0;m<8;m++)
        {
          if ( (tmp_char >>7-m) & 0x01 == 0x01)
            {
              lcd_set_point(x+j*8+m,y+i,charColor); // �ַ���ɫ 
            }
            else
            {
              // do nothing // ͸������
            }
        }
       }
    }
  }
  else 
  {
    for (i=0;i<(high);i++)
    {
      for (j=0;j<wbyte;j++)
      {
        tmp_char=*hanzipoint++;
        for(m=0;m<8;m++)
        {
          if ( (tmp_char >>7-m) & 0x01 == 0x01)
            {
              lcd_set_point(x+j*8+m,y+i,charColor); // �ַ���ɫ 
            }
            else
            {
              lcd_set_point(x+i,y+i-8,bkColor); // ������ɫ
            }
        }
       }
    }  
  }
}

/****************************************************************************
* ��    �ƣ�void lcd_draw_picture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *picture)
* ��    �ܣ���ָ�����귶Χ��ʾһ��ͼƬ
* ��ڲ�����StartX     ����ʼ����
*           StartY     ����ʼ����
*           EndX       �н�������
*           EndY       �н�������
            picture        ͼƬͷָ��
* ���ڲ�������
* ˵    ����ͼƬȡģ��ʽΪˮƽɨ�裬16λ��ɫģʽ (RGB565)
* ���÷�����lcd_draw_picture(0,0,100,100,(u16*)demo);
****************************************************************************/
void lcd_draw_picture(u16 start_x, u16 start_y, u16 end_x, u16 end_y, u16 *picture)
{
	u32 length;
	u32 i;
	u16 x, y;

	x = start_x;
	y = start_y;

	length = (end_x - start_x + 1) * (end_y - start_y + 1) ;  
	for (i = 0; i < length; i++)
	{
		lcd_set_point(x, y, *picture++);

		y++;
		if(y > end_y)
		{
			x++;
			y = start_y;
		}
	}  
}

/****************************************************************************
* ��    �ƣ�lcd_put_char_8x16
* ��    �ܣ����8x16�ַ�
* ��ڲ����� x �ַ��������� y �ַ��������� ch �ַ� ch_color �ַ���ɫ bk_color ������ɫ
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void lcd_put_char_8x16(u16 x, u16 y, u8 ch, u16 ch_color, u16 bk_color)
{
  u16 i,j;	
  u8 temp = 0;		

  for(i = 0; i < 16; i++)
  {
    /**< ȡ���δ����1���ֽڣ� �ֿ���ʡ����ASCII���е�ǰ32���ַ� */
    temp = ASCII_8x16[((ch - 0x20) * 16) + i];  
    for(j = 0; j < 8; j++)
    {
      if((temp >> (7 - j)) & 0x01)			  /**< �ַ���ɫ */
      { 
        lcd_set_point(x + j, y + i, ch_color); 
      } 
      else  if(bk_color != HYALINE)			  /**< ������ɫ */
      {				
        lcd_set_point(x + j, y + i, bk_color);  								
      }
    }
  }		
} 

/****************************************************************************
* ��    �ƣ�lcd_put_char_16x24
* ��    �ܣ����16x24�ַ�
* ��ڲ����� x �ַ��������� y �ַ��������� ch �ַ� ch_color �ַ���ɫ bk_color ������ɫ
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void lcd_put_char_16x24(u16 x, u16 y, u8 ch, u16 ch_color, u16 bk_color)
{
  u16 i,j;	
  u16 temp = 0;		

  for(i = 0; i < 24; i++)
  {
    temp = ASCII_16x24[((ch - 0x20) * 24) + i]; 
    for(j = 0; j < 16; j++)
    {
      if((temp >> j) & 0x01)			
      {
        lcd_set_point(x + j, y + i, ch_color); 
      }
      else  if(bk_color != HYALINE)			
      {				
        lcd_set_point(x + j, y + i, bk_color);  								
      }
    }
  }		
}

/****************************************************************************
* ��    �ƣ�lcd_put_string_8x16
* ��    �ܣ����8x16�ַ���
* ��ڲ����� x �ַ��������� y �ַ��������� *str �ַ��� ch_color �ַ���ɫ bk_color ������ɫ
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void  lcd_put_string_8x16(u16 x, u16 y, char *str,  u16 ch_color, u16 bk_color)
{
  u16 i;
  for (i = 0; *(str + i); i++)
  {
    lcd_put_char_8x16(x + (8*i), y, *(str + i), ch_color, bk_color);
  } 	
}

/****************************************************************************
* ��    �ƣ�lcd_put_string_16x24
* ��    �ܣ����16x24�ַ���
* ��ڲ����� x �ַ��������� y �ַ��������� *str �ַ��� ch_color �ַ���ɫ bk_color ������ɫ
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void  lcd_put_string_16x24(u16 x, u16 y, char *str, u16 ch_color, u16 bk_color)
{
	u16 i;
	for (i = 0; *str; i++)
	{
		lcd_put_char_16x24(x + (16*i), y, *str++, ch_color, bk_color);
	} 	
}

/****************************************************************************
* ��    �ƣ�lcd_data2string ���ڲ�������
* ��    �ܣ�������ת��Ϊ�ַ���
* ��ڲ�����dat ת��ǰ������ len ���ݵĳ��� ����֧�� <= 10�� *str ת������ַ���
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
static void lcd_data2string(long dat, u8 len, char* str) 
{
	u8 i;
  u8 c;
  long d;
	
	if(dat < 0) 
	{
		*str++ = '-';
		dat = -dat;
	} 
  for(i = len; i > 0; i--)
	{
		d = LCD_POW10[i - 1];
    c = (u8) (dat / d);
		dat -= c * d;
    *str++ = c + '0';
	}  
  *str = 0;
}

/****************************************************************************
* ��    �ƣ�lcd_put_data_8x16
* ��    �ܣ����8x16����
* ��ڲ����� x �ַ��������� y �ַ��������� dat ���� len ���ݳ���ch_color �ַ���ɫ bk_color ������ɫ
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void lcd_put_data_8x16(u16 x, u16 y, long dat, u8 len, u16 ch_color, u16 bk_color) 
{
	char dat2str[11];
	lcd_data2string(dat, len, dat2str);
	lcd_put_string_8x16(x, y, dat2str, ch_color, bk_color);
}

/****************************************************************************
* ��    �ƣ�lcd_put_data_16x24
* ��    �ܣ����16x24����
* ��ڲ����� x �ַ��������� y �ַ��������� dat ���� len ���ݳ��� *str �ַ��� ch_color �ַ���ɫ bk_color ������ɫ
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void lcd_put_data_16x24(u16 x, u16 y, long dat, u8 len, u16 ch_color, u16 bk_color) 
{
	char dat2str[11];
	lcd_data2string(dat, len, dat2str);
	lcd_put_string_16x24(x, y, dat2str, ch_color, bk_color);
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
        lcd_data_as_input();
        lcd_delay(1);
	lcd_set_rs();
	lcd_clr_rd();
	lcd_set_rd();
	data = LCD_Read(); 
	lcd_set_cs();
         lcd_data_as_output();
        lcd_delay(1);
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
        lcd_delay(1);
	dummy = lcd_read_sta();
	dummy ++;
	value = lcd_read_sta();
	lcd_data_as_output();
	lcd_delay(1);
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

/*-------------����LCD�������ڵ�ǰLCD��Ļ���Ļ���PictureAddrͼƬ-------------*/
void DrawPicture_Center(u16 *PictureAddr)  
{

    u16 PictureWidth;
    u16 PictureHeight;
    u16 * picuturepoint;
    
    picuturepoint=PictureAddr;
    
    PictureHeight = picuturepoint[1];//?����?��??????����?�㨹o??����???����D??��?D
    PictureWidth  = picuturepoint[2];//?����?��????��?����?�㨹o??����???����D??��?D
    
    lcd_draw_picture((320-PictureWidth+1)/2, (240-PictureHeight+1)/2, ((320+PictureWidth+1)/2)-1, 
                        ((240+PictureHeight+1)/2)-1, (u16 *)(PictureAddr + 8));
}


/****************************************************************************
* ��    �ƣ�testlcd �����Ժ�����
* ��    �ܣ�����ͼ����ʾ�������ַ�������ʾ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void testlcd(void)
{
 int i;
  lcd_config();
  lcd_clr_screen(BLUE);  
	for( i=0;i<319;i++)
	{	
		lcd_set_point(i, 30, GREEN);
	}

	lcd_put_string_16x24(0, 35, "picture 120X168 :", WHITE, BLUE);

	/**/for( i=0;i<319;i++)
	{	
		lcd_set_point(i, 60, RED);
	}
        DrawPicture_Center((u16 *)gImage_picture);

	for(i=0;i<319;i++)
	{	
		lcd_set_point(i, 180, RED);
	}

	lcd_draw_hanzi(5,180,(u8*)Hanzi, WHITE, HYALINE); 	  

	for(i=0;i<319;i++)
	{	
		lcd_set_point(i, 210, GREEN);
	}
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

u16 getLcdIdCode(void)
{
  return DeviceIdCode;
}

