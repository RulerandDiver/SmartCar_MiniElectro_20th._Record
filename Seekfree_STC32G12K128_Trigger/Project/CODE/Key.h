#ifndef __KEY_H_
#define __KEY_H_

#include "headfile.h"


// ʹ��ǰ׺�����ͻ
#define KEY_UP_PIN    P36
#define KEY_DOWN_PIN  P37
#define KEY_ENTER_PIN P35
//#define Back        P35 ˫��
//#define BigChange   P35 ����
//���岦�뿪������
#define SW1_PIN     P67
#define SW2_PIN     P66

// ��������ֵö�٣���ӳ���ö��ֵ��
enum KeyValue {
    Up,         // �ϼ�����
    Down,       // �¼�����
    Enter,      // ȷ�ϼ�����
    Back,       // ���أ�ȷ�ϼ�˫����
    BigChange,  // ȷ�ϼ�����
    Up_Test,    // �ϼ�˫��
    Down_Test,  // �¼�˫��
    Up_Long,    // �ϼ�������������
    Down_Long,  // �¼�������������
    Null        // �޲���
};

// ������ֵ���Ե��ô���Ϊ��λ��
#define LONG_PRESS_COUNT 150  // ʾ��ֵ������ʵ�ʵ��� 50~150

// ״̬����
#define STATE_IDLE          0
#define STATE_FIRST_PRESS   1
#define STATE_FIRST_RELEASE 2
#define STATE_SECOND_PRESS  3
#define STATE_LONG_PRESS    4  // ��������״̬

// ��������
enum KeyValue Key_Single_Double(uint8 double_click_time);
#endif
