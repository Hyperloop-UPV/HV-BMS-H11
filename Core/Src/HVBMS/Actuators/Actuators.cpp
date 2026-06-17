#include "HVBMS/Actuators/Actuators.hpp"

#include "HVBMS/Data/Data.hpp"

Contactor Actuators::contactor_low;
Contactor Actuators::contactor_high;
Contactor Actuators::contactor_precharge;
Contactor Actuators::contactor_discharge;
Contactor Actuators::contactor_common_high;

void Actuators::init() {
    contactor_discharge = Contactor{DO::contactor_discharge, DI::aux_contactor_discharge, false};
    contactor_precharge = Contactor{DO::contactor_precharge, DI::aux_contactor_precharge, true};
    contactor_low = Contactor{DO::contactor_low, DI::aux_contactor_low, true};
    contactor_high = Contactor{DO::contactor_high, DI::aux_contactor_high, true};
    contactor_common_high =
        Contactor(DO::contactor_common_high, DI::aux_contactor_common_high, true);

    contactor_discharge.close();
    contactor_precharge.open();
    contactor_low.open();
    contactor_high.open();
    contactor_common_high.open();

    Scheduler::register_task(50000, Actuators::update_contactors);
}

void Actuators::open_HV() {
    contactor_discharge.close();
    contactor_low.open();
    contactor_common_high.open();
    contactor_high.open();
    contactor_precharge.open();
    Sensors::precharge_offset = 0.0;
}

bool Actuators::is_HV_open() {
    return contactor_discharge.is_closed() && contactor_low.is_open() && contactor_high.is_open() &&
           contactor_precharge.is_open() && contactor_common_high.is_open();
}

void Actuators::close_HV() {
    contactor_discharge.open();
    contactor_low.close();
    contactor_common_high.close();
    contactor_precharge.open();
    contactor_high.close();
}

bool Actuators::is_HV_closed() {
    return contactor_discharge.is_open() && contactor_low.is_closed() &&
           contactor_high.is_closed() && contactor_precharge.is_open() &&
           contactor_common_high.is_closed();
}

void Actuators::start_precharge() {
    contactor_discharge.open();
    contactor_low.close();
    contactor_common_high.close();
    contactor_precharge.close();
    contactor_high.open();
    Sensors::precharge_offset = 0.02;
}

bool Actuators::is_precharging() {
    return contactor_discharge.is_open() && contactor_low.is_closed() && contactor_high.is_open() &&
           contactor_precharge.is_closed() && contactor_common_high.is_closed();
}

void Actuators::toggle_operational_led() { DO::operational_led->toggle(); }

bool& Actuators::get_contactor_low_state() { return contactor_low.get_state(); }

bool& Actuators::get_contactor_high_state() { return contactor_high.get_state(); }

bool& Actuators::get_contactor_discharge_state() { return contactor_discharge.get_state(); }

bool& Actuators::get_contactor_precharge_state() { return contactor_precharge.get_state(); }

bool& Actuators::get_contactor_common_high_state() { return contactor_common_high.get_state(); }

void Actuators::update_contactors() {
    contactor_discharge.is_open();
    contactor_precharge.is_open();
    contactor_low.is_open();
    contactor_high.is_open();
    contactor_common_high.is_open();
}
