#include "HVBMS/Sensors/IMD.hpp"

#include "HVBMS/HVBMS.hpp"

void IMD::imd_callback() {
    if (ok->read() == GPIO_PinState::GPIO_PIN_RESET) {
        is_ok = false;
    } else {
        is_ok = true;
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