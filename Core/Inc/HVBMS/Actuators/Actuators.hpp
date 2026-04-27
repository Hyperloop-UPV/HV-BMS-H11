#pragma once
#include "HVBMS/Actuators/Contactor.hpp"

class Actuators {
private:
    static Contactor contactor_low; // 0
    static Contactor contactor_high; // 1
    static Contactor contactor_precharge; //2
    static Contactor contactor_discharge; //3

    static uint8_t contactors_timeout_id;

public:
    static void init();
    static void open_HV();
    static bool is_HV_open();

    static void close_HV();
    static bool is_HV_closed();

    static void start_precharge();
    static bool is_precharging();
    static void toggle_operational_led();
    
    static bool& get_contactor_low_state();
    static bool& get_contactor_high_state();
    static bool& get_contactor_precharge_state();
    static bool& get_contactor_discharge_state();

    static void update_contactors();
};
