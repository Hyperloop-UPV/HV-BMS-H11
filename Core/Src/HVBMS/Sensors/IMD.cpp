#include "HVBMS/Sensors/IMD.hpp"

#include "HVBMS/HVBMS.hpp"

void IMD::imd_callback() {
    if (ok->read() == GPIO_PinState::GPIO_PIN_RESET) {
        is_ok = false;
    } else {
        is_ok = true;
    }

    FAULT("IMD fault");
}