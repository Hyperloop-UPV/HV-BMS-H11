#pragma once

#include "HVBMS/Data/Data.hpp"
#include "ST-LIB_LOW.hpp"

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
    using IC_Type =
        ST_LIB::InputCapture<timer_imd, GlobalTimer::ic_pin, ST_LIB::TimerChannel::CHANNEL_2>;
    inline static IC_Type* ic{nullptr};
    inline static std::optional<IC_Type> ic_instance{std::nullopt};
    inline static DigitalOutputDomain::Instance* pow{nullptr};
    inline static DigitalInputDomain::Instance* ok{nullptr};
    inline static float freq{};
    inline static float duty{};

   public:
    inline static GPIO_PinState ok_status{GPIO_PIN_SET};

    inline static bool is_ok{true};
    inline static DataPackets::imd_status status{DataPackets::imd_status::FAST_EVAL};
    inline static float resistance{};

    static void bind(DigitalOutputDomain::Instance* pow_pin, DigitalInputDomain::Instance* ok_pin) {
        pow = pow_pin;
        ok = ok_pin;
        GlobalTimer::input_timer
            .get_input_capture<GlobalTimer::ic_pin, ST_LIB::TimerChannel::CHANNEL_2>();
        ic = &ic_instance.value();
        ic->turn_on();
    }

    static void power_on() { pow->turn_on(); }

    static void calculate_resistance() { resistance = ((90 * 1.2e6) / (duty - 5)) - 1.2e6; }

    static void read() {
        if (ok == nullptr || ic == nullptr) {
            return;
        }

        ok_status = ok->read();

        if (ok_status == GPIO_PinState::GPIO_PIN_RESET) {
            is_ok = false;
        } else {
            is_ok = true;
        }

        freq = ic->get_frequency();
        duty = ic->get_duty_cycle();
        if (lessError(freq, 0, 0.1)) {
            status = DataPackets::imd_status::SHORTCIRCUIT;
            return;
        }
        if (lessError(freq, 10, 0.1)) {
            status = DataPackets::imd_status::NORMAL;
            calculate_resistance();
            return;
        }
        if (lessError(freq, 20, 0.1)) {
            status = DataPackets::imd_status::UNDERVOLTAGE;
            calculate_resistance();
            return;
        }
        if (lessError(freq, 30, 0.1)) {
            status = DataPackets::imd_status::FAST_EVAL;
            return;
        }
        if (lessError(freq, 40, 0.1)) {
            status = DataPackets::imd_status::EQUIPMENT_FAULT;
            return;
        }
        if (lessError(freq, 50, 0.1)) {
            status = DataPackets::imd_status::GROUNDING_FAULT;
            return;
        }
    }
};
