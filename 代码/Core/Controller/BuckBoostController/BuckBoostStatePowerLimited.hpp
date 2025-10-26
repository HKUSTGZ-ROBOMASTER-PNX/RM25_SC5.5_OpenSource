#ifndef BUCKBOOSTSTATE_POWERLIMITED_HPP
#define BUCKBOOSTSTATE_POWERLIMITED_HPP

#include "StateMachine.hpp"
#include "BuckBoostController.hpp"
#include "Pid.hpp"

class BuckBoostStatePowerLimited : public State<BuckBoostController>
{
private:
    float m_power_set;
    float m_cap_power_set;
    float m_cap_voltage_diff;
    float m_control_freq_reciprocal;
    float m_capacitance_reciprocal;
    float m_pwm_diff;

public:
    Pid pidPower;

    BuckBoostStatePowerLimited(){;}

    virtual void Init(BuckBoostController* pOwner);
    virtual void Enter(BuckBoostController* pOwner);
    virtual void Execute(BuckBoostController* pOwner);
    virtual void Exit(BuckBoostController* pOwner);

    static BuckBoostStatePowerLimited* Instance()
    {
        static BuckBoostStatePowerLimited instance;
        return &instance;
    }
};

#endif
