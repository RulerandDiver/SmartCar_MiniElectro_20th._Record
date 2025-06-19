#ifndef __CONTROL_H_
#define __CONTROL_H_

#include "headfile.h"
#include "Key.h"
typedef struct PID
{
	int16 iError;  // 本次偏差
    int16 LastError;  // 上次偏差
    int16 PrevError;  // 上上次偏差
    double KP;
    double KI;
    double KD;
	double KP2;
	double GKD;
}PID;

#define MOTOR_MAX 4000

//定义按键引脚
#define KEY1_PIN    P36
#define KEY2_PIN    P37
#define KEY3_PIN    P35
#define KEY4_PIN    P71
//定义拨码开关引脚
#define SW1_PIN     P67
#define SW2_PIN     P66
//限幅宏函数
#define Fun_Limit_ab(x ,a , b) ((x) < (a) ? (a) : ( (x) > (b) ? (b) : (x)))
//下方extern为变量解释，仅在main文件中debug使用

extern PID Left_MOTOR_PID, Right_MOTOR_PID, Turn_PID_ele;  // 三个结构体变量
extern float Loc_kp;
extern float Loc_kd;

extern unsigned char run;

extern int16 adc_max[]; //归一化
extern int16 adc_min[]; 
extern int16 L, L2, M, R2, R;  			 		   // 电感值
extern double eleValue;  // 差比和

extern double   Turn_ele[4];  // 转向环 PD 参数
extern double eleOut;		// 转向环输出值

extern double Left_MOTOR[3];   // 左轮速度环 PID 参数
extern double Right_MOTOR[3];  // 右轮速度环 PID 参数

extern double encoder_L, encoder_R;								// 左右轮编码器数据
extern double LEFT_MOTOR_Duty, RIGHT_MOTOR_Duty; 				// 左右电机占空比
extern double Left_High_Speed, Right_High_Speed, High_Speed;	// 左右轮目标速度、基础目标速度

extern uint16 Bat_Vol;//电池电压

extern double Distance_Int;
extern bit Encoder_Int;

extern bit Gyro_Int;
extern float Gyro_z;
/**********************************按键检测变量******************************************/
//开关标志位
extern uint8 key1_flag;
extern uint8 key2_flag;
extern uint8 key3_flag;
extern uint8 key4_flag;
//拨码开关状态变量
extern uint8 sw1_status;
extern uint8 sw2_status;
/*****************************************************************************************************/
//小车运行状态变量
extern uint8 running;

/**********************************函数区******************************************/
void Dir_Control();		// 转向控制函数
void Motor_Control();  	// 电机控制函数
void Ultima_Control();  // 综合控制函数
void ADC_Measure(); //电感测量及归一化
void KEY_Scan(void);//按键检测
void Ultima_Control(void);
float absolute(float error2);

void huandao(void);
double range_protect(double duty,double min,double max);

double PID_Realize(PID*sptr, double ActualSpeed, double SetSpeed, double *MOTOR_PID);   // 增量式 PID
double Loc_pid(PID*sptr,double error,double *Turn_PID);
void Key_Pro();
/*****************************************************************************************************/

#endif
