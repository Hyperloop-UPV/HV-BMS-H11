    #pragma once

    #include "HVBMS/Comms/Comms.hpp"
    #include "ST-LIB_LOW.hpp"
    #include "ST-LIB.hpp"

    using ST_LIB::ADCDomain;

    template <std::size_t FilterSize>
    class ADCLinearSensor {
        
        ADCDomain::Instance* adc_instance{nullptr};
        float slope;
        float offset;

        MovingAverage<FilterSize> filter{};

    public:
        float reading{};

        ADCLinearSensor(float slope_, float offset_) : slope{slope_}, offset{offset_} {}

        void bind(ADCDomain::Instance* adc) { adc_instance = adc; }

        void read() {
            if (!adc_instance) return;

            reading = slope * adc_instance->get_value() + offset;
        }
    };
