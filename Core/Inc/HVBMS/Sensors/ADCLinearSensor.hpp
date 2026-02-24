#pragma once

#include "HVBMS/Comms/Comms.hpp"
#include "ST-LIB.hpp"
#include "ST-LIB_LOW.hpp"

using ST_LIB::ADCDomain;

class ADCLinearSensor {
    ADCDomain::Instance* adc_instance{nullptr};
    float slope;
    float offset;

   public:
    float reading{0.0};

    ADCLinearSensor(float slope_, float offset_) : slope{slope_}, offset{offset_} {}

    void bind(ADCDomain::Instance* adc) { adc_instance = adc; }

    void read() {
        if (!adc_instance) return;

        reading = slope * adc_instance->get_value() + offset;
    }
};
