#include "Elec.h"

// �˲������� - ʹ�ö�ά������ʽ
// ��һά��ʾ��б�ţ�0-HL, 1-VL, 2-HML, 3-HC, 4-HMR, 5-VR, 6-HR
//0-HL, 1-VL, 2-HC, 3-VR, 4-HR
// �ڶ�ά�����������ڴ洢��ʷ����



// ����ȫ��Ȩ�����ã�ֻ�������ֻ���Ԫ��
//�����Ȩ��(HL��HR),���ĵ��Ȩ��(HC),������Ȩ��(VL��VR),�˲�ϵ��,��������仯��,����
TrackWeights track_weights[4] = {
    // ��ֱͨ��

    {0.20f, 0.30f, 0.20f, 0.70f, 30, "ֱ��"},
    
    // ֱ�����
    {0.25f, 0.35f, 0.40f, 1.00f, 50, "ֱ�����"},
    
    // ʮ��Բ��
    {0.35f, 0.20f, 0.15f, 0.90f, 40, "ʮ��Բ��"},
    
    // ����
    {0.35f, 0.10f, 0.25f, 1.00f, 50, "����"}
};

uint16 adc_fliter_data[SENSOR_COUNT][HISTORY_COUNT] = {0}; //�˲����ֵ
float result[SENSOR_COUNT] = {0};		//��洢ÿ������˲�������ս��ֵ����δ��һ�������������˲�����͹�һ��������м����
uint16 sum[SENSOR_COUNT][HISTORY_COUNT] = {0};      	//�ۼӵĺ�


// ��һ������ - ��Ϊ������ʽ
float normalized_data[SENSOR_COUNT] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};  // ��һ����ĵ����������

// �洢ÿ����е������Сֵ�����ڶ�̬У׼ - ��Ϊ������ʽ
// uint16 min_value[SENSOR_COUNT] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};  // ÿ����е���Сֵ
// uint16 max_value[SENSOR_COUNT] = {0, 0, 0, 0, 0, 0, 0};  // ÿ����е����ֵ
uint16 min_value[SENSOR_COUNT] = {12,0,0,0,5};  // ÿ����е���Сֵ
uint16 max_value[SENSOR_COUNT] = {890,890,888,890,890};  // ÿ����е����ֵ

// ���λ�ü�����ر���
float signal_strength_value = 0;   // �ź�ǿ��ָ��
int16 position = 0;
float filter_param = 0.4f;   // �˲�ϵ�����ɵ� Խ��Խ����


// ������Ϣ��ر�־λ
uint8 track_type = 0;         // �������ͣ�0-��ͨ��1-ֱ�������2-ʮ��Բ����3-����
uint8 track_type_last = 0;         // �������ͣ�0-��ͨ��1-ֱ�������2-ʮ��Բ����3-����

uint8 track_type_zj = 0;	  //1-��ֱ�ǣ�2-��ֱ��
uint8 track_route = 0; 		  //1-�һ���2-��
uint8 track_route_status = 0; //1-�뻷��2-���У�3-����
uint8 track_ten_flag = 1;	//ʮ��Բ����0��ʾ����ʱ0.5s�ٿ�ʼ�жϣ�1-���Կ�ʼ�ж�
uint8 ten_change_flag = 0; //1��ʾ0.5��track_ten_flag=1

uint8 protection_flag = 0;// ��ű����߼�����,0��ʾδ������1��ʾ����


uint8 speed_count = 0;

//-----------------------------------------------------------------------------
// @brief  	��Ŵ�������ʼ��
// @param   ��
// @return  ��
// @author  ZP
// Sample usage: electromagnetic_init();
//-----------------------------------------------------------------------------
void electromagnetic_init(void)
{
   uint8 i = 0, j = 0;

   adc_init(ADC_HL, 0);   // ��������
   adc_init(ADC_VL, 0);   // ���������
   adc_init(ADC_HC, 0);   // �м������
   adc_init(ADC_VR, 0);   // �Ҳ�������
   adc_init(ADC_HR, 0);   // �Ҳ������
   
   // ��ʼ����ά����
   for(i = 0; i < SENSOR_COUNT; i++)
   {
       for(j = 0; j < HISTORY_COUNT; j++)
       {
           adc_fliter_data[i][j] = 0;
           sum[i][j] = 0;
       }
       normalized_data[i] = 0.0f;  // ��ʼ����һ����������
   }
}

