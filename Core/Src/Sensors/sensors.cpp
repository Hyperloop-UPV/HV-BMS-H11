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

void Sensors::update(){
    voltage_sensor().read();
    current_sensor().read();
}

void Sensors::open_sdc(){
    sdc().process_state(PinState::OFF);
}

}  // namespace HVBMS
