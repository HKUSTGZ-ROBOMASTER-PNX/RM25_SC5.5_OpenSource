#include "BuckBoostStateCurrentLimited.hpp"
#include "BuckBoostStatePowerLimited.hpp"
#include "Math.hpp"

void BuckBoostStateCurrentLimited::Init(BuckBoostController *pOwner)
{
    pidPower.kp = 0.5f;
    pidPower.ki = 0.0f;
    pidPower.kd = 0.001f;
    pidPower.maxOut = 5.0f;
    pidPower.maxIOut = 0.0f;
}

uint32_t power_check_count = 0;

void BuckBoostStateCurrentLimited::Enter(BuckBoostController *pOwner)
{
    pidPower.Clear();

    if (pOwner->multimeter.GetOutPower() < 0.0f)
    {
        m_cap_state = -1;
    }
    else if (pOwner->multimeter.GetOutPower() > 0.0f)
    {
        m_cap_state = 1;
    }

    m_current_set = 20.0f;
    power_check_count = 0;
}

void BuckBoostStateCurrentLimited::Execute(BuckBoostController *pOwner)
{
    float cap_voltage = pOwner->multimeter.GetOutVoltage();
    float input_power = pOwner->multimeter.GetInputPower();

    if (m_cap_state == -1)
    {
        pidPower.ref = m_current_set * cap_voltage;
        pidPower.fdb = -pOwner->multimeter.GetOutPower();
        pidPower.UpdateResult();

        pOwner->m_pwm_set -= (-pidPower.result);

        if (input_power - pOwner->m_max_power < 1.0f)
        {
            power_check_count++;
        }
        else
        {
            power_check_count = 0;
        }

        if (power_check_count == 10)
        {
            pOwner->buckBoostFsm.ChangeState(BuckBoostStatePowerLimited::Instance());
        }
    }
    else if (m_cap_state == 1)
    {
        pidPower.ref = m_current_set * cap_voltage;
        pidPower.fdb = pOwner->multimeter.GetOutPower();
        pidPower.UpdateResult();

        pOwner->m_pwm_set -= pidPower.result;

        if (pOwner->m_max_power - input_power < 1.0f)
        {
            power_check_count++;
        }
        else
        {
            power_check_count = 0;
        }

        if (power_check_count == 10)
        {
            pOwner->buckBoostFsm.ChangeState(BuckBoostStatePowerLimited::Instance());
        }
    }
}

void BuckBoostStateCurrentLimited::Exit(BuckBoostController *pOwner)
{
}
