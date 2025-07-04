#include "Control.h"

PID Left_MOTOR_PID, Right_MOTOR_PID, Turn_PID_ele;  // 三个结构体变量

unsigned char run = 0;  // 电机启停标志位


int16 adc_max[5] = {3560,3555,3560,3570,3570}; 
int16 adc_min[5] = {88,4,1,6,83}; 

/*电感*/
//short data 不能extern后在其他文件调用.比如oled显示时，不能如此使用
int16 L=0, L2=0, M=0, R2=0, R=0;
double eleValue=0;	// 差比和
double eleOut = 0;		
uint8 Left_Vector=0,Right_Vector=0;
int16 eleSum=0,eleDiff=0;

/*电压*/
uint16 Bat_Vol=0;
/*转向环*/
//double  Turn_ele[4]={4.1 , 0.03 , 0 , 0};//KP,GKD,KP2,KD	
double  Turn_ele[4]={3.3 , 0.02 , 0.01 , 0};//KP,GKD,KP2,KD	
			

/*速度环*/
double Left_MOTOR[3] =  {80, 1, 0};   
double Right_MOTOR[3] = {80, 1, 0};  

/*电机和编码器*/
double encoder_L, encoder_R;							
double LEFT_MOTOR_Duty, RIGHT_MOTOR_Duty;				
double Left_High_Speed=0, Right_High_Speed=0;

/*编码器积分*/
bit Encoder_Int=0;
double Distance=0,Distance_Int=0;

/*陀螺仪积分*/
bit Gyro_Int=0;
float Gyro_z=0;

/*苯环*/
short data youhuandao_flag1=0,youhuandao_flag2_s=0,youhuandao_flag3_s=0,youhuandaoleijia=0;


/**********************************按键检测变量******************************************/
//拨码开关状态变量
uint8 sw1_status;
uint8 sw2_status;

//开关状态变量
uint8 key1_status = 1;
uint8 key2_status = 1;
uint8 key3_status = 1;
uint8 key4_status = 1;

//上一次开关状态变量
uint8 key1_last_status;
uint8 key2_last_status;
uint8 key3_last_status;
uint8 key4_last_status;

//开关标志位
uint8 key1_flag;
uint8 key2_flag;
uint8 key3_flag;
uint8 key4_flag;

uint8 running=0;
/*****************************************************************************************************/
/**
 * @brief 限幅保护函数
 */
double range_protect(double duty, double min, double max)
{
    return duty >= max ? max : (duty <= min ? min : duty);
}


/**
 * @brief 电机控制函数，实现左右轮速度闭环控制
 */
void Motor_Control()
{
	// 编码器数据在屏幕上已经显示，一定要核对确认：①左右不要搞反  ②正负不要搞反
	encoder_L = Read_Encoder(En_L);  // 左编码器
	encoder_R = Read_Encoder(En_R)*0.5;   // 右编码器
	
	//陀螺仪Z轴获取
	mpu6050_get_gyro();
	
	/* 采集各通道电感值 */
	ADC_Measure();

	/* 差比和计算赛道偏差 */
	if(youhuandao_flag2_s == 1) eleValue = 50;
	else if(youhuandao_flag3_s == 1) eleValue = 10;//防止出圆环多走
	//基础差比和循迹
//	else	eleValue = ((L - R) + (L2 - R2))*100/(L + R + L2 + R2 + 1);	// 差比和の分子
	else eleValue = (eleDiff * 100) / (eleSum + 1);//计算差比和数值 并放大

	eleOut = Loc_pid(&Turn_PID_ele,eleValue,Turn_ele); //位置环
	

	//实际调节后速度减去实际调节后位置偏差减去一绝对值（抑制弯道）
//	LEFT_MOTOR_Duty  += PID_Realize(&Left_MOTOR_PID, encoder_L, Left_High_Speed-eleOut-absolute(eleOut*0.5), Left_MOTOR);
//	RIGHT_MOTOR_Duty += PID_Realize(&Right_MOTOR_PID, encoder_R, Right_High_Speed+eleOut-absolute(eleOut*0.5), Right_MOTOR);
	
	LEFT_MOTOR_Duty  += PID_Realize(&Left_MOTOR_PID, encoder_L, Left_High_Speed-eleOut, Left_MOTOR);
	RIGHT_MOTOR_Duty += PID_Realize(&Right_MOTOR_PID, encoder_R, Right_High_Speed+eleOut, Right_MOTOR);

//	Left_High_Speed  = 65;
//	Right_High_Speed = 65;
//	LEFT_MOTOR_Duty  += PID_Realize(&Left_MOTOR_PID, encoder_L, Left_High_Speed, Left_MOTOR);
//	RIGHT_MOTOR_Duty += PID_Realize(&Right_MOTOR_PID, encoder_R, Right_High_Speed, Right_MOTOR);


	if(LEFT_MOTOR_Duty>0)
	{
			LEFT_MOTOR_Duty = range_protect(LEFT_MOTOR_Duty, 0, MOTOR_MAX);
	}
	else
	{
			LEFT_MOTOR_Duty = range_protect(LEFT_MOTOR_Duty, -MOTOR_MAX, 0);
	}

	if(RIGHT_MOTOR_Duty>0)
	{
			RIGHT_MOTOR_Duty = range_protect(RIGHT_MOTOR_Duty, 0, MOTOR_MAX);
	}
	else
	{
			RIGHT_MOTOR_Duty = range_protect(RIGHT_MOTOR_Duty, -MOTOR_MAX, 0);
	}
	

	Motor_Ctrl((int16)LEFT_MOTOR_Duty,-(int16)RIGHT_MOTOR_Duty);
//	Motor_Ctrl(0,-900);
}


