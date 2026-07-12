#pragma once
#include "HVBMS/Sensors/BatteryH11.hpp"
#include "Protections/Protection.hpp"

inline constexpr auto battery_temp_max_protection =
    Protections::protection<"battery_temp_max", Batteries::max_temperature>(
        Protections::Rules::above(60.0f));

inline constexpr auto battery_temp_min_protection =
    Protections::protection<"battery_temp_min", Batteries::min_temperature>(
        Protections::Rules::below(-10.0f));

inline constexpr auto battery_cell_voltage_max_protection =
    Protections::protection<"cell_voltage_max", Batteries::max_total_voltage>(
        Protections::Rules::above(4.2f));

inline constexpr auto battery_cell_voltage_min_protection =
    Protections::protection<"cell_voltage_min", Batteries::min_total_voltage>(
        Protections::Rules::below(2.7f));
