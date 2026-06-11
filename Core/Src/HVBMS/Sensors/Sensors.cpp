#define BCC_STLIB_IMPLEMENTATION
#include "HVBMS/Sensors/Sensors.hpp"

void Sensors::init() {
    imd.bind(DO::imd_enable);
    imd.power_on();

    DO::sdc_fw_fault->turn_on();
    sdc.enable();

    Scheduler::register_task(1000, []() {Sensors::update_sensors(); });
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
    ADC_reading::voltage_reading = (Sensors::VOLTAGE_SLOPE * ADC::adc_voltage_ch1->get_value()) + (Sensors::VOLTAGE_OFFSET - 5.5);
    ADC_reading::current_reading = (Sensors::CURRENT_SLOPE * ADC::adc_current->get_value()) + (Sensors::CURRENT_OFFSET - 6.07);
    imd.read();
}
