/* stemwin��ֲ������ */
/*
1.��������ʼ�� ���ڳ�ʼ�� 
2.��ʱ����ʼ�����ṩ10ms��ʱ������GUI_TOUCH_Exec,����OS_TimeMS
3.GUI�����󣬶�ʱ1ms����OS_TimeMS ��GUI_Delay(100)�Զ�����GUI_Exec().
4.�궨��Ļ����������Demo,��GUIDEMO.h�޸�Ҫ���еķ���;�����������ʫ��д�ļ��׼�����
*/

#include "lcd_ssd1289.h" 
#include "GUI.h"
#include "guidemo.h"
#include "timer5.h"
#include "touch.h"
extern int touch_calibrate();
extern int calculator_main();

int main(void)
{ 
  touch_config();
  Timer5Config();
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
  GUI_Init();
  
  /*�������궨*/
  //touch_calibrate();
  
  /*����Demo*/
<<<<<<< HEAD
  //GUIDEMO_Main();
  
  /*���׼�����*/
  calculator_main();  
=======
  GUIDEMO_Main();
  
  /*���׼�����*/
  //calculator_main();  
>>>>>>> a53357495e8d9c6dcf9e59a823a1ec5e5289ddbf
  while(1);
}
