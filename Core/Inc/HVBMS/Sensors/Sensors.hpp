#pragma once

#include "../../../deps/LTC6810-Driver/Inc/BMS.hpp"
#include "HVBMS/Data/Data.hpp"
#include "BatteryPack.hpp"
#include "IMD.hpp"
#include "SDC.hpp"
#include "BatteryH11.hpp"


class Sensors {
    // Real characterization HVBMS-H11 (channel 1 voltage)
    static constexpr float VOLTAGE_SLOPE{181.04199675052496};
    static constexpr float VOLTAGE_OFFSET{-9.393116311187555};

    // Current al reves
    static constexpr float CURRENT_SLOPE{-336.6379022410105};
    static constexpr float CURRENT_OFFSET{570.2997963006376};

   public:

    inline static LinearSensor<float> voltage_sensor { *ADC::adc_voltage_ch2, VOLTAGE_SLOPE, VOLTAGE_OFFSET, ADC_reading::voltage_reading};

    inline static LinearSensor<float> current_sensor { *ADC::adc_current, CURRENT_SLOPE, CURRENT_OFFSET, ADC_reading::current_reading};

    inline static SDC sdc;

    inline static Batteries battery_h11;

    inline static IMD imd;

    static void init();
    static void update_sensors();
    static void update_batteries();
};
