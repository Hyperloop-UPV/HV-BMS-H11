#pragma once

#include "ADCLinearSensor.hpp"
// #include "BMS.hpp"
#include "../../../deps/LTC6810-Driver/Inc/BMS.hpp"
#include "BatteryPack.hpp"
#include "SDC.hpp"

#define BATTERIES_CONNECTED 1
#define N_BATTERIES 2

class Sensors {
    // Voltage sensor for HVBMS with ID 4
    // Hecho tambien
    static constexpr float VOLTAGE_SLOPE{122.03744695176259};
    static constexpr float VOLTAGE_OFFSET{-3.57028270765926};

    // Current sensor for HVBMS with ID 1
    // Hecho
    static constexpr float CURRENT_SLOPE{89.94180359377545};
    static constexpr float CURRENT_OFFSET{-151.40450651439056};

    // SDC
    static constexpr Pin& SDC_GOOD_PIN{PB12};

   public:
    inline static ADCLinearSensor<5> voltage_sensor{ADC::adc_voltage, VOLTAGE_SLOPE, VOLTAGE_OFFSET};

    inline static ADCLinearSensor<5> current_sensor{ADC::adc_current, CURRENT_SLOPE, CURRENT_OFFSET};

    inline static SDC sdc{*EXTI_SDC::sdc_interrupt};

    inline static BatteryPack<N_BATTERIES>* batteries;

    static void init();
    static void update_sensors();
    static void update_batteries();
};

