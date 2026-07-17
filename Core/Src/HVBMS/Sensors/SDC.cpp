#include "HVBMS/Sensors/SDC.hpp"

#include "HVBMS/HVBMS.hpp"

void SDC::sdc_callback() {
    if (read_timeout != Scheduler::INVALID_ID) {
        Scheduler::cancel_timeout(read_timeout);
    }
    read_timeout = Scheduler::set_timeout(200000, []() {
        read_timeout = Scheduler::INVALID_ID;
        if (sdc_interrupt->read() == GPIO_PinState::GPIO_PIN_SET) {
            status = DataPackets::sdc_status::ENGAGED;
        } else {
            status = DataPackets::sdc_status::DISENGAGED;
            FAULT("SDC fault");
        }
    });
}
