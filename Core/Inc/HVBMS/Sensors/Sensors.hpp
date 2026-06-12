#pragma once

#include "../../../deps/LTC6810-Driver/Inc/BMS.hpp"
#include "BatteryH11.hpp"
#include "BatteryPack.hpp"
#include "HVBMS/Data/Data.hpp"
#include "IMD.hpp"
#include "SDC.hpp"

class Sensors {
    // Real characterization HVBMS-H11 (channel 1 voltage)
    static constexpr float VOLTAGE_SLOPE{181.22132400065266};
    static constexpr float VOLTAGE_OFFSET{-9.5968814756376};

    // Current al reves
    static constexpr float CURRENT_SLOPE{-336.6379022410105};
    static constexpr float CURRENT_OFFSET{570.2997963006376};

   public:
    // Linear sensor does not work, so i just read directly from the input
    // No need to do otherwise

    inline static SDC sdc;

    inline static Batteries battery_h11;

    inline static IMD imd;

    static void init();
    static void update_sensors();
    static void update_batteries();
};
