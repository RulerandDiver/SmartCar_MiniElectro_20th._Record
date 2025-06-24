#include "Key.h"

bit Go_Flag=0;

//KEY_UP,KEY_FUNC,KEY_DOWN,KEY_GO
Key_tt Key[4] = {0,0,0,0};

//拨码开关状态变量
uint8 sw1_status;
uint8 sw2_status;

void Key_Task()
{
	static uint16 Voltage_Count=0;
	static bit Voltage_Flag=0;
	
	
	if(Key[0].flag == 1)
	{
		Key[0].flag = 0;
	}
	
	if(Key[1].flag == 1)
	{
		Key[1].flag = 0;
	}
	
	if(Key[2].flag == 1)
	{
		Key[2].flag = 0;
	}
	
	if(Key[3].flag == 1)
	{
		Key[3].flag = 0;
		Go_Flag=1;
	}
	
#if 0
	//外侧上拨启动
	if(sw1_status)
	{
		Test_Wireless_Oscilloscope();//无线转串口调试PID		
	}
	if(sw2_status)
	{	
		//电压检测
		if(++Voltage_Count == 100) Voltage_Count=0,Voltage_Flag=1;//不能太快，会让单片机挂掉
		if(Voltage_Flag)
		{
			Voltage_Flag=0;
			Bat_Vol = ADC_Voltage();
		}
	}
	
	/* 冲出赛道保护 */
	if(L<1 && L2<1 && R2<1 && R<1)run=0;
	else run=1;
	
	/* 速度设置 & 差速计算 */
	if(run == 1 && running == 1)  // 在赛道内
	{	
		Left_High_Speed  = 80;
		Right_High_Speed = 80   ;
	}
	if(run == 0 && running == 1) // 出赛道 进入保护,编码器计步代替延时，计步一段距离，若仍然丢线，则停车，后期补上，前期无需停车
	{
		Left_High_Speed  = 0;
		Right_High_Speed = 0;
		eleOut=0;		
	}
#endif
}

void Key_Scan()
{
	uint8 i;
	Key[0].state = KEY_UP;
	Key[1].state = KEY_FUNC;
	Key[2].state = KEY_DOWN;
	Key[3].state = KEY_GO;
	
	sw1_status = SW1_PIN;
	sw2_status = SW2_PIN;
	
	for(i=0;i<4;i++)
	{
		switch (Key[i].step)
		{
			//按键低电平有效，首次有效进入按下次数记录（消抖），因按键放在定时器中断内判断，因此消抖有效
			case 0:
			{
				if (Key[i].state == 0)
				{
					Key[i].step = 1;
				}
			}
			break;
			//如果仍然为低电平，则认为单击有效
			case 1:
			{
				if (Key[i].state == 0)
				{
					Key[i].step = 2;
					Key[i].flag = 1;
				}
				else
				{
					Key[i].step = 0;
				}
			}
			break;
			//计步状态清零
			case 2:
			{
				if (Key[i].state == 1)
				{
					Key[i].step = 0;
				}
			}
			break;
		}
	}
	
	
}




























