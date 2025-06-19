#ifndef __CONTROL_H_
#define __CONTROL_H_

#include "headfile.h"
#include "Key.h"
typedef struct PID
{
	int16 iError;  // ����ƫ��
    int16 LastError;  // �ϴ�ƫ��
    int16 PrevError;  // ���ϴ�ƫ��
    double KP;
    double KI;
    double KD;
	double KP2;
	double GKD;
}PID;

#define MOTOR_MAX 4000

//���尴������
#define KEY1_PIN    P36
#define KEY2_PIN    P37
#define KEY3_PIN    P35
#define KEY4_PIN    P71
//���岦�뿪������
#define SW1_PIN     P67
#define SW2_PIN     P66
//�޷��꺯��
#define Fun_Limit_ab(x ,a , b) ((x) < (a) ? (a) : ( (x) > (b) ? (b) : (x)))
//�·�externΪ�������ͣ�����main�ļ���debugʹ��

extern PID Left_MOTOR_PID, Right_MOTOR_PID, Turn_PID_ele;  // �����ṹ�����
extern float Loc_kp;
extern float Loc_kd;

extern unsigned char run;

extern int16 adc_max[]; //��һ��
extern int16 adc_min[]; 
extern int16 L, L2, M, R2, R;  			 		   // ���ֵ
extern double eleValue;  // ��Ⱥ�

extern double   Turn_ele[4];  // ת�� PD ����
extern double eleOut;		// ת�����ֵ

extern double Left_MOTOR[3];   // �����ٶȻ� PID ����
extern double Right_MOTOR[3];  // �����ٶȻ� PID ����

extern double encoder_L, encoder_R;								// �����ֱ���������
extern double LEFT_MOTOR_Duty, RIGHT_MOTOR_Duty; 				// ���ҵ��ռ�ձ�
extern double Left_High_Speed, Right_High_Speed, High_Speed;	// ������Ŀ���ٶȡ�����Ŀ���ٶ�

extern uint16 Bat_Vol;//��ص�ѹ

extern double Distance_Int;
extern bit Encoder_Int;

extern bit Gyro_Int;
extern float Gyro_z;
/**********************************����������******************************************/
//���ر�־λ
extern uint8 key1_flag;
extern uint8 key2_flag;
extern uint8 key3_flag;
extern uint8 key4_flag;
//���뿪��״̬����
extern uint8 sw1_status;
extern uint8 sw2_status;
/*****************************************************************************************************/
//С������״̬����
extern uint8 running;

/**********************************������******************************************/
void Dir_Control();		// ת����ƺ���
void Motor_Control();  	// ������ƺ���
void Ultima_Control();  // �ۺϿ��ƺ���
void ADC_Measure(); //��в�������һ��
void KEY_Scan(void);//�������
void Ultima_Control(void);
float absolute(float error2);

void huandao(void);
double range_protect(double duty,double min,double max);

double PID_Realize(PID*sptr, double ActualSpeed, double SetSpeed, double *MOTOR_PID);   // ����ʽ PID
double Loc_pid(PID*sptr,double error,double *Turn_PID);
void Key_Pro();
/*****************************************************************************************************/

#endif
