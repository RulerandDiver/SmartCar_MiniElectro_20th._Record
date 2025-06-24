#ifndef __ELEC_H_
#define __ELEC_H_

#include "headfile.h"

// ���ͨ������
#define ADC_HL  ADC_P10  // ��������
#define ADC_VL  ADC_P17  // ���������
//#define ADC_HML ADC_P01  // ���к�����
#define ADC_HC  ADC_P03  // �м������
//#define ADC_HMR ADC_P13  // ���к�����
#define ADC_VR  ADC_P01  // �Ҳ�������
#define ADC_HR  ADC_P02  // �Ҳ������

// ������鶨��
#define SENSOR_COUNT 5   // ��и���
#define HISTORY_COUNT 5  // �˲���������ǰֻ�洢����ֵ

// ����������������track_type��Ӧ
#define WEIGHT_STRAIGHT    0  // ֱ��
#define WEIGHT_RIGHT_ANGLE 1  // ֱ�����
#define WEIGHT_CROSS       2  // ʮ��Բ��
#define WEIGHT_ROUNDABOUT  3  // ����
#define WEIGHT_SPEED       4  //��ȼ��� 

// ������Ȩ�ؽṹ��
typedef struct {
    float weight_outer;    // �����Ȩ��(HL��HR)
    float weight_center;   // ���ĵ��Ȩ��(HC)
    float weight_vertical; // ������Ȩ��(VL��VR)
    float filter_param;    // �˲�ϵ�����ɵ�
    int16 max_change_rate; // ��������仯��
    char *name;            // �����������ƣ����ڵ���
} TrackWeights;


// �������ö��
//typedef enum {
//    SENSOR_HL  = 0,  // ��������
//    SENSOR_VL  = 1,  // ���������-8
//    SENSOR_HML = 2,  // ���к�����
//    SENSOR_HC  = 3,  // �м������
//    SENSOR_HMR = 4,  // ���к�����
//    SENSOR_VR  = 5,  // �Ҳ�������
//    SENSOR_HR  = 6   // �Ҳ������
//} sensor_type_e;


// �������ö��
typedef enum {
    SENSOR_HL  = 0,  // ��������
    SENSOR_VL  = 1,  // ���������-8
    SENSOR_HC  = 2,  // �м������
    SENSOR_VR  = 3,  // �Ҳ�������
    SENSOR_HR  = 4   // �Ҳ������
} sensor_type_e;

// ��������
void electromagnetic_init(void);               // ��ʼ����Ŵ�����
uint16 get_adc(uint16 i);                      // ��ȡADC��ֵ
void average_filter(void);                     // ���ƾ�ֵ�˲�����
void mid_filter(void);                         // ��λֵ�˲�����
void update_min_max_values(void);              // ����ÿ����е������Сֵ
void normalize_sensors(void);                  // ��һ���������
int16 calculate_position_improved(void);      // �Ľ������λ��
uint8 check_electromagnetic_protection(void);  // ��ű����߼�����

// �ⲿ��������
extern uint16 adc_fliter_data[SENSOR_COUNT][HISTORY_COUNT]; // �˲����ֵ
extern float result[SENSOR_COUNT];                // ��н������
extern float normalized_data[SENSOR_COUNT];       // ��һ����ĵ�����ݣ�������ʽ
extern uint16 min_value[SENSOR_COUNT];            // ÿ����е���Сֵ
extern uint16 max_value[SENSOR_COUNT];            // ÿ����е����ֵ
extern int16 position;                         // λ��ƫ��
extern uint8 protection_flag;                  // ������־
extern float signal_strength_value;            // �ź�ǿ��ָ��

//���λ�ü������
extern float filter_param;   // �˲�ϵ�����ɵ�
extern uint8 track_type;        // �������ͣ�0-��ͨ��1-ʮ�֣�2-������3-ֱ�����
extern uint8 track_type_last;         // �������ͣ�0-��ͨ��1-ֱ�������2-ʮ��Բ����3-����

extern uint8 track_route;      // ����·����1-��ת��2-��ת
extern uint8 track_route_status;
extern uint8 track_type_zj;
extern uint8 track_ten_flag;
extern uint8 ten_change_flag;
// ������غ���
void display_electromagnetic_data(void);       // ��ʾ��Ŵ���������

// ��ű����߼�����
extern uint8 protection_flag;

#endif
