#ifndef __KEY_H_
#define __KEY_H_

#include "headfile.h"


typedef struct
{
	uint8 step;
	uint8 state;
	uint8 flag;
}Key_tt;


// 使用前缀避免冲突
#define KEY_UP    P37
#define KEY_FUNC  P36
#define KEY_DOWN  P70
#define KEY_GO    P35


//定义拨码开关引脚
#define SW1_PIN     P67
#define SW2_PIN     P66

extern Key_tt Key[4];
extern bit Go_Flag;
extern uint8 sw1_status;
extern uint8 sw2_status;
void Key_Task();
void Key_Scan();


#endif