//-----------------------------------------------------------------------------
// @brief  	�õ�adc��ֵ
// @param   ���ADC/������
// @return  �����adcֵ
// @author  ZP
// Sample usage: get_adc(1)
//-----------------------------------------------------------------------------
uint16 get_adc(uint16 i)
{
	switch(i){
		case 0:
			return adc_once(ADC_HL, ADC_10BIT);  //ADC_10BIT�ǵ��Ѱ����ѷֱ���
		case 1:
			return adc_once(ADC_VL, ADC_10BIT);
		case 2:
			return adc_once(ADC_HC, ADC_10BIT);
		case 3:
			return adc_once(ADC_VR, ADC_10BIT); 
		case 4:
			return adc_once(ADC_HR, ADC_10BIT);
		default:
			return 0;
	}
}

//-----------------------------------------------------------------------------
// @brief  	���ƾ�ֵ�˲�
// @param   ��
// @return  ��
// @author  zp
// Sample usage: average_filter();
// ʵʱƽ��ADC�������ݣ����������������,
//-----------------------------------------------------------------------------
// ���ƾ�ֵ�˲���ز���
static uint8 times = HISTORY_COUNT;  // �˲�����
static uint8 i_num = SENSOR_COUNT;  // �������

void average_filter(void)
{
    static uint16 filter_index = 0;  // ���ƴ���������
    static uint8 is_initialized = 0; // ��ʼ����־,ֻ�ڵ�һ�ε���ʱ���ж�β���,��������ʱʹ�������ĵ����㷨   
    uint16 a = 0, b = 0;
    
    // ����Ƿ���Ҫ��ʼ��
    if (!is_initialized)
    {
        // �����ۼ���
        for(a = 0; a < i_num; a++)
        {
            sum[a][0] = 0;
        }
        
        // ǰ���βɼ����ۻ��㹻������
        for(filter_index = 0; filter_index < times; filter_index++)
        {
            for(b = 0; b < i_num; b++)
            {
                sum[b][0] += get_adc(b);  // �ɼ�һ��ADC���ۼ�
            }
            delay_us(5); // ��Ӷ�����ʱ��߲����ȶ���
        }
        
        // �����ʼƽ��ֵ
        for(a = 0; a < i_num; a++)
        {
            adc_fliter_data[a][0] = sum[a][0] / times;  // ��ƽ��
            result[a] = adc_fliter_data[a][0];
        }
        
        is_initialized = 1; // ��ǳ�ʼ�����
    }
    else  // �ѳ�ʼ����ִ�е��ƾ�ֵ�˲�
    {
        for(a = 0; a < i_num; a++)
        {
            // ���ƾ�ֵ�˲������㷨����ȥƽ��ֵ��������ֵ�����¼���ƽ��ֵ
            sum[a][0] -= (sum[a][0] / times);         // ÿ��ȥ��ƽ��ֵ�Ĺ���
            sum[a][0] += get_adc(a);              // ������ֵ
            adc_fliter_data[a][0] = (sum[a][0] / times);  // ���µ�ƽ��ֵ
            result[a] = adc_fliter_data[a][0];      // ������
        }
    }
}


//-----------------------------------------------------------------------------
// @brief  	��λֵ�˲�����ÿ����е���λ����Ϊ���
// @param   ��
// @return  ��
// @author  ZP
// Sample usage: mid_filter();
// ����������ţ�����ź��ȶ���,���ֵ�˲�����γ������˲��ܹ�
//-----------------------------------------------------------------------------
static uint8 mid_initialized = 0;  // ��λֵ�˲���ʼ����־
static uint16 sample_count = 0;    // ����������

