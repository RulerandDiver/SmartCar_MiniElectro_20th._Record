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
 * ********************      	���汣�� ��Զ��BUG		  ********************
 * ********************                                  ********************
 * **************************************************************************
 */
 
#include "headfile.h"

void All_Init();
void main(void)
{
	board_init();			// ��ʼ���Ĵ���,��ɾ���˾���롣
	
	// �˴���д�û�����(���磺�����ʼ�������)

	//�����ʼ��
	All_Init();

    while(1)
	{
		
		Key_Task();//��������
		
		// ��ȡ�˲����ADC����		
		mid_filter();      // ʹ����λֵ�˲���ȡ�������

		// ��һ��������顤
		normalize_sensors();
		
		//����ű���
		protection_flag = check_electromagnetic_protection();
		
		// ����λ��ƫ��
		position = calculate_position_improved();
		
		display_electromagnetic_data();
		
		
		
		//��OLED��Ļ����ʾ����Ҫ��ʼ��OLED��Ļ������ʹ�á�
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
	adc_init(ADC_P00, ADC_SYSclk_DIV_32);//��ѹ���
	
	
	Motor_Init();//Motor
	Encoder_Init();

	gpio_mode(P6_4,GPO_PP);
	gpio_mode(P6_5,GPO_PP);

	//Kp,Ki,Kd,Kf,I_Limit,O_Limit,GKd,Kp2
	Pid_Init(&SpeedPID,80.0f, 1.0f, 0.0f, 0.0f, 5000.0f, 6000.0f,0.0f,0.0f);
	Pid_Init(&TurnPID,3.3f, 0.0f, 0.0f, 0.0f, 0.0f, 6000.0f,0.02,0.01);

	LowPass_init(&leftSpeedFilt, 0.556);   //��ʼ����ͨ�˲���
	LowPass_init(&rightSpeedFilt, 0.556);
	Kalman_Init(&imu693_kf, 0.98, 0.02, imu693kf_Q, imu693kf_R, 0.0);
	//gpio_mode(P2_4,GPO_PP);
	//P24=0;

	
	//mpu6050_init();
	

	//pit_timer_ms(TIM_1,10);  // ��ʱ��1�жϳ�ʼ��,�����ܵ�С���������PID���㡢����������ݶ�2ms����������������Ը�����������ʱ�䣬��������
	pit_timer_ms(TIM_0,10);//������⡢��������tick
	pit_timer_ms(TIM_1, 5);//�������������ǡ�Ԫ�ر�־λ�жϡ�pwm��ֵ���ٶȾ���
	
	delay_ms(100); // ��ʱ�ȴ�ϵͳ�ȶ�
}



















