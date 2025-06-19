#include "Control.h"

PID Left_MOTOR_PID, Right_MOTOR_PID, Turn_PID_ele;  // �����ṹ�����

unsigned char run = 0;  // �����ͣ��־λ


int16 adc_max[5] = {3560,3555,3560,3570,3570}; 
int16 adc_min[5] = {88,4,1,6,83}; 

/*���*/
//short data ����extern���������ļ�����.����oled��ʾʱ���������ʹ��
int16 L=0, L2=0, M=0, R2=0, R=0;
double eleValue=0;	// ��Ⱥ�
double eleOut = 0;		
uint8 Left_Vector=0,Right_Vector=0;
int16 eleSum=0,eleDiff=0;

/*��ѹ*/
uint16 Bat_Vol=0;
/*ת��*/
//double  Turn_ele[4]={4.1 , 0.03 , 0 , 0};//KP,GKD,KP2,KD	
double  Turn_ele[4]={3.3 , 0.02 , 0.01 , 0};//KP,GKD,KP2,KD	
			

/*�ٶȻ�*/
double Left_MOTOR[3] =  {80, 1, 0};   
double Right_MOTOR[3] = {80, 1, 0};  

/*����ͱ�����*/
double encoder_L, encoder_R;							
double LEFT_MOTOR_Duty, RIGHT_MOTOR_Duty;				
double Left_High_Speed=0, Right_High_Speed=0;

/*����������*/
bit Encoder_Int=0;
double Distance=0,Distance_Int=0;

/*�����ǻ���*/
bit Gyro_Int=0;
float Gyro_z=0;

/*����*/
short data youhuandao_flag1=0,youhuandao_flag2_s=0,youhuandao_flag3_s=0,youhuandaoleijia=0;


/**********************************����������******************************************/
//���뿪��״̬����
uint8 sw1_status;
uint8 sw2_status;

//����״̬����
uint8 key1_status = 1;
uint8 key2_status = 1;
uint8 key3_status = 1;
uint8 key4_status = 1;

//��һ�ο���״̬����
uint8 key1_last_status;
uint8 key2_last_status;
uint8 key3_last_status;
uint8 key4_last_status;

//���ر�־λ
uint8 key1_flag;
uint8 key2_flag;
uint8 key3_flag;
uint8 key4_flag;

uint8 running=0;
/*****************************************************************************************************/
/**
 * @brief �޷���������
 */
double range_protect(double duty, double min, double max)
{
    return duty >= max ? max : (duty <= min ? min : duty);
}


/**
 * @brief ������ƺ�����ʵ���������ٶȱջ�����
 */
