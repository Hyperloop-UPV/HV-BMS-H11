#include "HVBMS/Sensors/Sensors.hpp"

void Sensors::update_batteries(){
    if constexpr (BATTERIES_CONNECTED){
        batteries.update();
        batteries.read(current_sensor.reading);
    }
}

void Sensors::update_sensors(){
    voltage_sensor.read();
    current_sensor.read();
}
