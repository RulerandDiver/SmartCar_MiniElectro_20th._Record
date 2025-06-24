/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897(已满)  三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		adc
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ790875685)
 * @version    		查看doc内version文件 版本说明
 * @Software 		MDK FOR C251 V5.60
 * @Target core		STC32G12K128
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-4-14
 ********************************************************************************************************************/


#include "zf_adc.h"
#include "intrins.h"
#include "SEEKFREE_OLED.h"

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADC初始化
//  @param      adcn            选择ADC通道
//  @param      speed      		ADC时钟频率
//  @return     void
//  Sample usage:               adc_init(ADC_P10,ADC_SYSclk_DIV_2);//初始化P1.0为ADC功能,ADC时钟频率：SYSclk/2
//-------------------------------------------------------------------------------------------------------------------
void adc_init(ADCN_enum adcn,ADC_SPEED_enum speed)
{
	ADC_CONTR |= 1<<7;				//1 ：打开 ADC 电源
	
	ADC_CONTR &= (0xF0);			//清除ADC_CHS[3:0] ： ADC 模拟通道选择位
	ADC_CONTR |= adcn;
	
	if((adcn >> 3) == 1) //P0.0
	{
		//IO口需要设置为高阻输入
		P0M0 &= ~(1 << (adcn & 0x07));
		P0M1 |= (1 << (adcn & 0x07));
	}
	else if((adcn >> 3) == 0) //P1.0	
	{
		//IO口需要设置为高阻输入
		P1M0 &= ~(1 << (adcn & 0x07));
	    P1M1 |= (1 << (adcn & 0x07));
	}

	ADCCFG |= speed&0x0F;			//ADC时钟频率SYSclk/2/speed&0x0F;
	
	ADCCFG |= 1<<5;					//转换结果右对齐。 ADC_RES 保存结果的高 2 位， ADC_RESL 保存结果的低 8 位。
}



//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADC转换一次
//  @param      adcn            选择ADC通道
//  @param      resolution      分辨率
//  @return     void
//  Sample usage:               adc_convert(ADC_P10, ADC_10BIT);
//-------------------------------------------------------------------------------------------------------------------
uint16 adc_once(ADCN_enum adcn,ADCRES_enum resolution)
{
	uint16 adc_value;
	static uint16 last_adc_value=0;
	uint16 count=0;
	bit time_out;
	ADC_CONTR &= (0xF0);			//清除ADC_CHS[3:0] ： ADC 模拟通道选择位
	ADC_CONTR |= adcn;
	
	ADC_CONTR |= 0x40;  			// 启动 AD 转换
	while (!(ADC_CONTR & 0x20) )  	// 查询 ADC 完成标志
	{
		if(++count >= 500) ADC_CONTR |= 0x20,time_out=1;
	}
	ADC_CONTR &= ~0x20;  			// 清完成标志
	
	if(time_out)
	{
		time_out=0;
		//超时，转换失败，返回上次adc值,防止卡死
		ADC_RES = 0;
		ADC_RESL = 0;
		return last_adc_value;
	}
	
	adc_value = ADC_RES;  			//存储 ADC 的 12 位结果的高 4 位
	adc_value <<= 8;
	adc_value |= ADC_RESL;  		//存储 ADC 的 12 位结果的低 8 位
	
	ADC_RES = 0;
	ADC_RESL = 0;
	
	adc_value >>= resolution;		//取多少位
	last_adc_value = adc_value;

	return adc_value;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief     ADC初始化
//  @param      adcn            选择ADC通道
//  @param      resolution      分辨率
//  @return     void
//  Sample usage:               adc_convert(ADC_P10, ADC_10BIT);
//-------------------------------------------------------------------------------------------------------------------
void adc_init_5(ADC_SPEED_enum resolution)
{
	adc_init(ADC_P10, resolution);//左竖
	adc_init(ADC_P17, resolution);//左竖
	adc_init(ADC_P03, resolution);//中
	adc_init(ADC_P01, resolution);//右垂
	adc_init(ADC_P02, resolution);//右竖
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADC采集N次
//  @param      adcn            选择ADC通道
//  @param      resolution      分辨率
//  @return     void
//  Sample usage:               adc_convert(ADC_P10, ADC_10BIT);
//获取10次ADC1采样值中值平均滤波法
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
        //2.取中值
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
*  函数名称：void ADC_Voltage(void)
*  功能说明：电压测量测试程序
*  参数说明：无
*  函数返回：无
*  修改时间：2020年3月10日
*  备    注：P00脚测量电压
*************************************************************************/
uint16 ADC_Voltage(void)
{
	uint16 ad_result = 0;           //引脚电压
	uint16 battery_voltage = 0;     //电池电压
	uint32 temp=0;
	uint8 delay;

	ad_result = adc_once(ADC_P00, ADC_12BIT);//不能作过多处理，滤波算法尽量简单，因为算不完被打断会导致程序卡死
	for(delay=0;delay<10;delay++);
	temp = (((uint32)ad_result * 3300) / 4096);  //计算出当前adc引脚的电压 计算公式为 ad_result*VCC/ADC分辨率    VCC单位为mv
	battery_voltage =  temp * 5.5;//根据引脚电压  和分压电阻的阻值计算电池电压 计算公司为   引脚电压*(R2+R3)/R3   R3为接地端电阻	
		
	return battery_voltage;
}


















