#ifndef SENSORS_HPP
#define SENSORS_HPP

#include "ADCLinearSensor.hpp"
//#include "BMS.hpp"
#include "../../../deps/LTC6810-Driver/Inc/BMS.hpp"
#include "BatteryPack.hpp"
#include "SDC.hpp"

#define BATTERIES_CONNECTED 1
#define N_BATTERIES 15

namespace HVBMS {
class Sensors {
    // Voltage sensor for HVBMS with ID 4
    // Hecho tambien
    static constexpr Pin &VOLTAGE_PIN{PF13};
    static constexpr float VOLTAGE_SLOPE{122.03744695176259};
    static constexpr float VOLTAGE_OFFSET{-3.57028270765926};

    // Current sensor for HVBMS with ID 1
    // Hecho    
    static constexpr Pin &CURRENT_PIN{PA0};
    static constexpr float CURRENT_SLOPE{89.94180359377545};
    static constexpr float CURRENT_OFFSET{-151.40450651439056};

    // SDC
    static constexpr Pin &SDC_GOOD_PIN{PB12};

public:

    inline static ADCLinearSensor<5> voltage_sensor{
        VOLTAGE_PIN,
        VOLTAGE_SLOPE,
        VOLTAGE_OFFSET};


    inline static ADCLinearSensor<5> current_sensor{
        CURRENT_PIN,
        CURRENT_SLOPE,
        CURRENT_OFFSET};



    inline static SDC sdc{SDC_GOOD_PIN};


    // inline static BatteryPack<N_BATTERIES> batteries{
    //     static_cast<uint16_t>(Comms::IDPacket::TOTAL_VOLTAGE),
    //     static_cast<uint16_t>(Comms::IDPacket::DRIVER_DIAG),
    //     static_cast<uint16_t>(Comms::IDPacket::BATTERY_1),
    //     static_cast<uint16_t>(Comms::IDPacket::BATTERIES_DATA)};

    static void update_sensors();
    static void update_batteries();
};
}  // namespace HVBMS

#endif