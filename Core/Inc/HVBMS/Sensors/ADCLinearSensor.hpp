#pragma once

#include "HVBMS/Comms/Comms.hpp"
#include "ST-LIB_LOW.hpp"
#include "ST-LIB.hpp"

using ST_LIB::ADCDomain;

template <std::size_t FilterSize>
class ADCLinearSensor {
    FilteredLinearSensor<float, FilterSize> sensor;
    MovingAverage<FilterSize> filter{};

   public:
    float reading{};

    ADCLinearSensor(ADCDomain::Instance* adc, float slope, float offset)
        : sensor {
              *adc,
              slope,
              offset,
              reading,
              filter
          } {}

    void read() { sensor.read(); };
};

