#include "HVBMS/Actuators/Contactor.hpp"

Contactor::Contactor(DigitalOutputDomain::Instance* output, DigitalInputDomain::Instance* input,
                     bool normally_opened)
    : output{output},
      input{input},
      normally_opened(normally_opened) {}

void Contactor::open() {
    if (normally_opened)
        output->turn_off();
    else
        output->turn_on();
}

// hay que mirar si es reset o set
bool Contactor::is_open() {
    if (input->read() == GPIO_PinState::GPIO_PIN_SET)
        state = true;
    else 
        state = false;
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
        state = true;
    else
        state = false;
    return state;
}

bool& Contactor::get_state() { return state; }