/**
 * @brief 综合控制函数，在中断中调用
 */
void Ultima_Control()
{
//	huandao();
//	if(Encoder_Int)
//	{
//		Distance += encoder_L;
//		Distance_Int = Distance/80;//编码器脉冲数除以实际距离 cm
//	}
//	if(Gyro_Int)
//	{
//		Gyro_z += (mpu6050_gyro_z - 15)*0.00064;//0.00064     360° 除以 陀螺仪轴记 
//	}
	//Dir_Control();  // 转向控制
	Motor_Control();  //电机控制
	//KEY_Scan();//按键检测
	Key_Pro();
}

/**
 * @brief 循迹电感测量
 */
void ADC_Measure()
{
	L  = ADC_Median_Average(ADC_P10);  	
	L2 = ADC_Median_Average(ADC_P17);	
	M  = ADC_Median_Average(ADC_P03);		
	R2 = ADC_Median_Average(ADC_P01);	
	R  = ADC_Median_Average(ADC_P02);		

	L   = (float)(L - adc_min[0])   / (adc_max[0] - adc_min[0])* 100.0f;    //最大值最小值需要经过拟合，而不是简单的根据直道最值敲定
	L2  = (float)(L2 - adc_min[1])  / (adc_max[1] - adc_min[1])* 100.0f;
	M   = (float)(M - adc_min[2])   / (adc_max[2] - adc_min[2])* 100.0f;     
	R2  = (float)(R2 - adc_min[3])  / (adc_max[3] - adc_min[3])* 100.0f;
	R   = (float)(R - adc_min[4])   / (adc_max[4] - adc_min[4])* 100.0f;     

	L =  (L < 0)   ? 0   : L;
	L =  (L > 100) ? 100 : L;
	L2 = (L2 < 0)   ? 0   : L2;
	L2 = (L2 > 100) ? 100 : L2;	
	M =  (M < 0)   ? 0   : M;
	M =  (M > 100) ? 100 : M;
	R2 = (R2 < 0)   ? 0   : R2;
	R2 = (R2 > 100) ? 100 : R2;
	R =  (R < 0)   ? 0   : R;
	R =  (R > 100) ? 100 : R;	
	
	Left_Vector  = Fun_Limit_ab(sqrt(L * L + L2 * L2),0,100);
	Right_Vector = Fun_Limit_ab(sqrt(R * R + R2 * R2),0,100);
	eleSum  = Left_Vector + Right_Vector;
	eleDiff = Left_Vector - Right_Vector;
	
}

