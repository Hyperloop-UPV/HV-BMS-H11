#pragma once
#include "ST-LIB.hpp"

using ST_LIB::DigitalOutputDomain;


//constexpr DigitalOutputDomain::DigitalOutput led_PG8{ST_LIB::PG8};
//constexpr DigitalOutputDomain::DigitalOutput led_PG7{ST_LIB::PG7};

constexpr DigitalOutputDomain::DigitalOutput led_PG8{ST_LIB::PE1};
constexpr DigitalOutputDomain::DigitalOutput led_PG7{ST_LIB::PB0};

namespace HVBMS{
    struct global {
        DigitalOutputDomain::Instance *operational_led;
        DigitalOutputDomain::Instance *fault_led;
    };
    extern global Global;
};