#define BCC_STLIB_IMPLEMENTATION
#include "HVBMS/Sensors/Sensors.hpp"

#include "HALAL/Benchmarking_toolkit/DataWatchpointTrace/DataWatchpointTrace.hpp"

void Sensors::init() {
    DataWatchpointTrace::start();

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
    DataWatchpointTrace::start_count();
    battery_h11.read();
    unsigned int prueba = DataWatchpointTrace::stop_count();
    (void)prueba;
}

void Sensors::update_sensors() {
    ADC_reading::voltage_reading =
        (Sensors::VOLTAGE_SLOPE * ADC::adc_voltage_ch1->get_value()) + Sensors::VOLTAGE_OFFSET;
    if (ADC_reading::voltage_reading <= 8) ADC_reading::voltage_reading -= 5;
    ADC_reading::current_reading = (Sensors::CURRENT_SLOPE * ADC::adc_current->get_value()) +
                                   (Sensors::CURRENT_OFFSET - 6.07) +
                                   (Sensors::precharge_offset * ADC_reading::voltage_reading);
    imd.read();
}
