#include "Guards/guards.hpp"
#include "Sensors/Sensors.hpp"

bool Guards::connection_finished(){
    return false;
}

bool Guards::fault_during_operation(){
    return (uint32_t)HVBMS::Sensors::voltage_sensor().reading < 410 && 
            (uint32_t)HVBMS::Sensors::current_sensor().reading < 120;
}
