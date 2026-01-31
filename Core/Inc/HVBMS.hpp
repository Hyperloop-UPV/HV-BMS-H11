#pragma once

#include "ST-LIB.hpp"

using ST_LIB::DigitalOutputDomain;


constexpr DigitalOutputDomain::DigitalOutput led_PG8{ST_LIB::PG8};
constexpr DigitalOutputDomain::DigitalOutput led_PG7{ST_LIB::PG7};
constexpr DigitalOutputDomain::DigitalOutput contactor_PG14{ST_LIB::PG14};
constexpr DigitalOutputDomain::DigitalOutput contactor_PG12{ST_LIB::PG12};
constexpr DigitalOutputDomain::DigitalOutput contactor_PD4{ST_LIB::PD4};
constexpr DigitalOutputDomain::DigitalOutput contactor_PF4{ST_LIB::PF5};
constexpr DigitalOutputDomain::DigitalOutput sdc_PA11{ST_LIB::PA11};

namespace HVBMS{
    namespace Global {
        inline DigitalOutputDomain::Instance *operational_led;
        inline DigitalOutputDomain::Instance *fault_led;
        inline DigitalOutputDomain::Instance *contactor_low;
        inline DigitalOutputDomain::Instance *contactor_high;
        inline DigitalOutputDomain::Instance *contactor_precharge;
        inline DigitalOutputDomain::Instance *contactor_discharge;
        inline DigitalOutputDomain::Instance *sdc_obccu;
    };      
};
