#include "BuckBoostStateInit.hpp"
#include "BuckBoostStatePowerLimited.hpp"
#include "Math.hpp"

bool init_restart_flag = false;

void BuckBoostStateInit::Init(BuckBoostController* pOwner)
{
  m_init_count = 500;//20 = 0.001s
	m_init_flag = false;
}
     
void BuckBoostStateInit::Enter(BuckBoostController* pOwner)
{
    
}

void BuckBoostStateInit::Execute(BuckBoostController* pOwner)
{ 
	float vout = pOwner->multimeter.m_out_voltage;
	
	if(m_init_flag == false)
	{
		m_init_flag = true;
		pOwner->m_last_pwm_set = pOwner->MinVoltageSetToPWMSet(vout);
	}

	if(vout<3.0f)
	{
		vout = 3.0f;
	}
	else if(vout>27.0f)
	{
		vout = 27.0f;
	}
	
	if(vout<(pOwner->multimeter.m_in_voltage))
	{
		pOwner->m_pwm_set  = pOwner->MinVoltageSetToPWMSet(vout) + 100;
	}
	else
	{
		pOwner->m_pwm_set  = pOwner->MaxVoltageSetToPWMSet(1.0f/vout) + 100;
	}
	
	if(m_init_count == 0)
	{
		bsp_hrtim_on();
		
			if(init_restart_flag == false)
			{
				 init_restart_flag = true;
				 bsp_hrtim_burst_on();
			}
		
		pOwner->buckBoostFsm.ChangeState(BuckBoostStatePowerLimited::Instance());
	}
	
	m_init_count--;
}

void BuckBoostStateInit::Exit(BuckBoostController* pOwner)
{
    ;
}
