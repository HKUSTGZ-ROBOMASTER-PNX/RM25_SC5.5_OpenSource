#include "BuckBoostController.hpp"
#include "BuckBoostStatePowerLimited.hpp"
#include "BuckBoostStateCurrentLimited.hpp"
#include "BuckBoostStateInit.hpp"
#include "stm32g4xx_ll_tim.h"
#include "stm32g4xx_ll_adc.h"

float BuckBoostController::m_max_voltage_set = 23.0f;
float BuckBoostController::m_re_max_voltage_set = 1.0f;
float BuckBoostController::m_min_voltage_set = 2.5f;

static int in_voltage_wait_count = 10000 * 5;
static float max_pwm_set = 0;
static float min_pwm_set = 0;
static float begin_voltage = 0.0f;
static float in_voltage = 24.0f;

static uint32_t debug_pwm_get = 0;
static float debug_input_current = 0.0f;
static float debug_input_v = 0.0f;
static float debug_output_v = 0.0f;

BuckBoostController::BuckBoostController() : buckBoostFsm(this), m_max_power(44.0f)
{
}

void BuckBoostController::Init()
{
	multimeter.Init();
	bsp_hrtim_init();
	bsp_hrtim_off();

	buckBoostFsm.Init();

	m_last_pwm_set = m_pwm_set = 2000;

	last_burst_mode = burst_mode = 0;

	m_switch = BuckBoostSwitch::Off;

	LL_TIM_EnableCounter(TIM8);

	// m_max_voltage_set = 23.0f;
	// m_re_max_voltage_set = 1.0f / m_max_voltage_set;

	max_pwm_set = 18000;
	min_pwm_set = 1000;
}

bool debug_flag = true;
bool restart_flag = false;

static float max_delta = 40.0f;

int debug_pwm_set = 2000;
static uint32_t no_battery_switch_off_keep_count = 0;

void BuckBoostController::Update()
{
	if (in_voltage_wait_count > 0)
	{
		m_switch = BuckBoostSwitch::Off;
		m_pwm_set = MinVoltageSetToPWMSet(multimeter.m_out_voltage);
		restart_flag = false;
		in_voltage_wait_count--;
	}
	else
	{
		if (no_battery_switch_off_keep_count == 0)
		{
			m_switch = BuckBoostSwitch::On;
			bsp_led_off();
		}
		else
		{
			no_battery_switch_off_keep_count--;
		}

		// in_voltage = multimeter.m_in_voltage;

		in_voltage = 24.0f;

		if (in_voltage < 5.0f)
		{
			in_voltage = 5.0f;
		}

		if (in_voltage > 30.0f)
		{
			in_voltage = 30.0f;
		}

		// min_pwm_set = MaxVoltageSetToPWMSet(m_re_max_voltage_set);
		// max_pwm_set = MinVoltageSetToPWMSet(m_min_voltage_set);

		// NoBatteryCheck();
	}

	if (m_switch == BuckBoostSwitch::Off)
	{
		bsp_led_on();
		bsp_hrtim_off();
	}
	else
	{
		if (debug_flag == true)
		{
			m_pwm_set = debug_pwm_set;
			bsp_hrtim_on();

			if (restart_flag == false)
			{
				restart_flag = true;
				bsp_hrtim_burst_on();
				burst_mode = 0;
				bsp_set_hrtim_mode(burst_mode);
			}
		}
		else
		{
			buckBoostFsm.Update();
		}

		if (m_pwm_set < min_pwm_set)
		{
			m_pwm_set = min_pwm_set;
		}
		else if (m_pwm_set > max_pwm_set)
		{
			m_pwm_set = max_pwm_set;
		}

		if (m_pwm_set - m_last_pwm_set > max_delta)
		{
			m_pwm_set = m_last_pwm_set + max_delta;
		}
		else if (m_last_pwm_set - m_pwm_set > max_delta)
		{
			m_pwm_set = m_last_pwm_set - max_delta;
		}

		// if (m_pwm_set < 16000.0f)
		// {
		// 	m_pwm_set = 16000.0f;
		// }
		// else if (m_pwm_set > 29500.0f)
		// {
		// 	m_pwm_set = 29500.0f;
		// }

		if (m_pwm_set < 14694.0f)
		{
			burst_mode = 0;
		}
		else if (m_pwm_set >= 14694.0f && m_pwm_set < 16000.0f)
		{
			burst_mode = 1;
		}
		else if (m_pwm_set >= 16000.0f && m_pwm_set < 17422.0f)
		{
			burst_mode = 2;
		}
		else if (m_pwm_set >= 17422.0f)
		{
			burst_mode = 3;
		}

		/*

		if (m_pwm_set<14694.0f)
		{
			burst_mode = 0;
		}
		else if (m_pwm_set >= 14694.0f && m_pwm_set < 16000.0f)
		{
			burst_mode = 1;
		}
		else if (m_pwm_set >= 16000.0f && m_pwm_set < 17422.0f)
		{
			burst_mode = 2;
		}
		else if (m_pwm_set >= 17422.0f)
		{
			burst_mode = 3;
		}
		*/

		if (last_burst_mode != burst_mode)
		{
			last_burst_mode = burst_mode;
			bsp_set_hrtim_mode(burst_mode);
		}

		bsp_hrtim_set((int)m_pwm_set);

		m_last_pwm_set = m_pwm_set;
	}
}

volatile bool debug_check_flag = true;

