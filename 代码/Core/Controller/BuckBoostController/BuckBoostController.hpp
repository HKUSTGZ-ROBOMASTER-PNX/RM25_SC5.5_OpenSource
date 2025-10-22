#ifndef BUCKBOOSTCONTROLLER_HPP
#define BUCKBOOSTCONTROLLER_HPP

#include <math.h>
#include "StateMachine.hpp"
#include "math_first_order_filter.h"
#include "bsp_HRTIM.h"
#include "bsp_led.h"
#include "Multimeter.hpp"
#include "bsp_can.h"

class BoardPacket
{
public:
    BoardPacket(uint8_t _id, uint8_t _length) : m_id(_id), m_length(_length) {}
    ~BoardPacket() {}

    bool m_receive_flag;
    bool m_send_flag;

    uint8_t m_id;
    uint8_t m_length;

    virtual void Send() = 0;
    virtual void Receive(uint8_t *_buffer) = 0;
};

struct SuperCapFdbStruct
{
    uint8_t cap_state_fdb;
    uint8_t cap_voltage_x5;
    int16_t input_power_x100;
    int16_t cap_power_x100;
    uint16_t battery_consumption;

} __attribute__((packed));

union SuperCapFdbUnion
{
    SuperCapFdbStruct packet_struct;
    uint8_t packet_data[8];
};

class SuperCapFdbPacket : public BoardPacket
{
public:
    uint8_t cap_state_fdb;
    float cap_voltage;
    float input_power;
    float cap_power;
    float battery_consumption;

    SuperCapFdbPacket();
    ~SuperCapFdbPacket() { ; }

    void Send() override;
    void Receive(uint8_t *_buffer) override;
};

struct SuperCapSetStruct
{
    uint8_t cap_state_set;
    uint8_t fly_extra_set;
    float power_limit_set;
    uint8_t raw_power_limit;
    uint8_t battery_consumption;
} __attribute__((packed));

union SuperCapSetUnion
{
    SuperCapSetStruct packet_struct;
    uint8_t packet_data[8];
};

class SuperCapSetPacket : public BoardPacket
{
public:
    uint8_t cap_state_set;
    uint8_t fly_extra_set;
    float power_limit_set;
    uint8_t raw_power_limit;
    uint8_t battery_consumption;

    SuperCapSetPacket();
    ~SuperCapSetPacket() { ; }

    void Send() override;
    void Receive(uint8_t *_buffer) override;
};

// Forward declaration
class BuckBoostController;

class BuckBoostFsm : public StateMachine<BuckBoostController>
{
public:
    BuckBoostFsm(BuckBoostController *_pOwner) : StateMachine<BuckBoostController>(_pOwner) {}
    void HandleInput();
    void Init() override;
};

enum class BuckBoostSwitch
{
    Off = 0,
    On = 1
};

class BuckBoostController
{
private:
    static float m_max_voltage_set;
    static float m_min_voltage_set;
    static float m_re_max_voltage_set;

    void SuperCapacitorUpdate();
    void NoBatteryCheck();

public:
    BuckBoostController();

    BuckBoostFsm buckBoostFsm;
    Multimeter multimeter;

    SuperCapFdbPacket supercap_fdb_packet;
    SuperCapSetPacket supercap_set_packet;

    float m_pwm_set;
    float m_last_pwm_set;
    float m_max_power;
    float m_max_current_set = 14.0f;

    BuckBoostSwitch m_switch;

    float MinVoltageSetToPWMSet(float voltage_set);
    float MaxVoltageSetToPWMSet(float voltage_set);

    void Init();
    void Update();
};

#endif