void mid_filter(void)
{
    uint16 temp = 0, a = 0, t = 0;
    uint16 mid_index = 0;  //��λ��
	uint16 i = 0; //����ѭ��
    // ������ʱ�����������򣬱����޸�ԭʼ����
    uint16 sort_array[HISTORY_COUNT];  // ʹ�ú궨��ĳ��������Ǳ���
	
    // ���þ�ֵ�˲���ȡ�µĲ���ֵ
    average_filter();
    
    // �����δ��ʼ�����
    if (!mid_initialized)
    {
        // ����ǰ�˲����������ʷ����
        for(a = 0; a < i_num; a++)
        {
            adc_fliter_data[a][sample_count] = adc_fliter_data[a][0];
            result[a] = adc_fliter_data[a][0];
        }
        
        sample_count++;
        
        // ���ɼ����㹻����ʱ����ǳ�ʼ�����
        if (sample_count >= times)
        {
            mid_initialized = 1;
            sample_count = 0;  // ���ü���������ѭ������
        }
    }
    else  // �ѳ�ʼ����ִ����λֵ�˲�
    {
        // ������ʷ��������
        for(a = 0; a < i_num; a++)
        {
            adc_fliter_data[a][sample_count] = adc_fliter_data[a][0];
        }
        
        // ����ѭ������������
        sample_count = (sample_count + 1) % times;
        
        // ��ÿ�����ͨ�����д���
        for(a = 0; a < i_num; a++)
        {
            for(t = 0; t < times; t++)
            {
                sort_array[t] = adc_fliter_data[a][t];
            }
            
            // ð������
            for(i = 0; i < times-1; i++)
            {
                for(t = 0; t < times-i-1; t++)
                {
                    if(sort_array[t] > sort_array[t+1])
                    {
                        temp = sort_array[t];
                        sort_array[t] = sort_array[t+1];
                        sort_array[t+1] = temp;
                    }
                }
            }
            
            // ������λ������
            mid_index = times / 2;  // 5 / 2 = 2  ,sort_array[2]�ǵ�����������λ��
            
            // ȡ��λ����Ϊ���
            result[a] = sort_array[mid_index];
        }
    }
}


//-----------------------------------------------------------------------------
// @brief  	����ÿ����е������Сֵ�����ڶ�̬У׼
// @param   ��
// @return  ��
// @author  ZP
// Sample usage: update_min_max_values();
// ����Ӧ�����仯�����ֹ�һ����Ч��,��ֹ����ֵ"����"ϵͳ,����<10����������,ǿ����С20�Ĳ�ֵ������������
//-----------------------------------------------------------------------------
void update_min_max_values(void)
{
    uint8 i;
    static uint16 update_counter = 0;
    
    // ������΢˥�������Сֵ��ʹϵͳ����Ӧ�����仯
    update_counter++;
    if(update_counter >= 1000)  // ÿ1000�ε���ִ��һ��˥��
    {
        update_counter = 0;
        
        // ��Сֵ��΢���ӣ����ֵ��΢���٣��γɻ���˥��
        for(i = 0; i < SENSOR_COUNT; i++)
        {
            // ��Сֵ����˥��������1%��
            min_value[i] += min_value[i] / 100;
            
            // ���ֵ����˥��������1%��
            if(max_value[i] > min_value[i])  // ȷ�����ֵʼ�մ�����Сֵ
                max_value[i] -= max_value[i] / 100;
        }
    }
    
    // ��̬����ÿ����е���Сֵ�����ֵ
    for(i = 0; i < SENSOR_COUNT; i++)
    {
        // �쳣ֵ��� - ��������쳣����쳣С�������Ǵ��������ϣ�������
        if(result[i] > 4000 || result[i] < 2)
            continue;
            
        // ������Сֵ�����Թ�С��ֵ��������������
        if(result[i] < min_value[i] && result[i] > 1) 
            min_value[i] = result[i];
        
        // �������ֵ
        if(result[i] > max_value[i]) 
            max_value[i] = result[i];
    }
    
    // ȷ�������Сֵ֮�����㹻��࣬������Խӽ�0��ֵ
    for(i = 0; i < SENSOR_COUNT; i++)
    {
        if(max_value[i] - min_value[i] < 20)
        {
            // ������̫С��ǿ������һ��������
            max_value[i] = min_value[i] + 20;
        }
    }
}

