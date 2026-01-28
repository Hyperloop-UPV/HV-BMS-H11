#include "Sensors/Sensors.hpp"

#include "Comms/Comms.hpp"

namespace HVBMS {
void Sensors::init() {
    voltage_sensor();
    current_sensor();
    sdc();

    // de momento esto esta aqui
    sdc().enable();
}

void Sensors::update_voltage(){
    voltage_sensor().read();
}

void Sensors::update_current(){
    current_sensor().read();
}


bool Sensors::check_sdc(){
    if (sdc().status == HVBMS::SDC::STATUS::DISENGAGED){
        return true;
    }
    else{
        return false;
    }
}

}  // namespace HVBMS
