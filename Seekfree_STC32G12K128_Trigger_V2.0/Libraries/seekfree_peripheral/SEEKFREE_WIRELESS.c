/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		逐飞科技无线转串口模块
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		MDK FOR C251 V5.60
 * @Target core		STC32G12K128
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-03-27
 * @note		
					接线定义：
					------------------------------------ 
					无线转串口      单片机                        
    				RX              查看SEEKFREE_WIRELESS.h文件中的WIRELESS_UART_INDEX_TX宏定义
    				TX              查看SEEKFREE_WIRELESS.h文件中的WIRELESS_UART_INDEX_RX宏定义
    				RTS             查看SEEKFREE_WIRELESS.h文件中的RTS_PIN宏定义
    				CMD             查看SEEKFREE_WIRELESS.h文件中的CMD_PIN宏定义
					------------------------------------ 
 ********************************************************************************************************************/


#include "SEEKFREE_WIRELESS.h"
#include "zf_uart.h"
#include "zf_fifo.h"
#include "seekfree_assistant.h"

static  fifo_struct     edata wireless_uart_fifo;
static  uint8           edata wireless_uart_buffer[WIRELESS_BUFFER_SIZE];  // 数据存放数组

//-------------------------------------------------------------------------------------------------------------------
//  @brief      无线转串口模块回调函数
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
void wireless_uart_callback(uint8 dat)
{
    // 接到一个字节后单片机将会进入串口中断，通过在此处读取dat可以取走数据
    fifo_write_buffer(&wireless_uart_fifo, &dat, 1);       // 存入 FIFO
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      无线转串口模块发送一个字节
//  @param      dat 	需要发送的字节长度
//  @return     uint8	0成功，1失败					
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
uint8 wireless_uart_send_byte(uint8 dat)
{
	if(WIRELESS_RTS_PIN == 1)  
	{
		return 1;//模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
	}
	
	uart_putchar(WIRELESS_UART_INDEX, dat);
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      无线转串口模块 发送函数
//  @param      buff        需要发送的数据地址
//  @param      len         发送长度
//  @return     uint32      剩余未发送的字节数   
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
uint32 wireless_uart_send_buff(uint8 *buff, uint32 len)
{
    while(len>30)
    {
        if(WIRELESS_RTS_PIN == 1)  
        {
            return len;//模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
        }
        //while(RTS_PIN);  //如果RTS为低电平，则继续发送数据
        uart_putbuff(WIRELESS_UART_INDEX,buff,30);

        buff += 30; //地址偏移
        len -= 30;//数量
    }
    
    if(WIRELESS_RTS_PIN == 1)  
    {
        return len;//模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
    }
    //while(WIRELESS_RTS_PIN);  //如果RTS为低电平，则继续发送数据
    uart_putbuff(WIRELESS_UART_INDEX,buff,len);//发送最后的数据
    
    return 0;
}



//-------------------------------------------------------------------------------------------------------------------
//  @brief      无线转串口模块 读取函数
//  @param      buff            存储的数据地址
//  @param      len             长度
//  @return     uint32          实际读取字节数
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 wireless_uart_read_buff (uint8 *buff, uint32 len)
{
    uint32 edata data_len = len;
    fifo_read_buffer(&wireless_uart_fifo, buff, &data_len, FIFO_READ_AND_CLEAN);
    return data_len;
}




//-------------------------------------------------------------------------------------------------------------------
//  @brief      无线转 串口模块初始化
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
void wireless_uart_init(void)
{
    WIRELESS_RTS_PIN = 0;
    wireless_type = WIRELESS_SI24R1;
	wireless_module_uart_handler = wireless_uart_callback;
    //本函数使用的波特率为115200，为无线转串口模块的默认波特率，如需其他波特率请自行配置模块并修改串口的波特率
    fifo_init(&wireless_uart_fifo, FIFO_DATA_8BIT, wireless_uart_buffer, WIRELESS_BUFFER_SIZE);
	uart_init(WIRELESS_UART_INDEX, WIRELESS_UART_RX_PIN, WIRELESS_UART_TX_PIN, WIRELESS_UART_BAUD, WIRELESS_TIMER_N);	//初始化串口    
    
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     助飞助手发送函数
// 参数说明     *buff           需要发送的数据地址
// 参数说明     length          需要发送的长度
// 返回参数     uint32          剩余未发送数据长度
// 使用示例
//-------------------------------------------------------------------------------------------------------------------
uint32 seekfree_assistant_transfer_callback   (const uint8 *buff, uint32 length)
{
	uart_putbuff(WIRELESS_UART_INDEX, buff, length);
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      无线串口波形观测
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
void Test_Wireless_Oscilloscope(void)
{
	// 初始化无线转串口
    wireless_uart_init();
	// 设置函数指针
	seekfree_assistant_transfer = seekfree_assistant_transfer_callback;
	
	// 设置数据
	seekfree_assistant_oscilloscope_data.dat[0] = 1;
	seekfree_assistant_oscilloscope_data.dat[1] = 1;
	seekfree_assistant_oscilloscope_data.dat[2] = 1;
	seekfree_assistant_oscilloscope_data.dat[3] = 1;
	
	seekfree_assistant_oscilloscope_data.dat[4] = 1;
	// 需要传输四个通道数据
	seekfree_assistant_oscilloscope_data.channel_num = 5;
	
	seekfree_assistant_oscilloscope_send(&seekfree_assistant_oscilloscope_data);
    
}


