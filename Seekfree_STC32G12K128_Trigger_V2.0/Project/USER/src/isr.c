  /***********************************************************
//         ����         ��    ��    ��     ��            ����
//                       .::::.
//                     .::::::::.
//                    :::::::::::
//                 ..:::::::::::'
//              '::::::::::::'
//                .::::::::::
//           '::::::::::::::..
//                ..::::::::::::.
//              ``::::::::::::::::
//               ::::``:::::::::'        .:::.
//              ::::'   ':::::'       .::::::::.
//            .::::'      ::::     .:::::::'::::.
//           .:::'       :::::  .:::::::::' ':::::.
//          .::'        :::::.:::::::::'      ':::::.
//         .::'         ::::::::::::::'         ``::::.
//     ...:::           ::::::::::::'              ``::.
//    ```` ':.          ':::::::::'                  ::::..
//                       '.:::::'                    ':'````..
***************************************************************/
#include "headfile.h"
#include "Key.h"
#include "Elec.h"
#include "Filtering.h"
#include "isr.h"
/*******************************************************������*********************************************************/
uint16 SysTick=0; //��ʱ����ѯ

// ������������ر���
static bit beep_flag = 0;          // ������������־��1��ʾ����
static uint8 beep_count = 0;        // ��������ʱ������
static uint8 track_ten_cnt = 0;    //���뻷�ظ��ж���ʱ��

// �����ǽ��ٶȵ�ԭʼֵ�Ϳ������˲�֮���ֵ
bit Gyro_Int=0;   /*�����ǻ���*/
float Gyro_Z=0;
float Filtered_GyroZ=0;

int16 positionReal = 0;                        //ʵ��������Ϊpid�����position,��λ�����

int g_SpeedPoint = 80;							//Ŀ���ٶȣ�һ�������ֻ����ٶȱ���һ��

volatile uint8  r_position = 55;
volatile uint16 r_distance = 6400;
volatile uint16 s_distance = 5000;

//PID����
float Turn_Pid=0;
float Velo_Pid=0;

//ռ�ձ�
int16 g_DutyLeft = 0, g_DutyRight = 0;         // �������Ҫ�������PWMֵ

/*******************************************************������*********************************************************/


void TM0_Isr() interrupt 1
{
	static count;
	if(count++==10) count=0,P64 ^= 1;
	//����ɨ��
	Key_Scan();
	//��ʱ����ѯ����
	if(SysTick < 100){
		SysTick++;
	}
	
    /* ����������ͱ仯�����Ʒ����� */
    if (track_type != track_type_last)
    {
        // �������ͷ����仯������������
        beep_flag = 1;
        beep_count = 0;  // ���ü�����
        P24 = 1;  // �򿪷�����
        
        // ������һ����������
        track_type_last = track_type;
    }
	
    /* ��������ʱ���� */
    if (beep_flag)
    {
        beep_count++;
        // 10ms * 20 = 200ms
        if (beep_count >= 10)
        {
            beep_count = 0;
            beep_flag = 0;
            P26 = 0;  // �رշ�����
        }
    }	
	
	/* ����ʮ��Բ����ʱ�ж� */
	if (ten_change_flag == 1)
	{
		// 10ms * 150 = 1500ms
		track_ten_cnt++;
		if (track_ten_cnt >= 150)
		{
			track_ten_flag = 1;
			track_ten_cnt = 0;
			ten_change_flag = 0;
		}
	}
	
}

