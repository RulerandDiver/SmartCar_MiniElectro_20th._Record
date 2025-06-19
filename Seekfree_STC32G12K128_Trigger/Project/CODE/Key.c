#include "Key.h"

/**************************************************************************
函数功能：按键扫描
入口参数：双击等待时间 20~50
返回  值：按键状态 
**************************************************************************/
enum KeyValue Key_Single_Double(uint8 double_click_time)
{
	//静态枚举，定义状态变量，初始状态为IDLE
    // 状态定义
    // === 所有局部变量必须在函数开头声明 ===
    static uint8 state = STATE_IDLE;
    static enum KeyValue first_key = Null;
    static uint16 press_count = 0;
    static uint16 release_count = 0;
    static uint8 debounce_count = 0;
    static uint8 last_up_state = 1;
    static uint8 last_down_state = 1;
    static uint8 last_enter_state = 1;
    
    uint8 current_up, current_down, current_enter;
    uint8 up_pressed = 0, down_pressed = 0, enter_pressed = 0;
    
    // 读取当前按键状态
    current_up = KEY_UP_PIN;
    current_down = KEY_DOWN_PIN;
    current_enter = KEY_ENTER_PIN;
    
    // 消抖处理 - 连续3次相同状态才确认
    if (current_up != last_up_state || 
        current_down != last_down_state || 
        current_enter != last_enter_state) 
    {
        debounce_count = 0;
        last_up_state = current_up;
        last_down_state = current_down;
        last_enter_state = current_enter;
        return Null;
    } 
    else if (debounce_count < 3) 
    {
        debounce_count++;
        return Null;
    }
    
    // 解析按键状态（假设低电平按下）
    up_pressed = (current_up == 0);
    down_pressed = (current_down == 0);
    enter_pressed = (current_enter == 0);
    
    // === 状态机处理 ===
	//state 可以用数字，define定义仍是数字，但不可用枚举
    switch (state) 
    {
        case STATE_IDLE:
            if (up_pressed) 
            {
                first_key = Up;
                press_count = 0;
                state = STATE_FIRST_PRESS;
            } 
            else if (down_pressed) 
            {
                first_key = Down;
                press_count = 0;
                state = STATE_FIRST_PRESS;
            } 
            else if (enter_pressed) 
            {
                first_key = Enter;
                press_count = 0;
                state = STATE_FIRST_PRESS;
            }
            break;
            
        case STATE_FIRST_PRESS:
            press_count++;
            
            // 长按检测
            if (press_count > LONG_PRESS_COUNT) 
            {
                // 进入长按状态，不立即返回
                state = STATE_LONG_PRESS;
                press_count = 0;
            }
            
            // 按键释放
            if (!up_pressed && !down_pressed && !enter_pressed) 
            {
                // 在达到长按阈值前释放，准备检测双击
                release_count = 0;
                state = STATE_FIRST_RELEASE;
            }
            break;
            
        case STATE_LONG_PRESS:  // 新增的长按处理状态
            // 在此状态下忽略所有按键操作，只等待释放
            if (!up_pressed && !down_pressed && !enter_pressed) 
            {
                state = STATE_IDLE;
                
                // 返回长按事件
                if (first_key == Up) 
                {
                    return Up_Long;
                }
                else if (first_key == Down) 
                {
                    return Down_Long;
                }
                else if (first_key == Enter) 
                {
                    return BigChange;
                }
            }
            break;
            
        case STATE_FIRST_RELEASE:
            release_count++;
            
            // 双击超时
            if (release_count > double_click_time) 
            {
                state = STATE_IDLE;
                return first_key; // 返回单击
            }
            
            // 第二次按下（必须是同一个键）
            if ((first_key == Up && up_pressed) ||
                (first_key == Down && down_pressed) ||
                (first_key == Enter && enter_pressed)) 
            {
                state = STATE_SECOND_PRESS;
            }
            break;
            
        case STATE_SECOND_PRESS:
            // 等待释放
            if (!up_pressed && !down_pressed && !enter_pressed) 
            {
                state = STATE_IDLE;
                
                // 返回双击类型
                if (first_key == Up) 
                {
                    return Up_Test;
                }
                else if (first_key == Down) 
                {
                    return Down_Test;
                }
                else if (first_key == Enter) 
                {
                    return Back;
                }
            }
            break;
    }
    
    return Null;
	
}