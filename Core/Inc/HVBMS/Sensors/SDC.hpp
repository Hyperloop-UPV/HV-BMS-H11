#pragma once
#include "ST-LIB.hpp"

using ST_LIB::EXTIDomain;

class SDC {
    inline static bool enabled{false};
    inline static GPIO_PinState start_state{GPIO_PinState::GPIO_PIN_SET};

   public:
    inline static States_SDC status{States_SDC::DISENGAGED};

    static void enable() { enabled = true; }

    static bool is_sdc_open() {
        if (!enabled) return false;
        return EXTI_SDC::sdc_state;
    }
};