//-----------------------------------------------------------------------------
// @brief  	��һ���������
// @param   ��
// @return  ��
// @author  ZP
// Sample usage: normalize_sensors();
//-----------------------------------------------------------------------------
void normalize_sensors(void)
{
    uint8 i;
    // ��ѡ���Թ�һ��������ݽ���ƽ���������ٶ���
    static float last_normalized[SENSOR_COUNT] = {0};
    // ƽ�����ӣ��ɵ�����ֵԽ����ӦԽ�쵫����Խ���ԣ�ֵԽС����ӦԽ������ƽ��
    float smooth_factor = 0.7f; // ������0.6-0.8��Χ�ڵ���������С��ʵ�ʱ���΢��    // ���ȸ��������Сֵ

    // update_min_max_values();
    
    // ��ÿ����н��й�һ������
    for(i = 0; i < SENSOR_COUNT; i++)
    {
        // ��������Сֵ�����Ƿ��㹻�󣬷�ֹ���Խӽ�0��ֵ
        if(max_value[i] - min_value[i] > 20) 
        {
            // ��׼���Թ�һ������ֵӳ�䵽0-100��Χ������100�������ʹ�ã�
            normalized_data[i] = (float)(result[i] - min_value[i]) * 100.0f / (max_value[i] - min_value[i]);
            
            // ��ѡ��ʹ��ƽ����������ӳ�䣬��ǿС�ź���Ӧ(�������С����Сƫ�Ӧ�����У�����ȡ��ƽ����ӳ���ע��)
            // normalized_data[i] = sqrtf(normalized_data[i] / 100.0f) * 100.0f;
        }
        else 
        {
            // ��������Сֵ����̫С�������Ǵ��������ϻ�δ��ȷ��ʼ��
            // ʹ��ԭʼֵ����Ա�����Ϊ�����Ҳ����100����һ����
            normalized_data[i] = (float)result[i] / 36.0f;  // ����ADC���ֵΪ3600����һ����0-100
        }
        
        // ���Ʒ�Χ��0-100֮��
        if(normalized_data[i] > 100.0f) normalized_data[i] = 100.0f;
        if(normalized_data[i] < 0.0f) normalized_data[i] = 0.0f;
    }
    
	//С�����ֲ���������
    for(i = 0; i < SENSOR_COUNT; i++)
    {
        // ��ͨ�˲�ƽ������
        normalized_data[i] = normalized_data[i] * smooth_factor + 
                            last_normalized[i] * (1.0f - smooth_factor);
        
        // ���浱ǰֵ�����´�ƽ��
        last_normalized[i] = normalized_data[i];
    }
}

//-----------------------------------------------------------------------------
// @brief  	�Ľ������λ�ã�ʹ������Ӧ��Ⱥͼ�Ȩ������
// @param   ��
// @return  ����õ���λ��ֵ����Χ-100��100
// @author  ZP
// Sample usage: position = calculate_position_improved();
// ��Ÿ�Ӧ����
//-----------------------------------------------------------------------------

