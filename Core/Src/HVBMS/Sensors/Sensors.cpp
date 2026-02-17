#include "HVBMS/Sensors/Sensors.hpp"

void Sensors::init(){
    static BatteryPack<N_BATTERIES> batteries_instance;
    batteries = &batteries_instance;
}

void Sensors::update_batteries(){
    if constexpr (BATTERIES_CONNECTED){
        batteries->update();
        batteries->read(current_sensor.reading);
    }
}

void Sensors::update_sensors(){
    voltage_sensor.read();
    current_sensor.read();
}
