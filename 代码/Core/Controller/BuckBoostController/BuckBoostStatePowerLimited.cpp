#include "BuckBoostStatePowerLimited.hpp"
#include "BuckBoostStateCurrentLimited.hpp"
#include "Math.hpp"

BuckBoostStatePowerLimited *debug;

void BuckBoostStatePowerLimited::Init(BuckBoostController *pOwner)
{
	debug = this;
	pidPower.kp = 0.3f;
	pidPower.ki = 0.000058f;
	pidPower.kd = 0.05f;
	pidPower.maxOut = 40.0f;
	pidPower.maxIOut = 0.6f;

	m_control_freq_reciprocal = 1.0f / 50000.0f;
	m_capacitance_reciprocal = 1.0f / 5.556f;
}

uint8_t over_current_count = 0;

void BuckBoostStatePowerLimited::Enter(BuckBoostController *pOwner)
{
	pidPower.Clear();
	over_current_count = 0;
	// pOwner->m_pwm_set = 5000;
}

float cap_current = 0.0f;
float cap_power = 0.0f;
float debug_1 = 0.0f;
float debug_2 = 0.0f;
float debug_3 = 0.0f;

void BuckBoostStatePowerLimited::Execute(BuckBoostController *pOwner)
{
	m_power_set = pOwner->m_max_power;

	if (pOwner->multimeter.m_out_current < 0.0f)
	{
		cap_current = -pOwner->multimeter.m_out_current;
	}
	else
	{
		cap_current = pOwner->multimeter.m_out_current;
	}

	cap_power = pOwner->multimeter.GetOutPower();

	if (cap_current > 20.5f)
	{
		over_current_count++;
	}
	else
	{
		over_current_count = 0;
	}

	if (over_current_count == 10)
	{
		pOwner->buckBoostFsm.ChangeState(BuckBoostStateCurrentLimited::Instance());
	}

	pidPower.ref = m_power_set;
	pidPower.fdb = pOwner->multimeter.GetInputPower();
	pidPower.UpdateResult();

	m_cap_power_set = pOwner->multimeter.GetOutPower() - pOwner->multimeter.GetInputPower() + m_power_set;
	m_cap_voltage_diff = m_cap_power_set * m_control_freq_reciprocal * m_capacitance_reciprocal / pOwner->multimeter.GetOutVoltage();
	m_pwm_diff = pOwner->DeltaVoltageToDeltaPWM(m_cap_voltage_diff);

	pOwner->m_pwm_set -= (pidPower.result+m_pwm_diff);
	
//	debug_1 = pidPower.result;
//	debug_2 = m_pwm_diff;
//	debug_3 = pOwner->m_pwm_set;
}

void BuckBoostStatePowerLimited::Exit(BuckBoostController *pOwner)
{
	;
}