extern void Ultima_Control(void);
void TM1_Isr() interrupt 3
{
	static count;
	//Ultima_Control();
	if(count++==20) count=0,P65 ^= 1;	
	
	/* ��ȡ�������������ֵ */
	g_encoleft_init  = get_left_encoder();
	g_encoright_init = get_right_encoder();
	
	/* �Ա�������ֵ�����˲� */
	g_EncoderLeft  = LowPass_Filter(&leftSpeedFilt, g_encoleft_init);
	g_EncoderRight = LowPass_Filter(&rightSpeedFilt, g_encoright_init);
	
	/* �Ա�������ֵ�����쳣���� */
	g_EncoderLeft  = encoder_debounce(&EncoderL, g_EncoderLeft);
	g_EncoderRight = encoder_debounce(&EncoderR, g_EncoderRight);
	
	/* ȡ���ұ�����ƽ��ֵ */
	g_EncoderAverage = (g_EncoderLeft*2 + g_EncoderRight) / 2;


	/* ��ȡ������ԭʼ���ݲ�����ת��Ϊ�������� */
	imu660ra_get_gyro();
	Gyro_Z = imu660ra_acc_transition(imu660ra_gyro_z);


	/* �ٶȾ��� */
	if (track_type == 0)//��ֱͨ��
	{		
		positionReal = position;
		g_SpeedPoint = SPEED_STRAIGHT;
	}
	else if (track_type == 1)//ֱ�����
	{		
		positionReal = position;
		g_SpeedPoint = SPEED_ANGLE;
	}
	else if (track_type == 2)//ʮ��Բ���ڲ�
	{		
		positionReal = position;
		g_SpeedPoint = SPEED_CROSS;
	}
	else if (track_type == 3 && track_route_status == 1)//Բ���뻷
	{
		g_SpeedPoint = 60;
		//�������Ʋ�
		g_intencoderALL += g_EncoderAverage;
		//s_distance��ʵ�ʾ��룬���Խ�g_intencoderALL �˳�һ��ϵ���õ�ʵ�ʾ���
		if(g_intencoderALL <= (int)s_distance)//��һ�׶���ֱ��
		{
			positionReal = 0;
		}
		else//����ڶ��׶δ�������
		{
			if (track_route == 1)//�һ�
			{
				positionReal = -r_position;
			}
			else if (track_route == 2)//��
			{
				positionReal = 40;
			}
						
			if (g_intencoderALL >= (int)r_distance)//�뻷���
			{
				track_route_status = 2;
				g_intencoderALL = 0;
				g_SpeedPoint = SPEED_ROUNDABOUT;
			}
		}
	}
	else if (track_type == 3 && track_route_status == 2)//�����ڲ�
	{
		positionReal = position;
		g_SpeedPoint = SPEED_ROUNDABOUT;
	}
	else if (track_type == 3 && track_route_status == 3)//Բ������
	{
		g_SpeedPoint = SPEED_ROUNDABOUT;
		g_intencoderALL += (g_EncoderLeft + g_EncoderRight) / 2;
		
		if (g_intencoderALL <= 2500)//��һ�׶δ�������
		{
			if (track_route == 1)//�һ�
			{
				positionReal = -50;
			}
			else if (track_route == 2)//��
			{
				positionReal = 50;
			}
		}
		else//�ڶ��׶�ֱ��
		{
			positionReal = 0;
			
			if (g_intencoderALL >= 4000)//�������
			{
				track_type = 0;
				track_route = 0;
				track_route_status = 0;
				
				g_intencoderALL = 0;
			}
		}
	}
	else if (track_type == 4)//��ȼ���ģʽ
	{
		positionReal = position;
		g_SpeedPoint = SPEED_SPEED;
	}


	//����������������ſ�ʼ��PID����ֹPID�ܷ�
	if (Go_Flag == 1)
	{
		Filtered_GyroZ = Kalman_Update(&imu693_kf, Gyro_Z);//��Gyro_Z���п������˲�
		
		Turn_Pid = Pid_Pos_GyroZ(&TurnPID, positionReal, Filtered_GyroZ);
		
		Kalman_Predict(&imu693_kf, Turn_Pid);//���¿������˲�����ֵ

		//�����ٶȻ�pid

		Velo_Pid = Pid_Inc_Ff(&SpeedPID, g_EncoderAverage, g_SpeedPoint);

		
		//תint
		g_DutyLeft  = (int16)(Velo_Pid - Turn_Pid);
		g_DutyRight = (int16)(Velo_Pid + Turn_Pid);

		if (protection_flag == 1)
		{
//			pid_clean(&LeftPID);
//			pid_clean(&RightPID);
			Pid_Clean(&SpeedPID);
			Pid_Clean(&TurnPID);
			
			//uartSendFlag = 0;
			
			Motor_Ctrl(0, 0);
		}
		else
		{
			Motor_Ctrl(g_DutyLeft, g_DutyRight);
		}
	}

	
}















