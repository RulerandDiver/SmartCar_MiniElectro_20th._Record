/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897(����)  ��Ⱥ��824575535
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		adc
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ790875685)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		MDK FOR C251 V5.60
 * @Target core		STC32G12K128
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-4-14
 ********************************************************************************************************************/


#include "zf_adc.h"
#include "intrins.h"
#include "SEEKFREE_OLED.h"

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADC��ʼ��
//  @param      adcn            ѡ��ADCͨ��
//  @param      speed      		ADCʱ��Ƶ��
//  @return     void
//  Sample usage:               adc_init(ADC_P10,ADC_SYSclk_DIV_2);//��ʼ��P1.0ΪADC����,ADCʱ��Ƶ�ʣ�SYSclk/2
//-------------------------------------------------------------------------------------------------------------------
void adc_init(ADCN_enum adcn,ADC_SPEED_enum speed)
{
	ADC_CONTR |= 1<<7;				//1 ���� ADC ��Դ
	
	ADC_CONTR &= (0xF0);			//���ADC_CHS[3:0] �� ADC ģ��ͨ��ѡ��λ
	ADC_CONTR |= adcn;
	
	if((adcn >> 3) == 1) //P0.0
	{
		//IO����Ҫ����Ϊ��������
		P0M0 &= ~(1 << (adcn & 0x07));
		P0M1 |= (1 << (adcn & 0x07));
	}
	else if((adcn >> 3) == 0) //P1.0	
	{
		//IO����Ҫ����Ϊ��������
		P1M0 &= ~(1 << (adcn & 0x07));
	    P1M1 |= (1 << (adcn & 0x07));
	}

	ADCCFG |= speed&0x0F;			//ADCʱ��Ƶ��SYSclk/2/speed&0x0F;
	
	ADCCFG |= 1<<5;					//ת������Ҷ��롣 ADC_RES �������ĸ� 2 λ�� ADC_RESL �������ĵ� 8 λ��
}



//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADCת��һ��
//  @param      adcn            ѡ��ADCͨ��
//  @param      resolution      �ֱ���
//  @return     void
//  Sample usage:               adc_convert(ADC_P10, ADC_10BIT);
//-------------------------------------------------------------------------------------------------------------------
uint16 adc_once(ADCN_enum adcn,ADCRES_enum resolution)
{
	uint16 adc_value;
	static uint16 last_adc_value=0;
	uint16 count=0;
	bit time_out;
	ADC_CONTR &= (0xF0);			//���ADC_CHS[3:0] �� ADC ģ��ͨ��ѡ��λ
	ADC_CONTR |= adcn;
	
	ADC_CONTR |= 0x40;  			// ���� AD ת��
	while (!(ADC_CONTR & 0x20) )  	// ��ѯ ADC ��ɱ�־
	{
		if(++count >= 500) ADC_CONTR |= 0x20,time_out=1;
	}
	ADC_CONTR &= ~0x20;  			// ����ɱ�־
	
	if(time_out)
	{
		time_out=0;
		//��ʱ��ת��ʧ�ܣ������ϴ�adcֵ,��ֹ����
		ADC_RES = 0;
		ADC_RESL = 0;
		return last_adc_value;
	}
	
	adc_value = ADC_RES;  			//�洢 ADC �� 12 λ����ĸ� 4 λ
	adc_value <<= 8;
	adc_value |= ADC_RESL;  		//�洢 ADC �� 12 λ����ĵ� 8 λ
	
	ADC_RES = 0;
	ADC_RESL = 0;
	
	adc_value >>= resolution;		//ȡ����λ
	last_adc_value = adc_value;

	return adc_value;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief     ADC��ʼ��
//  @param      adcn            ѡ��ADCͨ��
//  @param      resolution      �ֱ���
//  @return     void
//  Sample usage:               adc_convert(ADC_P10, ADC_10BIT);
//-------------------------------------------------------------------------------------------------------------------
void adc_init_5(ADC_SPEED_enum resolution)
{
	adc_init(ADC_P10, resolution);//����
	adc_init(ADC_P17, resolution);//����
	adc_init(ADC_P03, resolution);//��
	adc_init(ADC_P01, resolution);//�Ҵ�
	adc_init(ADC_P02, resolution);//����
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADC�ɼ�N��
//  @param      adcn            ѡ��ADCͨ��
//  @param      resolution      �ֱ���
//  @return     void
//  Sample usage:               adc_convert(ADC_P10, ADC_10BIT);
//��ȡ10��ADC1����ֵ��ֵƽ���˲���
//-------------------------------------------------------------------------------------------------------------------
uint16 ADC_Median_Average(ADCN_enum adcn)
{
  uint16 data1,data2,data3,tmp,sum=0;
  uint16 temp_val=0;
  uint8 delay,num;

  for(num=0;num<10;num++)
  {
     data1 = adc_once(adcn,ADC_12BIT);
       for(delay=0;delay<10;delay++);
     data2 = adc_once(adcn,ADC_12BIT);
       for(delay=0;delay<10;delay++);
     data3 = adc_once(adcn,ADC_12BIT);
        //2.ȡ��ֵ
        if (data1 > data3)
        {
          tmp = data1; data1 = data2; data2 = tmp;
         }
         if (data3 > data2)
           tmp = data2;
         else if(data3 > data1)
           tmp = data3;
         else
           tmp = data1;
         sum+=tmp;

    }
    temp_val=sum/10;

   return temp_val;
}

/*************************************************************************
*  �������ƣ�void ADC_Voltage(void)
*  ����˵������ѹ�������Գ���
*  ����˵������
*  �������أ���
*  �޸�ʱ�䣺2020��3��10��
*  ��    ע��P00�Ų�����ѹ
*************************************************************************/
uint16 ADC_Voltage(void)
{
	uint16 ad_result = 0;           //���ŵ�ѹ
	uint16 battery_voltage = 0;     //��ص�ѹ
	uint32 temp=0;
	uint8 delay;

	ad_result = adc_once(ADC_P00, ADC_12BIT);//���������ദ���˲��㷨�����򵥣���Ϊ�㲻�걻��ϻᵼ�³�����
	for(delay=0;delay<10;delay++);
	temp = (((uint32)ad_result * 3300) / 4096);  //�������ǰadc���ŵĵ�ѹ ���㹫ʽΪ ad_result*VCC/ADC�ֱ���    VCC��λΪmv
	battery_voltage =  temp * 5.5;//�������ŵ�ѹ  �ͷ�ѹ�������ֵ�����ص�ѹ ���㹫˾Ϊ   ���ŵ�ѹ*(R2+R3)/R3   R3Ϊ�ӵض˵���	
		
	return battery_voltage;
}


















