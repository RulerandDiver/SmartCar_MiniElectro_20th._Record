#include "Filtering.h"


const float imu693kf_Q = 0.17;
const float imu693kf_R = 0.20;

KalmanFilter imu693_kf;

LowPassFilter leftSpeedFilt;
LowPassFilter rightSpeedFilt;


void Kalman_Init(KalmanFilter* kf, float F, float B, float Q, float R, float initial_x)
{
    kf->F = F;
    kf->B = B;
    kf->Q = Q;
    kf->R = R;
    kf->P = 1.0;       // 初始协方差
    kf->x = initial_x;
}

void Kalman_Predict(KalmanFilter* kf, float u)
{
    kf->x = kf->F * kf->x + kf->B * u;
    kf->P = kf->F * kf->P * kf->F + kf->Q;
}

float Kalman_Update(KalmanFilter* kf, float z)
{
    kf->K = kf->P / (kf->P + kf->R);
    kf->x += kf->K * (z - kf->x);
    kf->P *= (1 - kf->K);
    return kf->x;
}

void LowPass_init(LowPassFilter* instance, float alpha)
{
	instance->alpha = alpha;
	instance->output = 0;
	instance->lastoutput = 0;
}

float LowPass_Filter(LowPassFilter* instance, float input) 
{
    // 实现一阶低通滤波的核心算法
    instance->output = instance->alpha * input + (1 - instance->alpha) * instance->lastoutput;
    
    // 更新上一时刻的输入值
    instance->lastoutput = instance->output;
    
    return instance->output;
}