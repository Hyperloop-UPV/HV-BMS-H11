#pragma once
#include "StateMachine/StateMachine.hpp"
#include "Actions/operational.hpp"
#include "Actions/connecting.hpp"
#include "Actions/fault.hpp"
#include "Guards/guards.hpp"
#include "Actuators/Actuators.hpp"
#include "Sensors/Sensors.hpp"

// Estados que necesita el sm basico
enum class BMSState {
    CONNECTING,
    OPERATIONAL,
    FAULT
};

// Crear estados 
constexpr auto connecting_state = make_state(BMSState::CONNECTING,
    Transition<BMSState>{BMSState::OPERATIONAL, &Guards::connection_finished}
);

constexpr auto operational_state = make_state(BMSState::OPERATIONAL,
    Transition<BMSState>{BMSState::FAULT, &Guards::fault_during_operation},
    Transition<BMSState>{BMSState::FAULT, &HVBMS::Sensors::check_sdc}
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
    bms_sm.add_enter_action([](){HVBMS::Actuators::open_HV();}, fault_state);
    bms_sm.add_enter_action([](){Fault::fault_to_cs();}, fault_state); // Necesito control station
    bms_sm.add_enter_action([](){HVBMS::Sensors::open_sdc();}, fault_state);

    // Al entrar a CONNECTING
    bms_sm.add_enter_action([](){Connecting::connect_tcp();}, connecting_state);
    bms_sm.add_enter_action([](){Connecting::connect_udp();}, connecting_state);

    // Al entrar a OPERATIONAL
    bms_sm.add_enter_action([](){HVBMS::Actuators::start_precharge();}, operational_state);
    bms_sm.add_enter_action([](){HVBMS::Sensors::init();}, operational_state);


    // Acciones CÍCLICAS
    using namespace std::chrono_literals;
    // Actualizar voltaje y corriente
    bms_sm.add_cyclic_action(HVBMS::Sensors::update_voltage, 10ms, operational_state);
    // Luego tendre que crear otro de current con una frecuencia distinta
    // De momento se queda asi
    bms_sm.add_cyclic_action(HVBMS::Sensors::update_current, 1ms, operational_state);

    return bms_sm;
}

inline auto BSM = build_bms_state_machine();

void remove_errors(){
    for(int i = 0; i < 10; i++){
        
    }
}