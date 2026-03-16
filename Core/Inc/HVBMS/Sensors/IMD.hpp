#pragma once

#include "ST-LIB_LOW.hpp"
#include "HVBMS/Data/Data.hpp"

template <typename A, typename B>
concept Substractable = requires(A a, B b) {
    { a - b } -> std::totally_ordered;
};

template <typename A, typename B, typename E>
    requires Substractable<A, B> && std::totally_ordered<E>
inline bool lessError(const A& a, const B& b, const E& error) {
    return std::abs(a - b) < error;
}
class IMD {

    //PWMSensor<float> m_ls;
    inline static DigitalOutputDomain::Instance* pow{nullptr};
    inline static DigitalInputDomain::Instance* ok{nullptr};
    float freq{};
    float duty{};

    uint8_t status{static_cast<uint8_t>(DataPackets::imd_status::FAST_EVAL)};
    float resistance{};

   public:
    GPIO_PinState ok_status{GPIO_PIN_SET};
    bool is_ok{true};

    //IMD(Pin& m_ls_pin) : m_ls{m_ls_pin, freq, duty} {}

    void bind(DigitalOutputDomain::Instance* pow_pin, DigitalInputDomain::Instance* ok_pin) {
        pow = pow_pin;
        ok = ok_pin;
    }  // meter aqui los demas cuando los tenga
    
    void power_on() { pow->turn_on(); }

    void calculate_resistance() { resistance = ((90 * 1.2e6) / (duty - 5)) - 1.2e6; }

    void read() {
        ok_status = ok->read();

        if (ok_status == GPIO_PinState::GPIO_PIN_RESET) {
            is_ok = false;
        } else {
            is_ok = true;
        }

        //m_ls.read();
        if (lessError(freq, 0, 0.1)) {
            status = static_cast<uint8_t>(DataPackets::imd_status::SHORTCIRCUIT);
            return;
        }
        if (lessError(freq, 10, 0.1)) {
            status = static_cast<uint8_t>(DataPackets::imd_status::NORMAL);
            calculate_resistance();
            return;
        }
        if (lessError(freq, 20, 0.1)) {
            status = static_cast<uint8_t>(DataPackets::imd_status::UNDERVOLTAGE);
            calculate_resistance();
            return;
        }
        if (lessError(freq, 30, 0.1)) {
            status = static_cast<uint8_t>(DataPackets::imd_status::FAST_EVAL);
            return;
        }
        if (lessError(freq, 40, 0.1)) {
            status = static_cast<uint8_t>(DataPackets::imd_status::EQUIPMENT_FAULT);
            return;
        }
        if (lessError(freq, 50, 0.1)) {
            status = static_cast<uint8_t>(DataPackets::imd_status::GROUNDING_FAULT);
            return;
        }
    }
};
