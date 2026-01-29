#ifndef SENSORS_HPP
#define SENSORS_HPP

#include "ADCLinearSensor.hpp"
#include "SDC.hpp"


namespace HVBMS{
class Sensors {
    // Voltage sensor for HVSCU with ID 4
    static constexpr Pin &VOLTAGE_PIN{PF13};
    static constexpr float VOLTAGE_SLOPE{122.2523193359375};
    static constexpr float VOLTAGE_OFFSET{-3.6849429607391357};

    // Current sensor for HVSCU with ID 1
    static constexpr Pin &CURRENT_PIN{PA0};
    static constexpr float CURRENT_SLOPE{92.27959442138672};
    static constexpr float CURRENT_OFFSET{-152.49656677246094};

    // SDC
    static constexpr Pin &SDC_GOOD_PIN{PB12};


   public:
    static ADCLinearSensor<5> &voltage_sensor() {
        static ADCLinearSensor<5> voltage_sensor{
            VOLTAGE_PIN,
            static_cast<uint16_t>(1),
            VOLTAGE_SLOPE,
            VOLTAGE_OFFSET};
        return voltage_sensor;
    }

    static ADCLinearSensor<5> &current_sensor() {
        static ADCLinearSensor<5> current_sensor{
            CURRENT_PIN,
            static_cast<uint16_t>(2),
            CURRENT_SLOPE,
            CURRENT_OFFSET};
        return current_sensor;
    }


    static SDC &sdc() {
        static SDC sdc{SDC_GOOD_PIN,
                       static_cast<uint16_t>(3)};
        return sdc;
    }


    static void init();
    static void update_voltage();
    static void update_current();
    static bool check_sdc();
};
}  // namespace HVBMS

#endif