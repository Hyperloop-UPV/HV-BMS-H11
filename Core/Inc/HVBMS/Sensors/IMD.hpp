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

using ST_LIB::EXTIDomain;

class IMD {
    using IC_Type =
        ST_LIB::InputCapture<timer_imd, GlobalTimer::ic_pin, ST_LIB::TimerChannel::CHANNEL_1>;
    inline static IC_Type* ic{nullptr};
    inline static DigitalInputDomain::Instance* ok{nullptr};
    inline static DigitalOutputDomain::Instance* pow{nullptr};
    inline static uint16_t debouncing_timeout{Scheduler::INVALID_ID};
    inline static bool enabled{false};
    
    public:
    inline static GPIO_PinState ok_status{GPIO_PIN_SET};
    
    inline static bool is_ok{true};
    inline static DataPackets::imd_status status{DataPackets::imd_status::FAST_EVAL};
    inline static float resistance{};
    inline static float freq{};
    inline static float duty{};

    static void bind(DigitalOutputDomain::Instance* pow_pin,
                     DigitalInputDomain::Instance* ok_pin) {
        ok = ok_pin;
        pow = pow_pin;
        static auto ic_instance =
            GlobalTimer::input_timer
                .get_input_capture<GlobalTimer::ic_pin, ST_LIB::TimerChannel::CHANNEL_1>();
        ic = &ic_instance;
        ic->turn_on();
    }

    static void power_on() { pow->turn_on(); }

    static void calculate_resistance() { resistance = ((90 * 1.2e6) / (duty - 5)) - 1.2e6; }

    static void imd_fault();

    static void read() {
        if (ok == nullptr || ic == nullptr) {
            return;
        }

        ok_status = ok->read();

        if (ok_status == GPIO_PinState::GPIO_PIN_RESET) {
            is_ok = false;
            if (!enabled) {
                if (debouncing_timeout == Scheduler::INVALID_ID) {
                    debouncing_timeout = Scheduler::set_timeout(2000000, []() {
                        enabled = true;
                        debouncing_timeout = Scheduler::INVALID_ID;
                    });
                }
                return;
            }
            imd_fault();
        } else {
            is_ok = true;
        }

        freq = ic->get_frequency();
        duty = ic->get_duty_cycle();
        if (lessError(freq, 0, 5)) {
            status = DataPackets::imd_status::SHORTCIRCUIT;
            return;
        }
        else if (lessError(freq, 10, 5)) {
            status = DataPackets::imd_status::NORMAL;
            calculate_resistance();
            return;
        }
        else if (lessError(freq, 20, 5)) {
            status = DataPackets::imd_status::UNDERVOLTAGE;
            calculate_resistance();
            return;
        }
        else if (lessError(freq, 30, 5)) {
            status = DataPackets::imd_status::FAST_EVAL;
            return;
        }
        else if (lessError(freq, 40, 5)) {
            status = DataPackets::imd_status::EQUIPMENT_FAULT;
            return;
        }
        else if (lessError(freq, 50, 5)) {
            status = DataPackets::imd_status::GROUNDING_FAULT;
            return;
        }
    }
};