//UART1�ж�
void UART1_Isr() interrupt 4
{
    uint8 res;
	static uint8 dwon_count;
    if(UART1_GET_TX_FLAG)
    {
        UART1_CLEAR_TX_FLAG;
        busy[1] = 0;
    }
    if(UART1_GET_RX_FLAG)
    {
        UART1_CLEAR_RX_FLAG;
        res = SBUF;
        //�����Զ�����
        if(res == 0x7F)
        {
            if(dwon_count++ > 20)
                IAP_CONTR = 0x60;
        }
        else
        {
            dwon_count = 0;
        }
    }
}

//UART2�ж�
void UART2_Isr() interrupt 8
{
    if(UART2_GET_TX_FLAG)
	{
        UART2_CLEAR_TX_FLAG;
		busy[2] = 0;
	}
    if(UART2_GET_RX_FLAG)
	{
        UART2_CLEAR_RX_FLAG;
		//�������ݼĴ���Ϊ��S2BUF
		gps_uart_callback(S2BUF);
	}
}


//UART3�ж�
void UART3_Isr() interrupt 17
{
    if(UART3_GET_TX_FLAG)
	{
        UART3_CLEAR_TX_FLAG;
		busy[3] = 0;
	}
    if(UART3_GET_RX_FLAG)
	{
        UART3_CLEAR_RX_FLAG;
		//�������ݼĴ���Ϊ��S3BUF

	}
}


//UART4�ж�
void UART4_Isr() interrupt 18
{
    if(UART4_GET_TX_FLAG)
	{
        UART4_CLEAR_TX_FLAG;
		busy[4] = 0;
	}
    if(UART4_GET_RX_FLAG)
	{
        UART4_CLEAR_RX_FLAG;

		//�������ݼĴ���Ϊ��S4BUF;
		if(wireless_module_uart_handler != NULL)
		{
			// �ú���Ϊ����ָ��
			// �ٳ�ʼ������ģ���ʱ�����øú�����ַ
			wireless_module_uart_handler(S4BUF);
		}
	}
}

#define LED P52
void INT0_Isr() interrupt 0
{
	LED = 0;	//����LED
}
void INT1_Isr() interrupt 2
{

}
void INT2_Isr() interrupt 10
{
	INT2_CLEAR_FLAG;  //����жϱ�־
}
void INT3_Isr() interrupt 11
{
	INT3_CLEAR_FLAG;  //����жϱ�־
}

void INT4_Isr() interrupt 16
{
	INT4_CLEAR_FLAG;  //����жϱ�־
}



void TM2_Isr() interrupt 12
{
	TIM2_CLEAR_FLAG;  //����жϱ�־
}
void TM3_Isr() interrupt 19
{
	TIM3_CLEAR_FLAG; //����жϱ�־
	
}

void TM4_Isr() interrupt 20
{
	TIM4_CLEAR_FLAG; //����жϱ�־
}

//void  INT0_Isr()  interrupt 0;
//void  TM0_Isr()   interrupt 1;
//void  INT1_Isr()  interrupt 2;
//void  TM1_Isr()   interrupt 3;
//void  UART1_Isr() interrupt 4;
//void  ADC_Isr()   interrupt 5;
//void  LVD_Isr()   interrupt 6;
//void  PCA_Isr()   interrupt 7;
//void  UART2_Isr() interrupt 8;
//void  SPI_Isr()   interrupt 9;
//void  INT2_Isr()  interrupt 10;
//void  INT3_Isr()  interrupt 11;
//void  TM2_Isr()   interrupt 12;
//void  INT4_Isr()  interrupt 16;
//void  UART3_Isr() interrupt 17;
//void  UART4_Isr() interrupt 18;
//void  TM3_Isr()   interrupt 19;
//void  TM4_Isr()   interrupt 20;
//void  CMP_Isr()   interrupt 21;
//void  I2C_Isr()   interrupt 24;
//void  USB_Isr()   interrupt 25;
//void  PWM1_Isr()  interrupt 26;
//void  PWM2_Isr()  interrupt 27;