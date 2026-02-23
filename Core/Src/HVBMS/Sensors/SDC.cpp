#include "HVBMS/Sensors/SDC.hpp"

#include "HVBMS/HVBMS.hpp"

void SDC::sdc_callback() {
    if (sdc_interrupt->read() == GPIO_PinState::GPIO_PIN_RESET) {
        SDC::status = DataPackets::sdc_status::DISENGAGED;
    } else {
        SDC::status = DataPackets::sdc_status::ENGAGED;
    }
    if (!enabled) {
        if (debouncing_timeout == Scheduler::INVALID_ID) {
            debouncing_timeout = Scheduler::set_timeout(10000, []() {
                enabled = true;
                debouncing_timeout = Scheduler::INVALID_ID;
            });
        }
        return;
    }

    HVBMS::state_machine.force_change_state((std::size_t)DataPackets::gsm_status::FAULT);
}
