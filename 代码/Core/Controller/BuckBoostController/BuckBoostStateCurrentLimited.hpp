#ifndef BUCKBOOSTSTATE_CURRENTLIMITED_HPP
#define BUCKBOOSTSTATE_CURRENTLIMITED_HPP

#include "StateMachine.hpp"
#include "BuckBoostController.hpp"
#include "Pid.hpp"

class BuckBoostStateCurrentLimited : public State<BuckBoostController>
{
private:
    int8_t m_cap_state;
    float m_current_set;
public:
    Pid pidPower;

    BuckBoostStateCurrentLimited(){;}

    virtual void Init(BuckBoostController* pOwner);
    virtual void Enter(BuckBoostController* pOwner);
    virtual void Execute(BuckBoostController* pOwner);
    virtual void Exit(BuckBoostController* pOwner);

    static BuckBoostStateCurrentLimited* Instance()
    {
        static BuckBoostStateCurrentLimited instance;
        return &instance;
    }
};

#endif
