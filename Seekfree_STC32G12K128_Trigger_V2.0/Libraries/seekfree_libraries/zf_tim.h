/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897(����)  ��Ⱥ��824575535
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		ctime
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ790875685)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		MDK FOR C251 V5.60
 * @Target core		STC32G12K128
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-4-14
 ********************************************************************************************************************/

#ifndef __ZF_TIM_H
#define __ZF_TIM_H
#include "common.h"


#define  LEFT_DIR    P46
#define  RIGHT_DIR   P77

#define TIM2_CLEAR_FLAG		AUXINTIF &= ~0x01; 
#define TIM3_CLEAR_FLAG		AUXINTIF &= ~0x02; // ���жϱ�־
#define TIM4_CLEAR_FLAG		AUXINTIF &= ~0x04; // ���жϱ�־

//��ö�ٶ��岻�����û��޸�
typedef enum    // ö��ADCͨ��
{
    CTIM0_P34=0,    
	CTIM1_P35,    	
	CTIM2_P12,     	
	CTIM3_P04,
	CTIM4_P06,
}CTIMN_enum;


//��ö�ٶ��岻�����û��޸�
typedef enum    // ö��ADCͨ��
{
    TIM_0,    
	TIM_1,    	
	TIM_2,     	
	TIM_3,
	TIM_4,
}TIMN_enum;

typedef struct
{
	int count;
	int encoderlast;
}Encoder_tt;






extern Encoder_tt EncoderL, EncoderR;

extern int g_encoleft_init, g_encoright_init;
extern int g_EncoderLeft, g_EncoderRight, g_EncoderAverage;
extern int g_intencoderALL, g_intencoderL, g_intencoderR;


void   ctimer_count_init(CTIMN_enum tim_n);
void   ctimer_count_clean(CTIMN_enum tim_n);
uint16 ctimer_count_read(CTIMN_enum tim_n);

void pit_timer_ms(TIMN_enum tim_n,uint16 time_ms);

void Encoder_Init(void);
int get_left_encoder(void);
int get_right_encoder(void);
int encoder_debounce(Encoder_tt *instance, int encoder);


#endif
