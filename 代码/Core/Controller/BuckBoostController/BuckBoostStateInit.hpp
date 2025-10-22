#ifndef BUCKBOOSTSTATE_INIT_HPP
#define BUCKBOOSTSTATE_INIT_HPP

#include "StateMachine.hpp"
#include "BuckBoostController.hpp"
#include "Pid.hpp"

class BuckBoostStateInit : public State<BuckBoostController>
{
private:
    int m_init_count;
	bool m_init_flag;
public:
    BuckBoostStateInit(){;}

    virtual void Init(BuckBoostController* pOwner);
    virtual void Enter(BuckBoostController* pOwner);
    virtual void Execute(BuckBoostController* pOwner);
    virtual void Exit(BuckBoostController* pOwner);

    static BuckBoostStateInit* Instance()
    {
        static BuckBoostStateInit instance;
        return &instance;
    }
};

#endif
