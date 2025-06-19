/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897(已满)  三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ790875685)
 * @version    		查看doc内version文件 版本说明
 * @Software 		MDK FOR C251 V5.60
 * @Target core		STC32G12K128
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-12-18
 ********************************************************************************************************************/

#include "headfile.h"

/*
 * 系统频率，可查看board.h中的 FOSC 宏定义修改。
 * board.h文件中FOSC的值设置为0,则程序自动设置系统频率为33.1776MHZ
 * 在board_init中,已经将P54引脚设置为复位
 * 如果需要使用P54引脚,可以在board.c文件中的board_init()函数中删除SET_P54_RESRT即可
 */




void All_Init();
void main()
{
	board_init();			// 初始化寄存器,勿删除此句代码。
	
	// 此处编写用户代码(例如：外设初始化代码等)

	//外设初始化
	All_Init();

    while(1)
	{
		Key_Pro();//按键处理
		//在OLED屏幕上显示，需要初始化OLED屏幕，才能使用。
		oled_p6x8str_spi(0,0,"Int:");	
		oled_int16_spi(6*8,0,Distance_Int);
		
		oled_p6x8str_spi(0,1,"L:");	
		oled_uint16_spi(6*8,1,L);
		oled_p6x8str_spi(0,2,"L2:");	
		oled_uint16_spi(6*8,2,L2);
		
//		oled_p6x8str_spi(0,3,"M:");	
//		oled_uint16_spi(6*8,3,M);
		
		oled_p6x8str_spi(0,3,"Vo:");	
		oled_uint16_spi(6*8,3,Bat_Vol);
		
		oled_p6x8str_spi(0,4,"R2:");	
		oled_uint16_spi(6*8,4,R2);
		
		oled_p6x8str_spi(0,5,"R:");	
		oled_uint16_spi(6*8,5,R);
		
		oled_p6x8str_spi(0,6,"ER:");	
		oled_int16_spi(6*8,6,eleValue); oled_int16_spi(11*8,6,eleOut);
		
		oled_p6x8str_spi(0,7,"mpu:");	
		oled_printf_float_spi(6*8,7,Gyro_z,4,1);
		
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
	
	adc_init_5(ADC_SYSclk_DIV_2);//ADC
	adc_init(ADC_P00, ADC_SYSclk_DIV_32);//
	
	Motor_Init();//Motor
	ctimer_count_init(CTIM4_P06);//Encoder
	ctimer_count_init(CTIM3_P04);
	gpio_mode(P6_4,GPO_PP);
	gpio_mode(P5_2,GPO_PP);
	gpio_mode(P6_5,GPO_PP);
	//gpio_mode(P2_4,GPO_PP);
	//P24=0;
	//pwm_init(PWM0_P24, 100, 5000);
	
	mpu6050_init();
	

	pit_timer_ms(TIM_1,10);  // 定时器1中断初始化,用于跑电感、编码器、PID计算、电机驱动，暂定2ms，若不正常，则测试各个部分运行时间，更改周期
}



















