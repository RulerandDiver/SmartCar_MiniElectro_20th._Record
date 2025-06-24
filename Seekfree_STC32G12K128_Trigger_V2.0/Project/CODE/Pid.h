#ifndef __PID_H_
#define __PID_H_

#include "headfile.h"

typedef struct
{
	float Kp;
	float Ki;
	float Kd;
	float Kf;//前馈
	float GKd;//陀螺仪
	float Kp2;//二次Kp

	
	float Error;
	float LastError;
	float PreError;
	float IntError;
	float LastTarget;
	
	float P_Out;
	float I_Out;
	float D_Out;
	float Output;
	
	float I_Limit;//积分限幅
	float O_Limit;//输出限幅
}PID_tt;

extern PID_tt LeftPID;//左轮速度PID
extern PID_tt RightPID;//右轮速度PID
extern PID_tt TurnPID;//转向环PID
extern PID_tt SpeedPID;


void Pid_Clean(PID_tt* Pid);
void Pid_Init(PID_tt *Pid,  float Kp, float Ki, float Kd, float Kf, float I_Limit, float O_Limit, float GKd, float Kp2);
float Pid_Velocity(PID_tt* Pid, float ActualSpeed, float SetSpeed);
float Pid_Position(PID_tt* Pid,double error);
int I_abs(int num);
float Pid_Inc_Ff(PID_tt* Pid, float Real, float Target);
float Pid_Pos_GyroZ(PID_tt* Pid, float position, float GyroZ);
#endif
