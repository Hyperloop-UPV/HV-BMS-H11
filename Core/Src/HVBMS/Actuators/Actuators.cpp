#include "HVBMS/Actuators/Actuators.hpp"
#include "HVBMS/Data/Data.hpp"


Contactor Actuators::contactor_low(DO::contactor_low, true);        
Contactor Actuators::contactor_high(DO::contactor_high, true);      
Contactor Actuators::contactor_precharge(DO::contactor_precharge, true);
Contactor Actuators::contactor_discharge(DO::contactor_discharge, false);


void Actuators::open_HV(){
    contactor_discharge.close();
    contactor_low.open();
    contactor_high.open();
    contactor_precharge.open();

}
bool Actuators::is_HV_open(){
    return contactor_discharge.is_closed() && contactor_low.is_open() &&
           contactor_high.is_open() && contactor_precharge.is_open();
}

void Actuators::close_HV(){
    contactor_discharge.open();
    contactor_low.close();
    contactor_precharge.open();
    contactor_high.close();
}

bool Actuators::is_HV_closed(){
    return contactor_discharge.is_open() && contactor_low.is_closed() &&
           contactor_high.is_closed() && contactor_precharge.is_open();
}

void Actuators::start_precharge(){
    contactor_discharge.open();
    contactor_low.close();
    contactor_precharge.close();
    contactor_high.open();
}

bool Actuators::is_precharging(){
    return contactor_discharge.is_open() && contactor_low.is_closed() &&
           contactor_high.is_open() && contactor_precharge.is_closed();
}

void Actuators::toggle_operational_led(){
    DO::operational_led->toggle();
}

