#include "Key.h"

/**************************************************************************
�������ܣ�����ɨ��
��ڲ�����˫���ȴ�ʱ�� 20~50
����  ֵ������״̬ 
**************************************************************************/
enum KeyValue Key_Single_Double(uint8 double_click_time)
{
	//��̬ö�٣�����״̬��������ʼ״̬ΪIDLE
    // ״̬����
    // === ���оֲ����������ں�����ͷ���� ===
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
    
    // ��ȡ��ǰ����״̬
    current_up = KEY_UP_PIN;
    current_down = KEY_DOWN_PIN;
    current_enter = KEY_ENTER_PIN;
    
    // �������� - ����3����ͬ״̬��ȷ��
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
    
    // ��������״̬������͵�ƽ���£�
    up_pressed = (current_up == 0);
    down_pressed = (current_down == 0);
    enter_pressed = (current_enter == 0);
    
    // === ״̬������ ===
	//state ���������֣�define�����������֣���������ö��
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
            
            // �������
            if (press_count > LONG_PRESS_COUNT) 
            {
                // ���볤��״̬������������
                state = STATE_LONG_PRESS;
                press_count = 0;
            }
            
            // �����ͷ�
            if (!up_pressed && !down_pressed && !enter_pressed) 
            {
                // �ڴﵽ������ֵǰ�ͷţ�׼�����˫��
                release_count = 0;
                state = STATE_FIRST_RELEASE;
            }
            break;
            
        case STATE_LONG_PRESS:  // �����ĳ�������״̬
            // �ڴ�״̬�º������а���������ֻ�ȴ��ͷ�
            if (!up_pressed && !down_pressed && !enter_pressed) 
            {
                state = STATE_IDLE;
                
                // ���س����¼�
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
            
            // ˫����ʱ
            if (release_count > double_click_time) 
            {
                state = STATE_IDLE;
                return first_key; // ���ص���
            }
            
            // �ڶ��ΰ��£�������ͬһ������
            if ((first_key == Up && up_pressed) ||
                (first_key == Down && down_pressed) ||
                (first_key == Enter && enter_pressed)) 
            {
                state = STATE_SECOND_PRESS;
            }
            break;
            
        case STATE_SECOND_PRESS:
            // �ȴ��ͷ�
            if (!up_pressed && !down_pressed && !enter_pressed) 
            {
                state = STATE_IDLE;
                
                // ����˫������
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