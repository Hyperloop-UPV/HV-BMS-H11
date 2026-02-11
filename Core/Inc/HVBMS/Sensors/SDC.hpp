#pragma once

#include "HVBMS/Data/Data.hpp"
#include "ST-LIB_LOW.hpp"

class SDC {
    SensorInterrupt sdc_good;
    PinState sdc_good_value{ON};

    bool enabled{false};
    bool triggered{false};

    void sdc_callback(void) {
        sdc_good.read();
        if (sdc_good_value == PinState::OFF) {
            if (enabled) triggered = true;
            //status = States_SDC::DISENGAGED;
        } else {
            //status = States_SDC::ENGAGED;
        }
    }

   public:
    //States_SDC status{States_SDC::DISENGAGED};

    SDC(Pin& pin)
        : sdc_good{pin, [&]() { sdc_callback(); }, &sdc_good_value, TRIGGER::BOTH_EDGES} {}

    void enable() { enabled = true; };

    bool is_sdc_open() { return triggered; };
};
