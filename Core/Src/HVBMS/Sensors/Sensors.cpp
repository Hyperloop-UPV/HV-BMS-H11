#include "HVBMS/Sensors/Sensors.hpp"

void Sensors::init() {
    imd.bind(DO::imd_enable);
    imd.power_on();
    
    DO::spi_enable->turn_on();
    
    DO::sdc_fw_fault->turn_on();
    sdc.enable();

    Scheduler::register_task(10000, []() { Sensors::update_batteries(); });
}

void Sensors::update_batteries() {
    if constexpr (BATTERIES_CONNECTED) {
        batteries.update();
        batteries.read(ADC_reading::current_reading);
    }
}

void Sensors::update_sensors() {
    voltage_sensor.read();
    current_sensor.read();
    imd.read();
}
