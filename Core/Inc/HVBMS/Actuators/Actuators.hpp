#pragma once
#include "HVBMS/Actuators/Contactor.hpp"

class Actuators {
    private:
        static Contactor contactor_low;
        static Contactor contactor_high;
        static Contactor contactor_precharge;
        static Contactor contactor_discharge;


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
};
