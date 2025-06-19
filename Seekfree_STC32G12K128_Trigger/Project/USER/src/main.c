/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897(����)  ��Ⱥ��824575535
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		main
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ790875685)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		MDK FOR C251 V5.60
 * @Target core		STC32G12K128
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-12-18
 ********************************************************************************************************************/

#include "headfile.h"

/*
 * ϵͳƵ�ʣ��ɲ鿴board.h�е� FOSC �궨���޸ġ�
 * board.h�ļ���FOSC��ֵ����Ϊ0,������Զ�����ϵͳƵ��Ϊ33.1776MHZ
 * ��board_init��,�Ѿ���P54��������Ϊ��λ
 * �����Ҫʹ��P54����,������board.c�ļ��е�board_init()������ɾ��SET_P54_RESRT����
 */




void All_Init();
void main()
{
	board_init();			// ��ʼ���Ĵ���,��ɾ���˾���롣
	
	// �˴���д�û�����(���磺�����ʼ�������)

	//�����ʼ��
	All_Init();

    while(1)
	{
		Key_Pro();//��������
		//��OLED��Ļ����ʾ����Ҫ��ʼ��OLED��Ļ������ʹ�á�
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
	

	pit_timer_ms(TIM_1,10);  // ��ʱ��1�жϳ�ʼ��,�����ܵ�С���������PID���㡢����������ݶ�2ms����������������Ը�����������ʱ�䣬��������
}



















