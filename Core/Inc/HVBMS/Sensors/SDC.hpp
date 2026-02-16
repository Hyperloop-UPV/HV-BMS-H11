#pragma once

#include "HVBMS/Data/Data.hpp"
#include "ST-LIB_LOW.hpp"

class SDC {
    SensorInterrupt sdc_good;
    GPIO_PinState sdc_good_value{GPIO_PIN_SET};

    bool enabled{false};
    bool triggered{false};

   public:
    States_SDC status{States_SDC::DISENGAGED};

    SDC(ST_LIB::EXTIDomain::Instance exti) : sdc_good(exti, sdc_good_value) {
        Callbacks::sdc_instance = this;
    }

    void sdc_callback() {
        sdc_good.read();
        if (sdc_good_value == GPIO_PinState::GPIO_PIN_RESET) {
            if (enabled) triggered = true;
            status = States_SDC::DISENGAGED;
        } else {
            status = States_SDC::ENGAGED;
        }
    }

    void enable() { enabled = true; }
    bool is_sdc_open() { return triggered; }
};

namespace Callbacks {
inline void sdc_exti_callback() {
    if (sdc_instance) {
        sdc_instance->sdc_callback();
    }
}
}  // namespace Callbacks