void Motor_Control()
{
	// ��������������Ļ���Ѿ���ʾ��һ��Ҫ�˶�ȷ�ϣ������Ҳ�Ҫ�㷴  ��������Ҫ�㷴
	encoder_L = Read_Encoder(En_L);  // �������
	encoder_R = Read_Encoder(En_R)*0.5;   // �ұ�����
	
	//������Z���ȡ
	mpu6050_get_gyro();
	
	/* �ɼ���ͨ�����ֵ */
	ADC_Measure();

	/* ��Ⱥͼ�������ƫ�� */
	if(youhuandao_flag2_s == 1) eleValue = 50;
	else if(youhuandao_flag3_s == 1) eleValue = 10;//��ֹ��Բ������
	//������Ⱥ�ѭ��
//	else	eleValue = ((L - R) + (L2 - R2))*100/(L + R + L2 + R2 + 1);	// ��Ⱥͤη���
	else eleValue = (eleDiff * 100) / (eleSum + 1);//�����Ⱥ���ֵ ���Ŵ�

	eleOut = Loc_pid(&Turn_PID_ele,eleValue,Turn_ele); //λ�û�
	

	//ʵ�ʵ��ں��ٶȼ�ȥʵ�ʵ��ں�λ��ƫ���ȥһ����ֵ�����������
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
 * @brief �ۺϿ��ƺ��������ж��е���
 */
void Ultima_Control()
{
//	huandao();
//	if(Encoder_Int)
//	{
//		Distance += encoder_L;
//		Distance_Int = Distance/80;//����������������ʵ�ʾ��� cm
//	}
//	if(Gyro_Int)
//	{
//		Gyro_z += (mpu6050_gyro_z - 15)*0.00064;//0.00064     360�� ���� ��������� 
//	}
	//Dir_Control();  // ת�����
	Motor_Control();  //�������
	//KEY_Scan();//�������
	Key_Pro();
}

/**
 * @brief ѭ����в���
 */
void ADC_Measure()
{
	L  = ADC_Median_Average(ADC_P10);  	
	L2 = ADC_Median_Average(ADC_P17);	
	M  = ADC_Median_Average(ADC_P03);		
	R2 = ADC_Median_Average(ADC_P01);	
	R  = ADC_Median_Average(ADC_P02);		

	L   = (float)(L - adc_min[0])   / (adc_max[0] - adc_min[0])* 100.0f;    //���ֵ��Сֵ��Ҫ������ϣ������Ǽ򵥵ĸ���ֱ����ֵ�ö�
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
		
		if(M>0 ) //�Ӹ����жϵ��ڣ���ֹ��ʮ����ʶ�𣬼�1 5���
			P64=1,youhuandao_flag1=0,Gyro_Int=1,Encoder_Int=0,Distance=0,Distance_Int=0,youhuandao_flag2_s=1;         			  
		else 					youhuandaoleijia=0,youhuandao_flag1=0,Encoder_Int=0,Distance=0,Distance_Int=0;    
	}			
    if(youhuandaoleijia==1 && youhuandao_flag2_s==1 && Gyro_z<-21) P64=0,youhuandao_flag2_s=0;
  	
	  if(youhuandaoleijia==1 && Gyro_z!=0) 
	  {
		if(Gyro_z<-290 && Gyro_z>-300)  youhuandao_flag3_s=1;
		if(Gyro_z<-300) 							  youhuandao_flag3_s=0,youhuandao_flag1=1,Encoder_Int=1,Gyro_Int=0,Gyro_z=0;        
	  }		
//��ֹ��Բ��ʱ �ٴ�ʶ��Բ��	  
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
	//����ϲ�����
	if(sw1_status)
	{
		Test_Wireless_Oscilloscope();//����ת���ڵ���PID		
	}
	if(sw2_status)
	{	
		//��ѹ���
		if(++Voltage_Count == 100) Voltage_Count=0,Voltage_Flag=1;//����̫�죬���õ�Ƭ���ҵ�
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
	/* ����������� */
	if(L<1 && L2<1 && R2<1 && R<1)run=0;
	else run=1;
	
	/* �ٶ����� & ���ټ��� */
	if(run == 1 && running == 1)  // ��������
	{	
		Left_High_Speed  = 80;
		Right_High_Speed = 80   ;
	}
	if(run == 0 && running == 1) // ������ ���뱣��,�������Ʋ�������ʱ���Ʋ�һ�ξ��룬����Ȼ���ߣ���ͣ�������ڲ��ϣ�ǰ������ͣ��
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
 * @brief λ��ʽ PD������ת����ƣ�
 */
double Loc_pid(PID*sptr,double error,double *Turn_PID)
{
	double Loc_out;
	
	 
	sptr->KP   = *Turn_PID;  // ������ֵ
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
 * @brief ����ʽ PID�����������ֵ���ջ����ƣ�
 */
double PID_Realize(PID*sptr, double ActualSpeed, double SetSpeed, double *MOTOR_PID)
{
	double Increase;  // ����PID���
	
	sptr->KP = *MOTOR_PID;  // ������ֵ
	sptr->KI = *(MOTOR_PID+1);
	sptr->KD = *(MOTOR_PID+2);

	sptr->iError = SetSpeed - ActualSpeed;  // ��ǰ��� = Ŀ��ֵ - ʵ��ֵ

	Increase = sptr->KP * (sptr->iError - sptr->LastError)
			 + sptr->KI * sptr->iError
			 + sptr->KD * (sptr->iError - 2*sptr->LastError + sptr->PrevError);

	sptr->PrevError = sptr->LastError;  // �����ϴε����ϴ�
	sptr->LastError = sptr->iError;  // ���ϴε������

	return Increase;
}