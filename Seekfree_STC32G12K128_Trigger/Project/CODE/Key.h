#ifndef __KEY_H_
#define __KEY_H_

#include "headfile.h"


// 使用前缀避免冲突
#define KEY_UP_PIN    P36
#define KEY_DOWN_PIN  P37
#define KEY_ENTER_PIN P35
//#define Back        P35 双击
//#define BigChange   P35 长按
//定义拨码开关引脚
#define SW1_PIN     P67
#define SW2_PIN     P66

// 按键返回值枚举（添加长按枚举值）
enum KeyValue {
    Up,         // 上键单击
    Down,       // 下键单击
    Enter,      // 确认键单击
    Back,       // 返回（确认键双击）
    BigChange,  // 确认键长按
    Up_Test,    // 上键双击
    Down_Test,  // 下键双击
    Up_Long,    // 上键长按（新增）
    Down_Long,  // 下键长按（新增）
    Null        // 无操作
};

// 长按阈值（以调用次数为单位）
#define LONG_PRESS_COUNT 150  // 示例值，根据实际调整 50~150

// 状态定义
#define STATE_IDLE          0
#define STATE_FIRST_PRESS   1
#define STATE_FIRST_RELEASE 2
#define STATE_SECOND_PRESS  3
#define STATE_LONG_PRESS    4  // 新增长按状态

// 函数声明
enum KeyValue Key_Single_Double(uint8 double_click_time);
#endif
