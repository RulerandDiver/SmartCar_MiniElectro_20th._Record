  /***********************************************************
//         ！！         机    魂    大     悦            ！！
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
/*******************************************************变量区*********************************************************/
uint16 SysTick=0; //定时器轮询

// 蜂鸣器控制相关变量
static bit beep_flag = 0;          // 蜂鸣器开启标志，1表示开启
static uint8 beep_count = 0;        // 蜂鸣器计时计数器
static uint8 track_ten_cnt = 0;    //出入环重复判定计时器

// 陀螺仪角速度的原始值和卡尔曼滤波之后的值
bit Gyro_Int=0;   /*陀螺仪积分*/
float Gyro_Z=0;
float Filtered_GyroZ=0;

int16 positionReal = 0;                        //实际用来作为pid输入的position,即位置误差

int g_SpeedPoint = 80;							//目标速度，一般左右轮基础速度必须一致

volatile uint8  r_position = 55;
volatile uint16 r_distance = 6400;
volatile uint16 s_distance = 5000;

//PID参数
float Turn_Pid=0;
float Velo_Pid=0;

//占空比
int16 g_DutyLeft = 0, g_DutyRight = 0;         // 最后真正要给电机的PWM值

/*******************************************************变量区*********************************************************/


void TM0_Isr() interrupt 1
{
	static count;
	if(count++==10) count=0,P64 ^= 1;
	//按键扫描
	Key_Scan();
	//定时器轮询备用
	if(SysTick < 100){
		SysTick++;
	}
	
    /* 检测赛道类型变化并控制蜂鸣器 */
    if (track_type != track_type_last)
    {
        // 赛道类型发生变化，启动蜂鸣器
        beep_flag = 1;
        beep_count = 0;  // 重置计数器
        P24 = 1;  // 打开蜂鸣器
        
        // 更新上一次赛道类型
        track_type_last = track_type;
    }
	
    /* 蜂鸣器计时控制 */
    if (beep_flag)
    {
        beep_count++;
        // 10ms * 20 = 200ms
        if (beep_count >= 10)
        {
            beep_count = 0;
            beep_flag = 0;
            P26 = 0;  // 关闭蜂鸣器
        }
    }	
	
	/* 出入十字圆环计时判定 */
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
	
	/* 读取并清除编码器的值 */
	g_encoleft_init  = get_left_encoder();
	g_encoright_init = get_right_encoder();
	
	/* 对编码器的值进行滤波 */
	g_EncoderLeft  = LowPass_Filter(&leftSpeedFilt, g_encoleft_init);
	g_EncoderRight = LowPass_Filter(&rightSpeedFilt, g_encoright_init);
	
	/* 对编码器的值进行异常消除 */
	g_EncoderLeft  = encoder_debounce(&EncoderL, g_EncoderLeft);
	g_EncoderRight = encoder_debounce(&EncoderR, g_EncoderRight);
	
	/* 取左右编码器平均值 */
	g_EncoderAverage = (g_EncoderLeft*2 + g_EncoderRight) / 2;


	/* 读取陀螺仪原始数据并将其转化为物理数据 */
	imu660ra_get_gyro();
	Gyro_Z = imu660ra_acc_transition(imu660ra_gyro_z);


	/* 速度决策 */
	if (track_type == 0)//普通直线
	{		
		positionReal = position;
		g_SpeedPoint = SPEED_STRAIGHT;
	}
	else if (track_type == 1)//直角弯道
	{		
		positionReal = position;
		g_SpeedPoint = SPEED_ANGLE;
	}
	else if (track_type == 2)//十字圆环内部
	{		
		positionReal = position;
		g_SpeedPoint = SPEED_CROSS;
	}
	else if (track_type == 3 && track_route_status == 1)//圆环入环
	{
		g_SpeedPoint = 60;
		//编码器计步
		g_intencoderALL += g_EncoderAverage;
		//s_distance非实际距离，可以将g_intencoderALL 乘除一个系数得到实际距离
		if(g_intencoderALL <= (int)s_distance)//第一阶段先直行
		{
			positionReal = 0;
		}
		else//进入第二阶段打死进环
		{
			if (track_route == 1)//右环
			{
				positionReal = -r_position;
			}
			else if (track_route == 2)//左环
			{
				positionReal = 40;
			}
						
			if (g_intencoderALL >= (int)r_distance)//入环完毕
			{
				track_route_status = 2;
				g_intencoderALL = 0;
				g_SpeedPoint = SPEED_ROUNDABOUT;
			}
		}
	}
	else if (track_type == 3 && track_route_status == 2)//环岛内部
	{
		positionReal = position;
		g_SpeedPoint = SPEED_ROUNDABOUT;
	}
	else if (track_type == 3 && track_route_status == 3)//圆环出环
	{
		g_SpeedPoint = SPEED_ROUNDABOUT;
		g_intencoderALL += (g_EncoderLeft + g_EncoderRight) / 2;
		
		if (g_intencoderALL <= 2500)//第一阶段打死出环
		{
			if (track_route == 1)//右环
			{
				positionReal = -50;
			}
			else if (track_route == 2)//左环
			{
				positionReal = 50;
			}
		}
		else//第二阶段直走
		{
			positionReal = 0;
			
			if (g_intencoderALL >= 4000)//出环完毕
			{
				track_type = 0;
				track_route = 0;
				track_route_status = 0;
				
				g_intencoderALL = 0;
			}
		}
	}
	else if (track_type == 4)//深度加速模式
	{
		positionReal = position;
		g_SpeedPoint = SPEED_SPEED;
	}


	//按键发车，发车后才开始跑PID，防止PID跑飞
	if (Go_Flag == 1)
	{
		Filtered_GyroZ = Kalman_Update(&imu693_kf, Gyro_Z);//对Gyro_Z进行卡尔曼滤波
		
		Turn_Pid = Pid_Pos_GyroZ(&TurnPID, positionReal, Filtered_GyroZ);
		
		Kalman_Predict(&imu693_kf, Turn_Pid);//更新卡尔曼滤波器的值

		//计算速度环pid

		Velo_Pid = Pid_Inc_Ff(&SpeedPID, g_EncoderAverage, g_SpeedPoint);

		
		//转int
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















//UART1中断
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
        //程序自动下载
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

//UART2中断
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
		//接收数据寄存器为：S2BUF
		gps_uart_callback(S2BUF);
	}
}


//UART3中断
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
		//接收数据寄存器为：S3BUF

	}
}


//UART4中断
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

		//接收数据寄存器为：S4BUF;
		if(wireless_module_uart_handler != NULL)
		{
			// 该函数为函数指针
			// 再初始化无线模块的时候，设置该函数地址
			wireless_module_uart_handler(S4BUF);
		}
	}
}

#define LED P52
void INT0_Isr() interrupt 0
{
	LED = 0;	//点亮LED
}
void INT1_Isr() interrupt 2
{

}
void INT2_Isr() interrupt 10
{
	INT2_CLEAR_FLAG;  //清除中断标志
}
void INT3_Isr() interrupt 11
{
	INT3_CLEAR_FLAG;  //清除中断标志
}

void INT4_Isr() interrupt 16
{
	INT4_CLEAR_FLAG;  //清除中断标志
}



void TM2_Isr() interrupt 12
{
	TIM2_CLEAR_FLAG;  //清除中断标志
}
void TM3_Isr() interrupt 19
{
	TIM3_CLEAR_FLAG; //清除中断标志
	
}

void TM4_Isr() interrupt 20
{
	TIM4_CLEAR_FLAG; //清除中断标志
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