int16 calculate_position_improved(void)
{
    // �ں�����ʼ���������б���
    float weight_outer = 0.15f;   // �����Ȩ��(HL��HR)
    float weight_vertical = 0.15f; // ������Ȩ��(VL��VR)
    float weight_center = 0.0f;  // ���ĵ��Ȩ��(HC)(û����)
    
    float diff_outer = 0;        // ����в�ֵ
    float diff_vertical = 0;     // �����в�ֵ
    
    float sum_outer = 0;         // ����к�ֵ
    float sum_vertical = 0;      // �����к�ֵ
    float center_value = 0;      // �м���ֵ
    
    float ratio_outer = 0;       // ����в�Ⱥ�
    float ratio_vertical = 0;    // �����в�Ⱥ�
    
    float signal_strength = 0;   // �ź�ǿ��ָ��
    static int16 last_pos = 0;   // ��һ��λ��ֵ�������˲�
    static int16 very_last_pos = 0;  // ���ϴ�λ��ֵ�����ڶ����˲�
    static int16 very_very_last_pos = 0;  // �����ϴ�λ��ֵ�����������˲�
    int16 pos = 0;               // ��ǰ����õ���λ��ֵ
    static int16 max_change_rate = 8; // ��������仯�ʣ�Խ��Խ����
    int16 position_change = 0;   // λ�ñ仯��
	
	
	
	
	// λ�ü��㣨�������ĵ�еĹ��ף�
    // ���ĵ��Խ��λ��Խ�ӽ������ߣ�����ֱ�ӽ����ĵ����Ϊλ����������
    float center_correction = 0;
    
    // ������Ե�е� ��ֵ �� ��ֵ
    diff_outer = normalized_data[SENSOR_HL] - normalized_data[SENSOR_HR];
    sum_outer = normalized_data[SENSOR_HL] + normalized_data[SENSOR_HR];
    
    diff_vertical = normalized_data[SENSOR_VL] - normalized_data[SENSOR_VR];
    sum_vertical = normalized_data[SENSOR_VL] + normalized_data[SENSOR_VR];
    
    // ��ȡ�м���ֵ
    center_value = normalized_data[SENSOR_HC];
    
    // �����ź�ǿ��ָ�� -- ���е��ƽ��ֵ
    signal_strength = (sum_outer + sum_vertical + center_value) / 5.0f;
    signal_strength_value = signal_strength; // �����ź�ǿ��ָ��


    // �����Ⱥͣ�ʹ��ƽ�����ɺ�������Ӳ��ֵ���������ٽ�ֵ������������
    // �����ƽ������
    if(sum_outer > 16.0f)
        ratio_outer = diff_outer / sum_outer;
    else if(sum_outer < 3.0f)
        ratio_outer = 0;
    else
        ratio_outer = (diff_outer / sum_outer) * (sum_outer - 3.0f) / 7.0f; // 5-12��Χ�����Թ���
        
    
    // ������ƽ������
    if(sum_vertical > 16.0f)
        ratio_vertical = diff_vertical / sum_vertical;
    else if(sum_vertical < 3.0f)
        ratio_vertical = 0;
    else
        ratio_vertical = (diff_vertical / sum_vertical) * (sum_vertical - 3.0f) / 7.0f; // 5-12��Χ�����Թ���
    
	
	
    // ��������ʶ�� - �������������ж�
    if (track_type == WEIGHT_STRAIGHT || track_type == 4) // 0. ��ǰ��Ϊ����ͨ����ʱ�������ж��������� track_type 4 �����ܷ�
    {    
        // 1. ֱ���������
        if(((normalized_data[SENSOR_VL] > 65.0f && normalized_data[SENSOR_HR] < 35.0f && normalized_data[SENSOR_VR] < 20.0f) || //��ת
                (normalized_data[SENSOR_VR] > 65.0f && normalized_data[SENSOR_HL] < 30.0f && normalized_data[SENSOR_VL] < 20.0f)) &&  //��ת
								normalized_data[SENSOR_HC] < 75.0f && 
                signal_strength > 25.0f && signal_strength < 50.0f) // �����ź�ǿ�ȷ�Χ
        {
            track_type = WEIGHT_RIGHT_ANGLE; // ֱ�����
        }
        // else if (((normalized_data[SENSOR_HC] > 60.0f && normalized_data[SENSOR_HMR] > 88.0f && normalized_data[SENSOR_VL] > 50.0f && normalized_data[SENSOR_VR] > 75.0f) ||  //��ʱ��
        // (normalized_data[SENSOR_HC] > 80.0f && normalized_data[SENSOR_HML] > 80.0f && normalized_data[SENSOR_HMR] < 45.0f && normalized_data[SENSOR_VL] > 75.0f && normalized_data[SENSOR_VR] > 45.0f)) && 
        // track_ten_flag == 1 && signal_strength > 50.0f ) 
        // {
        //     track_type = 2; //ʮ��Բ��
        //     track_ten_flag = 0; 
        //     ten_change_flag = 1;//��Ӧ���뻷����ʱ2s����track_ten_flag=1
				
        // }
        else if((normalized_data[SENSOR_HR] > 70.0f && normalized_data[SENSOR_HC] > 90.0f && ((normalized_data[SENSOR_HR] + normalized_data[SENSOR_VR]) - (normalized_data[SENSOR_HL] + normalized_data[SENSOR_VL]) > 80.0f))  //�һ���
                 && signal_strength > 48.0f )    
        {
            track_type = 3;// ����
        }
    }
    else if (track_type == WEIGHT_RIGHT_ANGLE) // 1. ֱ�����
	{
		if (normalized_data[SENSOR_VL] > 60.0f && normalized_data[SENSOR_VR] < 30.0f )
		{
			track_type_zj = 1; //��ת
		}
		else if (normalized_data[SENSOR_VR] > 60.0f && normalized_data[SENSOR_VL] < 30.0f )
		{
			track_type_zj = 2; //��ת
		}
		
		if (track_type_zj != 0)
		{
			// �ص�ֱ�� - ��ѡ:���� signal_strength < 45.0f �ж�
			if (normalized_data[SENSOR_VR] < 20.0f && normalized_data[SENSOR_VL] < 20.0f ) 
			{
				track_type = WEIGHT_STRAIGHT; 
				track_type_zj = 0;
			}
//			if (signal_strength > 50) // ֱ���ҹս�Բ���������
//			{
//				track_type = WEIGHT_ROUNDABOUT; 
//				// track_type_zj = 0;
//			    // weight_outer = 0.4;  // ����ֱ����Ȩ
//			    // weight_middle = 0.1;
//			    // weight_vertical = 0.1;
//			    // filter_param = track_weights[WEIGHT_STRAIGHT].filter_param;
//			    // max_change_rate = track_weights[WEIGHT_STRAIGHT].max_change_rate;
//			}
		}
	}
    else if (track_type == WEIGHT_CROSS) // 2. ʮ��Բ��
    {
		// ����  
		if (((normalized_data[SENSOR_HC] > 70.0f && normalized_data[SENSOR_VL] > 80.0f && normalized_data[SENSOR_VR] > 70.0f)  || //��ʱ��
					 (normalized_data[SENSOR_HC] > 80.0f && normalized_data[SENSOR_VL] > 70.0f && normalized_data[SENSOR_VR] > 80.0f )) &&
						track_ten_flag == 1 && signal_strength > 50.0f )  //˳ʱ��
		 {
			track_type = WEIGHT_STRAIGHT; //��ֱ��
			track_ten_flag = 0;
			ten_change_flag = 1; //��Ӧ��������ʱ2s����track_ten_flag=1	
		 }
	}
    else if (track_type == WEIGHT_ROUNDABOUT) // 3. ����   
    {
        if(normalized_data[SENSOR_HR] > 80.0f && normalized_data[SENSOR_HL] < 40.0f && track_route == 0)
        {
            // �һ���
            track_route = 1;
						track_route_status = 1;
        }
        // else if(normalized_data[SENSOR_HR] < 30.0f && normalized_data[SENSOR_HL] > 70.0f && track_route == 0)
        // {
        //     // �󻷵�
        //     track_route = 2;
		// 	track_route_status = 1;
        // }
		if(track_route_status == 2 &&(normalized_data[SENSOR_VL] > 30.0f && normalized_data[SENSOR_HL] < 55.0f && normalized_data[SENSOR_HC] < 65.0f && normalized_data[SENSOR_VR] > 75.0f)) //�һ�
		{
//			track_route = 0;
			track_route_status = 3;
//			track_type == WEIGHT_RIGHT_ANGLE; // ����λ��
		}
    }

    // 4. ������0
//   if(normalized_data[SENSOR_HC] < 2.0f && normalized_data[SENSOR_HMR] < 2.0f && normalized_data[SENSOR_HML] < 2.0f)
//   {
//       track_type = WEIGHT_STRAIGHT;
//       track_route = 0;
//       track_route_status = 0;
//	   track_type_zj = 0;
//   }
    
    // �����������ͺ��ź�ǿ�ȵ���Ȩ��
   switch(track_type)
   {
       case WEIGHT_STRAIGHT: // ��ͨ����
           // ʹ��ֱ��Ȩ��
           weight_outer = track_weights[WEIGHT_STRAIGHT].weight_outer;

           weight_vertical = track_weights[WEIGHT_STRAIGHT].weight_vertical;
           filter_param = track_weights[WEIGHT_STRAIGHT].filter_param;
           max_change_rate = track_weights[WEIGHT_STRAIGHT].max_change_rate;
           break;
           
       case WEIGHT_RIGHT_ANGLE: // ֱ�����
           // ʹ��ֱ�����Ȩ��
           weight_outer = track_weights[WEIGHT_RIGHT_ANGLE].weight_outer;

           weight_vertical = track_weights[WEIGHT_RIGHT_ANGLE].weight_vertical;
           filter_param = track_weights[WEIGHT_RIGHT_ANGLE].filter_param;
           max_change_rate = track_weights[WEIGHT_RIGHT_ANGLE].max_change_rate;
           break;
           
       case WEIGHT_CROSS: // ʮ��Բ��
           // ʹ��ʮ��Բ��Ȩ��
           weight_outer = track_weights[WEIGHT_CROSS].weight_outer;

           weight_vertical = track_weights[WEIGHT_CROSS].weight_vertical;
           filter_param = track_weights[WEIGHT_CROSS].filter_param;
           max_change_rate = track_weights[WEIGHT_CROSS].max_change_rate;
           break;
           
       case WEIGHT_ROUNDABOUT: // ����
           // ʹ�û���Ȩ��
           weight_outer = track_weights[WEIGHT_ROUNDABOUT].weight_outer;

           weight_vertical = track_weights[WEIGHT_ROUNDABOUT].weight_vertical;
           filter_param = track_weights[WEIGHT_ROUNDABOUT].filter_param;
           max_change_rate = track_weights[WEIGHT_ROUNDABOUT].max_change_rate;
           break;
       default:
           // ʹ��Ĭ�ϵ�ֱ��Ȩ��
           weight_outer = track_weights[WEIGHT_STRAIGHT].weight_outer;

           weight_vertical = track_weights[WEIGHT_STRAIGHT].weight_vertical;
           filter_param = track_weights[WEIGHT_STRAIGHT].filter_param;
           max_change_rate = track_weights[WEIGHT_STRAIGHT].max_change_rate;
           break;
   }
    
    // ����������������е��ֵ����Сʱ�������Ѿ�ƫ������
    if(sum_outer < 10.0f && sum_vertical < 10.0f && center_value < 10.0f)
    {
//        if(last_pos > 0)
//            return (last_pos + 10);  // ����ƫ��
//        else
//            return (last_pos - 10); // ����ƫ��
		
		return last_pos;
    }
    
    // �����ĵ�д�����ֵʱ����Ϊ�����ӽ����ģ���λ�ý�������
    if(center_value > 60.0f) {
        // ����ϵ���������ĵ��ǿ�ȸ�ʱ������ϵ����
        center_correction = (center_value - 40.0f) / 60.0f * 0.5f;  // �������50%
    }
    
    // �����Ⱥͼ�Ȩƽ������λ��
    pos = (int16)((ratio_outer * weight_outer + 
                   ratio_vertical * weight_vertical) * 100.0f);
    
    // Ӧ�����ĵ������ - ������������
    pos = (int16)(pos * (1.0f - center_correction));
    
    // ���Ʒ�Χ��-100��100֮��
    if(pos > 100) pos = 100;
    if(pos < -100) pos = -100;
    
    // λ�ñ仯�����ƣ���ֹͻ��
    position_change = pos - last_pos;
    if(position_change > max_change_rate)
        pos = last_pos + max_change_rate;
    else if(position_change < -max_change_rate)
        pos = last_pos - max_change_rate;
    
    // Ӧ�õ�ͨ�˲���ƽ��λ�ñ仯
    pos = (int16)(filter_param * pos + (1-filter_param) * last_pos);
    
    // ����ź�ǿ�ȸߣ���ǿ�˲�Ч��
    if(signal_strength > 60.0f) {
        // Ӧ������ƽ���˲�����һ��ƽ��
        pos = (pos + last_pos + very_last_pos) / 3;
    }
		
		
//		if (track_type == 0 && (pos <= 10 || pos >= -10))
//		{
//				speed_count++;
//			
//				if (speed_count >= 12)
//				{
//						track_type = 4;
//						speed_count = 0;
//				}
//		}
//		else
//		{
//				speed_count = 0;
//		}
//		
//		if (track_type == 4 && (pos > 10 || pos < -10))
//		{
//			track_type = 0;
//		}
		
    
    // ������ʷλ��ֵ
    very_very_last_pos = very_last_pos;
    very_last_pos = last_pos;
    last_pos = pos;
    
    return pos;
}



