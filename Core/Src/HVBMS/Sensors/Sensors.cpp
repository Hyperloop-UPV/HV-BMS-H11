#define BCC_STLIB_IMPLEMENTATION
#include "HVBMS/Sensors/Sensors.hpp"

void Sensors::init() {
    imd.bind(DO::imd_enable);
    imd.power_on();
    
    DI::battery_intb->read();

    DO::sdc_fw_fault->turn_on();
    sdc.enable();

    battery_h11.init();
    battery_h11.start();

    Scheduler::register_task(10000, []() { Sensors::update_batteries(); });
}

void Sensors::update_batteries() {
    if constexpr (BATTERIES_CONNECTED) {
        battery_h11.read();
    }
}

void Sensors::update_sensors() {
    voltage_sensor.read();
    current_sensor.read();
    imd.read();
}
