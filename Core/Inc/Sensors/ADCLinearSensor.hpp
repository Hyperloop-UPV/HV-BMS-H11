#ifndef HVBMS_VOLTAGE_SENSOR_HPP
#define HVBMS_VOLTAGE_SENSOR_HPP

#include "ST-LIB_LOW.hpp"

namespace HVBMS {
template <std::size_t FilterSize>
class ADCLinearSensor {
    FilteredLinearSensor<float, FilterSize> sensor;
    MovingAverage<FilterSize> filter{};

   public:
    float reading{};

    ADCLinearSensor(Pin& pin, uint16_t id, float slope, float offset)
        : sensor{pin, slope, offset, reading, filter} {}

    void read() { sensor.read(); }
};
}  // namespace HBMS

#endif