#pragma once
#include "ST-LIB.hpp"
#include "HVBMS/Sensors/Sensors.hpp"


inline constexpr auto dc_voltage_protection =
    Protections::protection<"dc_bus_voltage", Sensors::voltage_sensor.reading>(
        Protections::Rules::above(410.0f)
    );

inline constexpr auto dc_current_protection =
    Protections::protection<"dc_bus_current", Sensors::current_sensor.reading>(
        Protections::Rules::above(120.0f));


