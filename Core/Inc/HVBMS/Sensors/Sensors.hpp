#pragma once

#include "../../../deps/LTC6810-Driver/Inc/BMS.hpp"
#include "HVBMS/Data/Data.hpp"
#include "BatteryPack.hpp"
#include "IMD.hpp"
#include "SDC.hpp"
#include "BatteryH11.hpp"

#define BATTERIES_CONNECTED 1
#define N_BATTERIES 16

class Sensors {
    static constexpr float VOLTAGE_SLOPE{122.03744695176259};
    static constexpr float VOLTAGE_OFFSET{-3.57028270765926};

    static constexpr float CURRENT_SLOPE{89.94180359377545};
    static constexpr float CURRENT_OFFSET{-151.40450651439056};

   public:

    inline static LinearSensor<float> voltage_sensor { *ADC::adc_voltage_ch2, VOLTAGE_SLOPE, VOLTAGE_OFFSET, ADC_reading::voltage_reading};

    inline static LinearSensor<float> current_sensor { *ADC::adc_current, CURRENT_SLOPE, CURRENT_OFFSET, ADC_reading::current_reading};

    inline static SDC sdc;

    inline static BatteryPack<N_BATTERIES> batteries;

    inline static Batteries battery_h11;

    inline static IMD imd;

    static void init();
    static void update_sensors();
    static void update_batteries();
};
