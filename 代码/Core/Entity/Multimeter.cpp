#include "Multimeter.hpp"
#include "stm32g4xx_ll_dma.h"

void Multimeter::Init()
{
    bsp_adc1_init();
    bsp_adc2_init();
    bsp_adc3_init();

    bsp_adc1_DMA_init(m_adc1_raw_data_buffer);
    bsp_adc2_DMA_init(m_adc2_raw_data_buffer);
    bsp_adc3_DMA_init(m_adc3_raw_data_buffer);

    for (int i = 0; i < 5; i++)
    {
        m_filter[i].Init();
    }

    m_filter[0].SetUpdatePeriod(1);
    m_filter[0].SetTau(0.01f);

    m_filter[1].SetUpdatePeriod(1);
    m_filter[1].SetTau(0.0005f);

    m_filter[2].SetUpdatePeriod(1);
    m_filter[2].SetTau(0.004f);

    m_filter[3].SetUpdatePeriod(1);
    m_filter[3].SetTau(0.00015f);

    m_filter[4].SetUpdatePeriod(1);
    m_filter[4].SetTau(0.01f);
}

float test_in_current = 0.0f;
float test_out_current = 0.0f;
float test_in_voltage = 0.0f;
float test_out_voltage = 0.0f;

void Multimeter::ADC1Calu()
{
    m_in_voltage = 0.000995368F * m_adc1_raw_data_buffer[1] + 0.0301716f;

    m_filter[0].m_Input = m_in_voltage;
    m_filter[0].Update();
    m_in_voltage = m_filter[0].m_Out;
    
    m_in_current = 0.0018182f * m_adc1_raw_data_buffer[0] - 29.7134f;

    m_filter[3].m_Input = m_in_current;
    m_filter[3].Update();
    test_in_current = m_in_current = m_filter[3].m_Out;

    m_in_power = m_in_current * m_in_voltage;
    m_out_power = m_out_voltage * m_out_current;
    m_dcdc_in_power = m_in_voltage * m_dcdc_in_current;
}

void Multimeter::ADC2Calu()
{
    m_dcdc_in_current = 0.00168764F * m_adc2_raw_data_buffer[1] - 27.5183F;
    m_out_voltage = 0.000994232F * m_adc2_raw_data_buffer[0] + 0.0551241f;

    m_filter[4].m_Input = m_dcdc_in_current;
    m_filter[4].Update();
    m_dcdc_in_current = m_filter[4].m_Out;

     m_filter[2].m_Input = m_out_voltage;
    m_filter[2].Update();
    m_out_voltage = m_filter[2].m_Out;
}

void Multimeter::ADC3Calu()
{
    m_out_current = 0.00178954F * m_adc3_raw_data_buffer[0] - 29.1036F;

    m_filter[1].m_Input = m_out_current;
    m_filter[1].Update();
    test_out_current = m_out_current = m_filter[1].m_Out;
}