volatile float min_in_power_no_judg = 3.5f;
volatile float min_in_power = -2.0f;
volatile float min_dcdc_in_power = -1.5f;
volatile float cap_out_energy = 0.0f;
volatile float cap_out_energy_no_judg = 0.0f;

volatile float min_battery_voltage = 18.0f;
volatile float min_cap_voltage = 18.0f;
volatile int no_battery_in_voltage_time_count = 0;
volatile int no_cap_low_voltage_time_count = 0;
volatile int judgement_off_count = 0;

void BuckBoostController::NoBatteryCheck()
{
	if (multimeter.m_in_power < min_in_power)
	{
		if (multimeter.m_dcdc_in_power < min_dcdc_in_power)
		{
			cap_out_energy += multimeter.m_dcdc_in_power * 0.00004f;
		}
		else
		{
			cap_out_energy = 0.0f;
		}

		if (multimeter.m_out_voltage < min_cap_voltage)
		{
			no_cap_low_voltage_time_count++;
		}
		else
		{
			no_cap_low_voltage_time_count = 0;
		}
	}
	else if (multimeter.m_in_power < min_in_power_no_judg)
	{
		if (multimeter.m_dcdc_in_power < min_dcdc_in_power)
		{
			cap_out_energy_no_judg += multimeter.m_dcdc_in_power * 0.00004f;
		}
		else
		{
			cap_out_energy_no_judg = 0.0f;
		}

		if (multimeter.m_out_voltage < min_cap_voltage)
		{
			no_cap_low_voltage_time_count++;
		}
		else
		{
			no_cap_low_voltage_time_count = 0;
		}
	}
	else
	{
		no_cap_low_voltage_time_count = 0;
		cap_out_energy = 0;
	}

	if (multimeter.m_in_voltage < 20.0f)
	{
		no_battery_in_voltage_time_count++;
	}
	else
	{
		no_battery_in_voltage_time_count = 0;
	}

	if (no_cap_low_voltage_time_count > 10000 || no_battery_in_voltage_time_count > 10000 || cap_out_energy < -30.0f || cap_out_energy_no_judg < -20.0f || judgement_off_count > 5000)
	{
		if (debug_check_flag == true)
		{
			bsp_led_on();
		}
		else
		{
			no_battery_switch_off_keep_count = 20000;
			m_switch = BuckBoostSwitch::Off;
		}
	}

	if (supercap_set_packet.cap_state_set == 0)
	{
		judgement_off_count++;
	}
	else
	{
		judgement_off_count = 0;
	}
}

float BuckBoostController::MinVoltageSetToPWMSet(float voltage_set)
{
	return voltage_set * (in_voltage * 28.9096f - 1358.82f) + 30385.0f;
}

float BuckBoostController::MaxVoltageSetToPWMSet(float re_voltage_set)
{
	return (14799.0f * in_voltage + 9174.0f) * re_voltage_set - 630.0f;
}

float BuckBoostController::DeltaVoltageToDeltaPWM(float _voltage)
{
	return (in_voltage * 28.9096f - 1358.82f) * _voltage;
}

void BuckBoostFsm::HandleInput()
{
	;
}

void BuckBoostFsm::Init()
{
	BuckBoostStateInit::Instance()->Init(m_pOwner);
	BuckBoostStatePowerLimited::Instance()->Init(m_pOwner);
	BuckBoostStateCurrentLimited::Instance()->Init(m_pOwner);
	SetCurrentState(BuckBoostStateInit::Instance());
}

SuperCapFdbPacket::SuperCapFdbPacket() : BoardPacket(0x00, 1)
{
	cap_state_fdb = 0;
	cap_voltage = 0.0f;
	input_power = 0.0f;
	cap_power = 0.0f;
}

void SuperCapFdbPacket::Send()
{
	SuperCapFdbUnion send_union;

	send_union.packet_struct.cap_state_fdb = cap_state_fdb;
	send_union.packet_struct.cap_voltage_x5 = cap_voltage * 5.0f;
	send_union.packet_struct.input_power_x100 = input_power * 100.0f;

	send_union.packet_struct.cap_power_x100 = cap_power * 100.0f;
	send_union.packet_struct.battery_consumption = battery_consumption;

	FDCAN_TxHeaderTypeDef TxHeader1;

	TxHeader1.Identifier = m_id;
	TxHeader1.IdType = FDCAN_STANDARD_ID;
	TxHeader1.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader1.DataLength = FDCAN_DLC_BYTES_8;
	TxHeader1.BitRateSwitch = FDCAN_BRS_OFF;
	TxHeader1.FDFormat = FDCAN_CLASSIC_CAN;
	TxHeader1.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
	TxHeader1.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader1.MessageMarker = 0;

	HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader1, &send_union.packet_data[0]);
}

void SuperCapFdbPacket::Receive(uint8_t *_buffer)
{
}

SuperCapSetPacket::SuperCapSetPacket() : BoardPacket(0x14, 1)
{
	cap_state_set = 1;
	fly_extra_set = 0;
	power_limit_set = 44.0f;
}

void SuperCapSetPacket::Send()
{
}

void SuperCapSetPacket::Receive(uint8_t *_buffer)
{
	SuperCapSetUnion receive_union;

	for (uint8_t i = 0; i < 6; i++)
	{
		receive_union.packet_data[i] = _buffer[i];
	}

	cap_state_set = receive_union.packet_struct.cap_state_set;
	fly_extra_set = receive_union.packet_struct.fly_extra_set;
	power_limit_set = receive_union.packet_struct.power_limit_set;
}
