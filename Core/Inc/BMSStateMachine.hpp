#pragma once
#include "StateMachine/StateMachine.hpp"

// Estados que necesita el sm basico
enum class BMSState {
    CONNECTING,
    OPERATIONAL,
    FAULT
};

// Declaramos las funciones que decidirán las transiciones
namespace guards {
    bool connection_finished();
    bool fault_during_precharge();
    bool fault_during_operation();
}

// Acciones a realizar 
namespace actions {
    // Nada más entrar en FAULT
    void contactors_to_open();
    void fault_to_cs();
    void open_sdc();

    // Nada más entrar en CONNECTING
    void connect_tcp();
    void connect_udp();

    // Nada más entrar en OPERATIONAL
    void start_precharge();

    // Cíclicas
    // OPERATIONAL
    void check_precharge_status();
    void read_current();
    void read_voltage();
    void monitor_cell_voltage();
    void read_sdc();
}

constexpr auto connecting_state = make_state(BMSState::CONNECTING,
    Transition<BMSState>{BMSState::OPERATIONAL, &guards::connection_finished},
    Transition<BMSState>{BMSState::FAULT, &guards::fault_during_precharge}
);

constexpr auto operational_state = make_state(BMSState::OPERATIONAL,
    Transition<BMSState>{BMSState::FAULT, &guards::fault_during_operation}
);

constexpr auto fault_state = make_state(BMSState::FAULT);

consteval auto build_bms_state_machine() {
    // Crear maquina de estados
    auto bms_sm = make_state_machine(
        BMSState::CONNECTING,
        connecting_state,
        operational_state,
        fault_state
    );

    // -- ACCIONES --
    
    // Al entrar a FAULT
    bms_sm.add_enter_action([](){actions::contactors_to_open();}, fault_state);
    bms_sm.add_enter_action([](){actions::fault_to_cs();}, fault_state);
    bms_sm.add_enter_action([](){actions::open_sdc();}, fault_state);

    // Al entrar a CONNECTING
    bms_sm.add_enter_action([](){actions::connect_tcp();}, connecting_state);
    bms_sm.add_enter_action([](){actions::connect_udp();}, connecting_state);

    // Al entrar a OPERATIONAL
    bms_sm.add_enter_action([](){actions::start_precharge();}, operational_state);


    // Acciones CÍCLICAS
    using namespace std::chrono_literals;
    // Se definen con la acción, el periodo y el estado en el que se ejecutan.
    bms_sm.add_cyclic_action(&actions::check_precharge_status, 1ms, operational_state);
    bms_sm.add_cyclic_action(&actions::read_current, 10ms, operational_state);
    bms_sm.add_cyclic_action(&actions::read_voltage, 10ms, operational_state);
    bms_sm.add_cyclic_action(&actions::monitor_cell_voltage, 10ms, operational_state);
    bms_sm.add_cyclic_action(&actions::read_sdc, 10ms, operational_state);

    return bms_sm;
}

inline auto BSM = build_bms_state_machine();

