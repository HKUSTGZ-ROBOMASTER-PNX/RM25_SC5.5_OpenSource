#ifndef BUCKBOOSTSTATE_POWERLIMITED_HPP
#define BUCKBOOSTSTATE_POWERLIMITED_HPP

#include "StateMachine.hpp"
#include "BuckBoostController.hpp"
#include "Pid.hpp"

class BuckBoostStatePowerLimited : public State<BuckBoostController>
{
private:
    float m_power_set;
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