void huandao(void)
{ 
	if(M>80 && youhuandaoleijia==0)   P64=0,youhuandaoleijia=1,youhuandao_flag1=1,Encoder_Int=1;
	if(youhuandaoleijia==1 && youhuandao_flag1==1 && Distance_Int>35)		
	{
		
		if(M>0 ) //加辅助判断调节，防止与十字误识别，即1 5电感
			P64=1,youhuandao_flag1=0,Gyro_Int=1,Encoder_Int=0,Distance=0,Distance_Int=0,youhuandao_flag2_s=1;         			  
		else 					youhuandaoleijia=0,youhuandao_flag1=0,Encoder_Int=0,Distance=0,Distance_Int=0;    
	}			
    if(youhuandaoleijia==1 && youhuandao_flag2_s==1 && Gyro_z<-21) P64=0,youhuandao_flag2_s=0;
  	
	  if(youhuandaoleijia==1 && Gyro_z!=0) 
	  {
		if(Gyro_z<-290 && Gyro_z>-300)  youhuandao_flag3_s=1;
		if(Gyro_z<-300) 							  youhuandao_flag3_s=0,youhuandao_flag1=1,Encoder_Int=1,Gyro_Int=0,Gyro_z=0;        
	  }		
//防止出圆环时 再次识别到圆环	  
	  if(youhuandaoleijia==1 && Gyro_z==0) 
		{
			if(Distance_Int>40)  youhuandao_flag1=0,youhuandaoleijia=0,Encoder_Int=0,Distance=0,Distance_Int=0;               
		}
}


void Key_Pro()
{
	static uint16 Voltage_Count=0;
	static bit Voltage_Flag=0;
	
	enum KeyValue key = Key_Single_Double(60);
	//外侧上拨启动
	if(sw1_status)
	{
		Test_Wireless_Oscilloscope();//无线转串口调试PID		
	}
	if(sw2_status)
	{	
		//电压检测
		if(++Voltage_Count == 100) Voltage_Count=0,Voltage_Flag=1;//不能太快，会让单片机挂掉
		if(Voltage_Flag)
		{
			Voltage_Flag=0;
			Bat_Vol = ADC_Voltage();
		}
	}
	if(!sw2_status) Bat_Vol=0;
	
	if( key == Down_Test )
	{
		key2_flag=0;
		Encoder_Int = 1;
		P64 = ~P64;
	}
	if( key == Down_Long )
	{
		Gyro_Int = 1;
		P65 = ~P65;
	}
	
	if( key == Down_Long && running == 0)
	{
		running = 1;
		
	}
	/* 冲出赛道保护 */
	if(L<1 && L2<1 && R2<1 && R<1)run=0;
	else run=1;
	
	/* 速度设置 & 差速计算 */
	if(run == 1 && running == 1)  // 在赛道内
	{	
		Left_High_Speed  = 80;
		Right_High_Speed = 80   ;
	}
	if(run == 0 && running == 1) // 出赛道 进入保护,编码器计步代替延时，计步一段距离，若仍然丢线，则停车，后期补上，前期无需停车
	{
		Left_High_Speed  = 0;
		Right_High_Speed = 0;
		eleOut=0;		
	}
}


float absolute(float error2){
	float real_error2;
	if(error2>0)  real_error2= error2;
	else          real_error2=-error2;
	return real_error2;
}

/**
 * @brief 位置式 PD（用于转向控制）
 */
double Loc_pid(PID*sptr,double error,double *Turn_PID)
{
	double Loc_out;
	
	 
	sptr->KP   = *Turn_PID;  // 参数赋值
	sptr->GKD  = *(Turn_PID+1);
	sptr->KP2  = *(Turn_PID+2);
	sptr->KD   = *(Turn_PID+3);
	
	Loc_out = error * sptr->KP 
		    - mpu6050_gyro_z * sptr->GKD 
			+ error*absolute(error)*sptr->KP2 
			+ (error - sptr->LastError)*sptr->KD;
	
	sptr->LastError = error; 
	return Loc_out;
}



/**
 * @brief 增量式 PID（用于左右轮电机闭环控制）
 */
double PID_Realize(PID*sptr, double ActualSpeed, double SetSpeed, double *MOTOR_PID)
{
	double Increase;  // 单次PID输出
	
	sptr->KP = *MOTOR_PID;  // 参数赋值
	sptr->KI = *(MOTOR_PID+1);
	sptr->KD = *(MOTOR_PID+2);

	sptr->iError = SetSpeed - ActualSpeed;  // 当前误差 = 目标值 - 实际值

	Increase = sptr->KP * (sptr->iError - sptr->LastError)
			 + sptr->KI * sptr->iError
			 + sptr->KD * (sptr->iError - 2*sptr->LastError + sptr->PrevError);

	sptr->PrevError = sptr->LastError;  // 让上上次等于上次
	sptr->LastError = sptr->iError;  // 让上次等于这次

	return Increase;
}