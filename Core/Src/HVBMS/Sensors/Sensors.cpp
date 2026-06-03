#define BCC_STLIB_IMPLEMENTATION
#include "HVBMS/Sensors/Sensors.hpp"

void Sensors::init() {
    imd.bind(DO::imd_enable);
    imd.power_on();
    

    DO::sdc_fw_fault->turn_on();
    sdc.enable();

    #if BATTERIES_CONNECTED
    battery_h11.init();
    battery_h11.start();

    Scheduler::register_task(10000, []() { Sensors::update_batteries(); });
    #endif
}

void Sensors::update_batteries() {
        battery_h11.read();
}

void Sensors::update_sensors() {
    voltage_sensor.read();
    current_sensor.read();
    imd.read();
}
