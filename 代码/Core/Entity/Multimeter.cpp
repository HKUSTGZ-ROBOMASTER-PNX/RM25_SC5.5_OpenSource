#include "Multimeter.hpp"
#include "stm32g4xx_ll_dma.h"

void Multimeter::Init()
{
    bsp_adc1_init();

    bsp_adc1_DMA_init(m_adc1_raw_data_buffer);

    for (int i = 0; i < 5; i++)
    {
        m_filter[i].Init();
    }

    m_filter[0].SetUpdatePeriod(1);
    m_filter[0].SetTau(0.01f);

    m_filter[1].SetUpdatePeriod(1);
    m_filter[1].SetTau(0.01f);

    m_filter[2].SetUpdatePeriod(1);
    m_filter[2].SetTau(0.004f);

    m_filter[3].SetUpdatePeriod(1);
    m_filter[3].SetTau(0.0075f);

    m_filter[4].SetUpdatePeriod(1);
    m_filter[4].SetTau(0.01f);
}

float test_in_current = 0.0f;
float test_out_current = 0.0f;
float test_in_voltage = 0.0f;
float test_out_voltage = 0.0f;

void Multimeter::Calu()
{

    // V5_K
    //
    //	m_dcdc_in_current = 0.0141791F*m_adc1_raw_data_buffer[1]-28.9897F+0.04F;
    //   m_out_current  = 0.0143329F*m_adc1_raw_data_buffer[4]-29.3045F+0.03F;
    //
    //	m_in_current = 0.0143366F*m_adc1_raw_data_buffer[2]-29.3295F;
    //    m_out_voltage = 0.00794972F*m_adc1_raw_data_buffer[0]+0.02349f;
    //   m_in_voltage = 0.00792925F*m_adc1_raw_data_buffer[3]+0.0904128f;

    // V5_L
    //
    //	m_dcdc_in_current = 0.0144579F*m_adc1_raw_data_buffer[1]-29.4135F;
    //   m_out_current  = 0.0140015F*m_adc1_raw_data_buffer[4]-28.6114F;
    //
    //	m_in_current = 0.0143366F*m_adc1_raw_data_buffer[2]-29.2874F;
    //    m_out_voltage = 0.00795321F*m_adc1_raw_data_buffer[0]+0.0204241F;
    //   m_in_voltage = 0.00796115F*m_adc1_raw_data_buffer[3]+0.0144995f;

    // V5_M

    //		m_dcdc_in_current = 0.0141791F*m_adc1_raw_data_buffer[1]-28.9897F+0.04F;
    //  m_out_current  = 0.0143329F*m_adc1_raw_data_buffer[4]-29.3045F+0.03F;
    //
    //	m_in_current = 0.0143366F*m_adc1_raw_data_buffer[2]-29.3295F;
    //   m_out_voltage = 0.00794972F*m_adc1_raw_data_buffer[0]+0.02349f;
    //  m_in_voltage = 0.00792925F*m_adc1_raw_data_buffer[3]+0.0904128f;

    // V5_N
    //	m_dcdc_in_current = 0.0143512F*m_adc1_raw_data_buffer[1]-29.3356F;
    //   m_out_current  = 0.0144928F*m_adc1_raw_data_buffer[4]-29.6232F;
    //
    //	m_in_current = 0.0143422F*m_adc1_raw_data_buffer[2]-29.3866F;
    //    m_out_voltage = 0.00796065F*m_adc1_raw_data_buffer[0]+0.0246844f;
    //   m_in_voltage = 0.00796811F*m_adc1_raw_data_buffer[3]-0.022265f;

    // V5_O
    //	m_dcdc_in_current = 0.0144173F*m_adc1_raw_data_buffer[1]-29.4742F;
    //   m_out_current  = 0.0142857F*m_adc1_raw_data_buffer[4]-29.2F;
    //
    //	m_in_current = 0.0142351F*m_adc1_raw_data_buffer[2]-29.1212F;
    //    m_out_voltage = 0.00794982F*m_adc1_raw_data_buffer[0]+0.027079f;
    //   m_in_voltage = 0.00795164F*m_adc1_raw_data_buffer[3]+0.00813981f;

    // V5_P
    m_dcdc_in_current = 0.00168764F * m_adc1_raw_data_buffer[1] - 27.5183F;
    m_out_current = 0.00178954F * m_adc1_raw_data_buffer[4] - 29.1036F;

    m_in_current = 0.0018182f * m_adc1_raw_data_buffer[2] - 29.7134f;
    m_out_voltage = 0.000995368F * m_adc1_raw_data_buffer[0] + 0.0301716f;
    m_in_voltage = 0.000994232F * m_adc1_raw_data_buffer[3] + 0.0551241f;

    m_filter[0].m_Input = m_out_voltage;
    m_filter[0].Update();
    m_out_voltage = m_filter[0].m_Out;

    m_filter[1].m_Input = m_out_current;
    m_filter[1].Update();
    test_out_current = m_out_current = m_filter[1].m_Out;

    m_filter[2].m_Input = m_in_voltage;
    m_filter[2].Update();
    m_in_voltage = m_filter[2].m_Out;

    m_filter[3].m_Input = m_in_current;
    m_filter[3].Update();
    test_in_current = m_in_current = m_filter[3].m_Out;

    m_filter[4].m_Input = m_dcdc_in_current;
    m_filter[4].Update();
    m_dcdc_in_current = m_filter[4].m_Out;

    m_in_power = m_in_current * m_in_voltage;
    m_out_power = m_out_voltage * m_out_current;
    m_dcdc_in_power = m_in_voltage * m_dcdc_in_current;
}