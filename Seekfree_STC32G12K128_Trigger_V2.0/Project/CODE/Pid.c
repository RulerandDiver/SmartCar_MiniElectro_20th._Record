#include "Pid.h"

PID_tt LeftPID;//�����ٶ�PID
PID_tt RightPID;//�����ٶ�PID
PID_tt TurnPID;//ת��PID
PID_tt SpeedPID;

float F_abs(float num)
{
	return (num > 0 ? num : -num);
}

int I_abs(int num)
{
	return (num > 0 ? num : -num);
}	

void Pid_Init(PID_tt *Pid,  float Kp, float Ki, float Kd, float Kf, float I_Limit, float O_Limit, float GKd, float Kp2)
{
	Pid -> Kp = Kp;
	Pid -> Ki = Ki;
	Pid -> Kd = Kd;
	Pid -> Kf = Kf;
	Pid -> GKd = GKd;
	Pid -> Kp2 = Kp2;
	
	Pid -> I_Limit = I_Limit;
	Pid -> O_Limit = O_Limit;
	
	Pid->Error = 0.0f;
	Pid->LastError = 0.0f;
	Pid->PreError = 0.0f;
	Pid->IntError = 0.0f;
	Pid->LastTarget = 0.0f;
	
	Pid->P_Out = 0.0f;
	Pid->I_Out = 0.0f;
	Pid->D_Out = 0.0f;
	Pid->Output = 0.0f;
	
}

/**
 * @brief �ٶ�pid��ǰ��
 */
float Pid_Inc_Ff(PID_tt* Pid, float Real, float Target)
{
	Pid->Error = Target - Real;
	Pid->IntError += Pid->Error;
	
	//�����޷�
	if (Pid->IntError > Pid->I_Limit)
	{
		Pid->IntError = Pid->I_Limit;
	}
	else if (Pid->IntError < -Pid->I_Limit)
	{
		Pid->IntError = -Pid->I_Limit;
	}
	
	//���ԡ����֡�΢�֡�ǰ����ͬ����
	Pid->P_Out = Pid->Kp * Pid->Error;
	Pid->I_Out = Pid->Ki * Pid->IntError;
	Pid->D_Out = Pid->Kd * (Pid->Error - Pid->LastError);
	
	Pid->Output = Pid->P_Out + Pid->I_Out + Pid->D_Out + Pid->Kf * (Target - Pid->LastTarget);
	
	Pid->LastError = Pid->Error;
	Pid->LastTarget = Target;
	
	//����޷�
	if (Pid->Output > Pid->O_Limit)
	{
		Pid->Output = Pid->O_Limit;
	}
	else if (Pid->Output < -Pid->O_Limit)
	{
		Pid->Output = -Pid->O_Limit;
	}
	
	return Pid->Output; 
}

/**
 * @brief ħ��λ��ʽpid���Ӷ����
 */
float Pid_Pos_GyroZ(PID_tt* Pid, float position, float GyroZ)
{
//	pid->p_out = (pid->kp * position) + (pid->kp * pid->kp * position * myfabs(position));
//	pid->d_out = (pid->kd * (position - pid->lasterror) + (pid->kd * pid->kd * GyroZ));
	Pid->P_Out = Pid->Kp * position;
//	Pid->d_out = (Pid->kd * (position - Pid->lasterror);
	Pid->D_Out = Pid->Kd * GyroZ;
//	Pid->d_out = 0.8*Pid->kd*Pid->kd * GyroZ+0.2*Pid->kd * (position - Pid->lasterror);
	
	Pid->Output = Pid->P_Out + Pid->D_Out;
	Pid->LastError = position;
	
	//����޷�
	if (Pid->Output > Pid->O_Limit)
	{
		Pid->Output = Pid->O_Limit;
	}
	else if (Pid->Output < -Pid->O_Limit)
	{
		Pid->Output = -Pid->O_Limit;
	}
	
	return Pid->Output;
}

#if 0
/**
 * @brief λ��ʽ PD������ת����ƣ�
 */
float Pid_Position(PID_tt* Pid,double error)
{
	float Loc_out;
	
	Pid -> Error = error;
	
	Loc_out = Pid -> Error * Pid->Kp 
		    - mpu6050_gyro_z * Pid->GKd 
			+ Pid -> Error*absolute(Pid -> Error)*Pid->Kp2 
			+ (Pid -> Error - Pid -> LastError)* Pid -> Kd;
	
	Pid -> LastError = Pid -> Error; 
	return Loc_out;
}

/**
 * @brief ����ʽ PID�����������ֵ���ջ����ƣ�
 */
float Pid_Velocity(PID_tt* Pid, float ActualSpeed, float SetSpeed)
{
	float Increase;  // ����PID���


	Pid->Error = SetSpeed - ActualSpeed;  // ��ǰ��� = Ŀ��ֵ - ʵ��ֵ

	Increase = Pid->Kp * (Pid->Error - Pid->LastError)
			 + Pid->Ki * Pid->Error
			 + Pid->Kd * (Pid->Error - 2*Pid->LastError + Pid->PreError);

	Pid->PreError = Pid->LastError;  // �����ϴε����ϴ�
	Pid->LastError = Pid->Error;  // ���ϴε������

	return Increase;
}
#endif

/**
 * @brief PID��λ
 */
void Pid_Clean(PID_tt* Pid)
{
	Pid->LastError = 0;
	Pid->IntError = 0;
	Pid->PreError = 0;
	Pid->Output = 0;
}





