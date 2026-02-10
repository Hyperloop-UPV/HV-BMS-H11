#ifndef HVBMS_SDC_HPP
#define HVBMS_SDC_HPP

#include "ST-LIB_LOW.hpp"
#include "Communications/Packets/DataPackets.hpp"

namespace HVBMS {
class SDC {
    SensorInterrupt sdc_good;
    PinState sdc_good_value{ON};

    bool enabled{false};
    bool triggered{false};

    void sdc_callback(void) {
        sdc_good.read();
        if (sdc_good_value == PinState::OFF) {
            if (enabled) triggered = true;
            status = DataPackets::sdc_status::DISENGAGED;
        } else {
            status = DataPackets::sdc_status::ENGAGED;
        }
    }

   public:
    DataPackets::sdc_status status{DataPackets::sdc_status::DISENGAGED};

    SDC(Pin& pin)
        : sdc_good {
              pin,
              [&]() { sdc_callback(); },
              &sdc_good_value,
              TRIGGER::BOTH_EDGES
          } {}
    void enable() { enabled = true; };

    bool is_sdc_open() { return triggered; };
};
}  // namespace HVBMS

#endif