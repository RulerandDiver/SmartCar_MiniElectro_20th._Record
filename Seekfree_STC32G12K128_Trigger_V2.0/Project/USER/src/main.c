  /*
 * **************************************************************************
 * ********************                                  ********************
 * ********************      COPYRIGHT INFORMATION       ********************
 * ********************                                  ********************
 * **************************************************************************
 *                                                                          *
 *                                   _oo8oo_                                *
 *                                  o8888888o                               *
 *                                  88" . "88                               *
 *                                  (| -_- |)                               *
 *                                  0\  =  /0                               *
 *                                ___/'==='\___                             *
 *                              .' \\|     |// '.                           *
 *                             / \\|||  :  |||// \                          *
 *                            / _||||| -:- |||||_ \                         *
 *                           |   | \\\  -  /// |   |                        *
 *                           | \_|  ''\---/''  |_/ |                        *
 *                           \  .-\__  '-'  __/-.  /                        *
 *                         ___'. .'  /--.--\  '. .'___                      *
 *                      ."" '<  '.___\_<|>_/___.'  >' "".                   *
 *                     | | :  `- \`.:`\ _ /`:.`/ -`  : | |                  *
 *                     \  \ `-.   \_ __\ /__ _/   .-` /  /                  *
 *                 =====`-.____`.___ \_____/ ___.`____.-`=====              *
 *                                   `=---=`                                *
 * **************************************************************************
 * ********************                                  ********************
 * ********************      	佛祖保佑 永远无BUG		  ********************
 * ********************                                  ********************
 * **************************************************************************
 */
 
#include "headfile.h"

void All_Init();
void main(void)
{
	board_init();			// 初始化寄存器,勿删除此句代码。
	
	// 此处编写用户代码(例如：外设初始化代码等)

	//外设初始化
	All_Init();

    while(1)
	{
		
		Key_Task();//按键处理
		
		// 获取滤波后的ADC数据		
		mid_filter();      // 使用中位值滤波获取电感数据

		// 归一化电感数组·
		normalize_sensors();
		
		//检查电磁保护
		protection_flag = check_electromagnetic_protection();
		
		// 计算位置偏差
		position = calculate_position_improved();
		
		display_electromagnetic_data();
		
		
		
		//在OLED屏幕上显示，需要初始化OLED屏幕，才能使用。
//		oled_p6x8str_spi(0,0,"Int:");	
//		oled_int16_spi(6*8,0,Distance_Int);
//		

//		
//		oled_p6x8str_spi(0,3,"Vo:");	
//		oled_uint16_spi(6*8,3,Bat_Vol);
//		

//		
//		oled_p6x8str_spi(0,6,"ER:");	
//		oled_int16_spi(6*8,6,eleValue); oled_int16_spi(11*8,6,eleOut);
//		
//		oled_p6x8str_spi(0,7,"mpu:");	
//		oled_printf_float_spi(6*8,7,Gyro_z,4,1);
		
		//printf("Voltage:%u\r\n",ad_result);
		//if(++count == 60) count=0,P64 = ~P64;

    }
}

void All_Init()
{
	//OLED
	oled_init_spi();
    oled_fill_spi(0x00);
	//oled_p6x8str_spi(0,0,"Initializing...:");
	
	//adc_init_5(ADC_SYSclk_DIV_2);//ADC
	electromagnetic_init();
	adc_init(ADC_P00, ADC_SYSclk_DIV_32);//电压检测
	
	
	Motor_Init();//Motor
	Encoder_Init();

	gpio_mode(P6_4,GPO_PP);
	gpio_mode(P6_5,GPO_PP);

	//Kp,Ki,Kd,Kf,I_Limit,O_Limit,GKd,Kp2
	Pid_Init(&SpeedPID,80.0f, 1.0f, 0.0f, 0.0f, 5000.0f, 6000.0f,0.0f,0.0f);
	Pid_Init(&TurnPID,3.3f, 0.0f, 0.0f, 0.0f, 0.0f, 6000.0f,0.02,0.01);

	LowPass_init(&leftSpeedFilt, 0.556);   //初始化低通滤波器
	LowPass_init(&rightSpeedFilt, 0.556);
	Kalman_Init(&imu693_kf, 0.98, 0.02, imu693kf_Q, imu693kf_R, 0.0);
	//gpio_mode(P2_4,GPO_PP);
	//P24=0;

	
	//mpu6050_init();
	

	//pit_timer_ms(TIM_1,10);  // 定时器1中断初始化,用于跑电感、编码器、PID计算、电机驱动，暂定2ms，若不正常，则测试各个部分运行时间，更改周期
	pit_timer_ms(TIM_0,10);//按键检测、蜂鸣器、tick
	pit_timer_ms(TIM_1, 5);//编码器、陀螺仪、元素标志位判断、pwm赋值、速度决策
	
	delay_ms(100); // 延时等待系统稳定
}



