//-----------------------------------------------------------------------------
// @brief  	��ű����߼�����
// @param   ��
// @return  ����������־λ��1��ʾ�Ѵ�������
// @author  ZP
// Sample usage: protection_flag = check_electromagnetic_protection();
//-----------------------------------------------------------------------------
uint8 check_electromagnetic_protection(void)
{
    // �ں�����ʼ���������б���
    uint8 is_out_of_track = 0;    // ����Ƿ����������ı�־λ
    uint16 sum_value = 0;         // ���е��ֵ���ܺ�
    uint16 threshold = 175;       // ��ֵ����Ҫ�������е�ʵ��������������ӣ�
    static uint8 out_of_track_count = 0;    // ������⵽���������Ĵ���������
    static uint8 in_track_count = 0;        // ������⵽�ڹ���ϵĴ���������
    static uint8 protection_triggered = 0;  // ����������־λ��1��ʾ�Ѵ�������
    uint8 i;
    uint8 trigger_reason = 0;     // ��¼����ԭ�����ڵ���
    
    if (Go_Flag == 1)
	{
		// �������е�еĺ�ֵ
		for(i = 0; i < SENSOR_COUNT; i++)
		{
			sum_value += result[i];
		}
		
		// �ж��Ƿ���������������
		// 1. ���е��ֵ�ܺ͹�С��˵��������������
		if(sum_value < threshold)
		{
			is_out_of_track = 1;
			trigger_reason = 1;
		}
		
		// 2. ��һ�����ֵ����С��˵��������������
		if(normalized_data[SENSOR_HL] < 5.0f && normalized_data[SENSOR_VL] < 5.0f && 
		   normalized_data[SENSOR_HC] < 5.0f && normalized_data[SENSOR_VR] < 5.0f && 
		   normalized_data[SENSOR_HR] < 5.0f)
		{
			is_out_of_track = 1;
			trigger_reason = 2;
		}
		
		// 3. λ��ƫ�����˵������ƫ������̫��
		if(position < -90 || position > 90)
		{
			// ֻ�е����ֵ�ܺ�Ҳ��Сʱ���ж�Ϊ������
			if(sum_value < threshold * 2)
			{
				is_out_of_track = 1;
				trigger_reason = 3;
			}
		}
		
		// ��������߼�����ֹżȻ�ĵ�ֵ��������
		if(is_out_of_track)
		{
			out_of_track_count++;
			in_track_count = 0;  // �����ڹ���ϵļ���
			
			if(out_of_track_count >= 50 && !protection_triggered)  // ����5�μ�⵽���������Ŵ�������
			{
				protection_triggered = 1;
				// ����������������������Ϣ�����ڵ���
	////            sprintf(g_TxData, "Protection triggered! Reason: %d, Sum: %d\n", trigger_reason, sum_value);
	////            uart_putstr(UART_4, g_TxData);
			}
		}
		// else
		// {
		//     // ����������������������
		//     in_track_count++;
		//     if(out_of_track_count > 0)
		//         out_of_track_count--;
				
		//     // �Զ��ָ����ƣ�����20�μ�⵽��������������״̬
		//     if(in_track_count >= 20 && protection_triggered)
		//     {
		//         protection_triggered = 0;
		//         out_of_track_count = 0;
		//         in_track_count = 0;
		//         // ��������Զ��ָ�����Ϣ�����ڵ���
		//         // sprintf(g_TxData, "Protection auto reset!\n");
		//         // uart_putstr(UART_4, g_TxData);
		//     }
		// }
		
		return protection_triggered;
	}
	return 0;
}



