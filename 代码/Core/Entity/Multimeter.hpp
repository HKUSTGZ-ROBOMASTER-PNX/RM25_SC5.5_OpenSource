#ifndef MULTIMETER_HPP
#define MULTIMETER_HPP

#include "bsp_adc.h"
#include "math_first_order_filter.h"

class Multimeter
{
public:
    FirstOrderFilter m_filter[5];
    uint16_t m_adc1_raw_data_buffer[2];
    uint16_t m_adc2_raw_data_buffer[2];
    uint16_t m_adc3_raw_data_buffer[2];
    
    float m_in_current;
    float m_out_current;
    float m_dcdc_in_current;
    float m_in_voltage;
    float m_out_voltage;
    float m_dcdc_in_power;
    float m_in_power;
    float m_out_power;
	float m_eff;

    Multimeter(/* args */){;}
    ~Multimeter(){;}

    void Init();
    void ADC1Calu();
    void ADC2Calu();
    void ADC3Calu();
		
    float GetInputPower(){return m_in_power;}
    float GetOutVoltage(){return m_out_voltage;}
    float GetOutCurrent(){return m_out_current;}
    float GetOutPower(){return m_out_power;}
};




#endif
