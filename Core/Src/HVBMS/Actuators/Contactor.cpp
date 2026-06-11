#include "HVBMS/Actuators/Contactor.hpp"

Contactor::Contactor(DigitalOutputDomain::Instance* output, DigitalInputDomain::Instance* input,
                     bool normally_opened)
    : output{output}, input{input}, normally_opened(normally_opened) {}

void Contactor::open() {
    if (normally_opened)
        output->turn_off();
    else
        output->turn_on();
}

bool Contactor::is_open() {
    if (input->read() == GPIO_PinState::GPIO_PIN_SET)
        if (normally_opened) {
            state = true;
        } else {
            state = false;
        }
    else {
        if (normally_opened) {
            state = false;
        } else {
            state = true;
        }
    }
    return state;
}

void Contactor::close() {
    if (normally_opened)
        output->turn_on();
    else
        output->turn_off();
}

bool Contactor::is_closed() {
    if (input->read() == GPIO_PinState::GPIO_PIN_RESET)
        if (normally_opened) {
            state = false;
        } else {
            state = true;
        }
    else {
        if (normally_opened) {
            state = true;
        } else {
            state = false;
        }
    }
    return state;
}

bool& Contactor::get_state() { return state; }