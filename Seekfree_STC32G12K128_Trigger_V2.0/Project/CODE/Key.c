#include "Key.h"

bit Go_Flag=0;

//KEY_UP,KEY_FUNC,KEY_DOWN,KEY_GO
Key_tt Key[4] = {0,0,0,0};

//���뿪��״̬����
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
	//����ϲ�����
	if(sw1_status)
	{
		Test_Wireless_Oscilloscope();//����ת���ڵ���PID		
	}
	if(sw2_status)
	{	
		//��ѹ���
		if(++Voltage_Count == 100) Voltage_Count=0,Voltage_Flag=1;//����̫�죬���õ�Ƭ���ҵ�
		if(Voltage_Flag)
		{
			Voltage_Flag=0;
			Bat_Vol = ADC_Voltage();
		}
	}
	
	/* ����������� */
	if(L<1 && L2<1 && R2<1 && R<1)run=0;
	else run=1;
	
	/* �ٶ����� & ���ټ��� */
	if(run == 1 && running == 1)  // ��������
	{	
		Left_High_Speed  = 80;
		Right_High_Speed = 80   ;
	}
	if(run == 0 && running == 1) // ������ ���뱣��,�������Ʋ�������ʱ���Ʋ�һ�ξ��룬����Ȼ���ߣ���ͣ�������ڲ��ϣ�ǰ������ͣ��
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
			//�����͵�ƽ��Ч���״���Ч���밴�´�����¼�����������򰴼����ڶ�ʱ���ж����жϣ����������Ч
			case 0:
			{
				if (Key[i].state == 0)
				{
					Key[i].step = 1;
				}
			}
			break;
			//�����ȻΪ�͵�ƽ������Ϊ������Ч
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
			//�Ʋ�״̬����
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




























