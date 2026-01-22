#include "state_machine.hpp"

template<size_t N>
StateMachine<N>::StateMachine(const std::array<state_t, N>& s) : states(s), current_state(s[0]) {}

template<size_t N>
void StateMachine<N>::update(){
    if (current_state) current_state();
}

void connecting(void){
    // Conectar a ->
        // Control Station via TCP
        // Read SDC
        // Conexion TCP/UDP (maybe TCP es la misma que la CS)
        // Read contactor state
        // Read IMD
        // Read cell monitoring
        // Read DC current and voltage


    // Connection succesful -> operational
    // Connection not succesful -> fault?

    // Debo estar leyendo (por ejemplo, el imd) en cuanto me conecto (desde este estado), antes de pasar a operational?
}

void operational(void){
    // Simplemente checkear que todo lo anterior este en los valores que toca
}

void fault(void){
    // Open contractors
    // FAULT to control station (TCP)
    // Open SDC
}

std::array<state_t, 3> basic_state_machine {connecting, operational, fault};

StateMachine basic = StateMachine<3>(basic_state_machine);


void update_state_machines(){
    basic.update();
}