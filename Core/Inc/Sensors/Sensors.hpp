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
    static constexpr Pin &VOLTAGE_PIN{PF13};
    static constexpr float VOLTAGE_SLOPE{122.2523193359375};
    static constexpr float VOLTAGE_OFFSET{-3.6849429607391357};

    // Current sensor for HVBMS with ID 1
    static constexpr Pin &CURRENT_PIN{PA0};
    static constexpr float CURRENT_SLOPE{92.27959442138672};
    static constexpr float CURRENT_OFFSET{-152.49656677246094};

    // SDC
    static constexpr Pin &SDC_GOOD_PIN{PB12};

public:

    inline static ADCLinearSensor<5> voltage_sensor{
        VOLTAGE_PIN,
        static_cast<uint16_t>(Comms::IDPacket::VOLTAGE),
        VOLTAGE_SLOPE,
        VOLTAGE_OFFSET,
        {Comms::Target::CONTROL_STATION}};


    inline static ADCLinearSensor<5> current_sensor{
        CURRENT_PIN,
        static_cast<uint16_t>(Comms::IDPacket::CURRENT),
        CURRENT_SLOPE,
        CURRENT_OFFSET,
        {Comms::Target::CONTROL_STATION}};



    inline static SDC sdc{SDC_GOOD_PIN,
        static_cast<uint16_t>(Comms::IDPacket::SDC)};


    inline static BatteryPack<N_BATTERIES> batteries{
        static_cast<uint16_t>(Comms::IDPacket::TOTAL_VOLTAGE),
        static_cast<uint16_t>(Comms::IDPacket::DRIVER_DIAG),
        static_cast<uint16_t>(Comms::IDPacket::BATTERY_1),
        static_cast<uint16_t>(Comms::IDPacket::MINIMUM_SOC),
        static_cast<uint16_t>(Comms::IDPacket::BATTERIES_DATA)};

    static void update_sensors();
    static void update_batteries();
};
}  // namespace HVBMS

#endif