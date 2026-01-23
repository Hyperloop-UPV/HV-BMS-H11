#pragma once
#include "StateMachine/StateMachine.hpp"
#include "Actions/operational.hpp"
#include "Actions/connecting.hpp"
#include "Actions/fault.hpp"

// Estados que necesita el sm basico
enum class BMSState {
    CONNECTING,
    OPERATIONAL,
    FAULT
};

// Mover esto a sus correspondientes archivos
// Declaramos las funciones que decidirán las transiciones
namespace guard {
    bool connection_finished();
    bool fault_during_operation();
}


// Crear estados 
constexpr auto connecting_state = make_state(BMSState::CONNECTING,
    Transition<BMSState>{BMSState::OPERATIONAL, &guard::connection_finished}
);

constexpr auto operational_state = make_state(BMSState::OPERATIONAL,
    Transition<BMSState>{BMSState::FAULT, &guard::fault_during_operation}
);

constexpr auto fault_state = make_state(BMSState::FAULT);


// Crear maquina de estados
consteval auto build_bms_state_machine() {
    auto bms_sm = make_state_machine(
        BMSState::CONNECTING,
        connecting_state,
        operational_state,
        fault_state
    );

    // -- ACCIONES --

    // Al entrar a FAULT
    bms_sm.add_enter_action([](){Fault::contactors_to_open();}, fault_state);
    bms_sm.add_enter_action([](){Fault::fault_to_cs();}, fault_state);
    bms_sm.add_enter_action([](){Fault::open_sdc();}, fault_state);

    // Al entrar a CONNECTING
    bms_sm.add_enter_action([](){Connecting::connect_tcp();}, connecting_state);
    bms_sm.add_enter_action([](){Connecting::connect_udp();}, connecting_state);

    // Al entrar a OPERATIONAL
    bms_sm.add_enter_action([](){Operational::start_precharge();}, operational_state);
    bms_sm.add_enter_action([](){Operational::start_sensors();}, operational_state);


    // Acciones CÍCLICAS
    using namespace std::chrono_literals;
    // Se definen con la acción, el periodo y el estado en el que se ejecutan.
    bms_sm.add_cyclic_action(Operational::check_precharge_status, 1ms, operational_state);
    bms_sm.add_cyclic_action(Operational::read_current, 10ms, operational_state);
    bms_sm.add_cyclic_action(Operational::read_voltage, 10ms, operational_state);
    bms_sm.add_cyclic_action(Operational::monitor_cell_voltage, 10ms, operational_state);
    bms_sm.add_cyclic_action(Operational::read_sdc, 10ms, operational_state);

    return bms_sm;
}

inline auto BSM = build_bms_state_machine();