// ��ʾ��Ŵ���������
void display_electromagnetic_data(void)
{
    // ��ʾԭʼ�˲����ݺ͹�һ������
	
	oled_p6x8str_spi(1,0,"HL:");	
	oled_uint16_spi(3*8,0,result[SENSOR_HL]);
	oled_p6x8str_spi(9*8,0,"N:");	
	oled_printf_float_spi(11*8,0,normalized_data[SENSOR_HL],3,2);
	
	oled_p6x8str_spi(1,1,"VL:");	
	oled_uint16_spi(3*8,1,result[SENSOR_VL]);
	oled_p6x8str_spi(9*8,1,"N:");	
	oled_printf_float_spi(11*8,1,normalized_data[SENSOR_VL],3,2);
	
	oled_p6x8str_spi(1,2,"HC:");	
	oled_uint16_spi(3*8,2,result[SENSOR_HC]);
	oled_p6x8str_spi(9*8,2,"N:");	
	oled_printf_float_spi(11*8,2,normalized_data[SENSOR_HC],3,2);

	oled_p6x8str_spi(1,3,"VR:");	
	oled_uint16_spi(3*8,3,result[SENSOR_VR]);
	oled_p6x8str_spi(9*8,3,"N:");	
	oled_printf_float_spi(11*8,3,normalized_data[SENSOR_VR],3,2);
	
	oled_p6x8str_spi(1,4,"HR:");	
	oled_uint16_spi(3*8,4,result[SENSOR_HR]);
	oled_p6x8str_spi(9*8,4,"N:");	
	oled_printf_float_spi(11*8,4,normalized_data[SENSOR_HR],3,2);

//    
//    // ��ʾλ�úͲ�Ⱥ�����
//    ips114_showstr_simspi(0,7,"Pos:");
//    ips114_showint16_simspi(5*8, 7, position);
//    
//    // ��ʾ����״̬
//    ips114_showstr_simspi(10*8,7,"P:");
//    ips114_showuint8_simspi(12*8, 7, protection_flag);
} 