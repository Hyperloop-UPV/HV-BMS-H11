#include "HVBMS/Sensors/SDC.hpp"
#include "HVBMS/HVBMS.hpp"

void SDC::sdc_callback(){
    if (sdc_interrupt->read() == GPIO_PinState::GPIO_PIN_RESET) {
        SDC::status = DataPackets::sdc_status::DISENGAGED;
    } else {
        SDC::status = DataPackets::sdc_status::ENGAGED;
    }
    if (!enabled){
        Scheduler::set_timeout(10000, [](){
            enabled = true;
        });
        return;
    } 
    

    HVBMS::state_machine.force_change_state(static_cast<std::size_t>(DataPackets::gsm_status::FAULT));
}

