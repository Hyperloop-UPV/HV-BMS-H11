#pragma once

#include "HVBMS/Comms/Comms.hpp"
#include "ST-LIB_LOW.hpp"

template <std::size_t FilterSize>
class ADCLinearSensor {
    FilteredLinearSensor<float, FilterSize> sensor;
    MovingAverage<FilterSize> filter{};

   public:
    float reading{};

    ADCLinearSensor(Pin& pin, float slope, float offset)
        : sensor {
              pin,
              slope,
              offset,
              reading,
              filter
          } {}

    void read() { sensor.read(); };
};

