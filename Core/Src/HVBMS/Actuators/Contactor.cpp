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

bool Contactor::is_open() { return !is_closed(); }

void Contactor::close() {
    if (normally_opened)
        output->turn_on();
    else
        output->turn_off();
}

bool Contactor::is_closed() {
    if (input->read() == GPIO_PinState::GPIO_PIN_RESET)
        state = false;
    else {
        state = true;
    }
    return state;
}

bool& Contactor::get_state() { return state; }