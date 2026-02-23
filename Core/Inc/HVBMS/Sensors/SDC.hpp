#pragma once
#include "Communications/Packets/DataPackets.hpp"
#include "ST-LIB.hpp"

using ST_LIB::EXTIDomain;

class SDC {
   public:
    inline static EXTIDomain::Instance* sdc_interrupt{nullptr};
    inline static DataPackets::sdc_status status{DataPackets::sdc_status::DISENGAGED};
    inline static bool enabled{false};
    inline static uint16_t debouncing_timeout{Scheduler::INVALID_ID}; 

    static void enable() {
        if (!sdc_interrupt) return;

        if (sdc_interrupt->read() == GPIO_PinState::GPIO_PIN_SET) {
            status = DataPackets::sdc_status::ENGAGED;
            enabled = true;
        }
    }

    static void sdc_callback();
};

constexpr EXTIDomain::Device sdc_PB12{ST_LIB::PB12, EXTIDomain::Trigger::BOTH_EDGES,
                                      SDC::sdc_callback};
