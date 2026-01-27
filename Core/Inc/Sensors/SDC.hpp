#ifndef HVBMS_SDC_HPP
#define HVBMS_SDC_HPP

#include "ST-LIB_LOW.hpp"

namespace HVBMS {
class SDC {
    SensorInterrupt sdc_good;
    PinState sdc_good_value{ON};

    bool enabled{false};

    void sdc_callback(void) {
        sdc_good.read();
        process_state(sdc_good_value);
    }

   public:
    enum class STATUS : uint8_t { ENGAGED, DISENGAGED };

    bool triggered{false};
    STATUS status{STATUS::DISENGAGED};

    SDC(Pin& pin, uint16_t id)
        : sdc_good{pin, [&]() { sdc_callback(); }, &sdc_good_value,
                   TRIGGER::BOTH_EDGES}{ };

    void enable() { enabled = true; }

    void process_state(PinState value){
        if (value == PinState::OFF) {
            if (enabled) triggered = true;
            status = STATUS::DISENGAGED;
        } else {
            status = STATUS::ENGAGED;
        }
    }
};
}  // namespace HVBMS

#endif