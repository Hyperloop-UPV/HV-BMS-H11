#pragma once
#include "Actuators/Actuators.hpp"
#include "Comms/Comms.hpp"
#include "ST-LIB.hpp"
#include "HVBMS.hpp"

// Estados que necesita el sm basico
enum BMSState : uint8_t{
    CONNECTING, 
    OPERATIONAL,
    FAULT
};

// Crear estados 
constexpr auto connecting_state = make_state(BMSState::CONNECTING,
    Transition<BMSState>{BMSState::OPERATIONAL, [](){return HVBMS::Comms::tcp_connected();}}
);

constexpr auto operational_state = make_state(BMSState::OPERATIONAL,
    Transition<BMSState>{BMSState::FAULT, [](){return !HVBMS::Comms::tcp_connected();}}
);


constexpr auto fault_state = make_state(BMSState::FAULT);

// Crear maquina de estados
inline constinit auto state_machine = []() consteval {
    auto bms_sm = make_state_machine(
        BMSState::CONNECTING,
        connecting_state,   
        operational_state,
        fault_state
    );


    // -- ACCIONES --

    // Acciones ON ENTRY
    // CONNECTING
    bms_sm.add_enter_action([](){HVBMS::Comms::start();}, connecting_state);
    
    // FAULT
    bms_sm.add_enter_action([](){HVBMS::Global::fault_led->turn_on();}, fault_state);
    bms_sm.add_enter_action([](){HVBMS::Global::operational_led->turn_off();}, fault_state);
    
    // OPERATIONAL
    bms_sm.add_enter_action([](){HVBMS::Global::operational_led->turn_on();}, operational_state);
    //bms_sm.add_enter_action([](){HVBMS::Comms::create_packets();}, operational_state);
    bms_sm.add_enter_action([](){HVBMS::Comms::create_packets();}, operational_state);
   
    // Acciones CÍCLICAS
    using namespace std::chrono_literals;
    // CONNECTING
    bms_sm.add_cyclic_action(HVBMS::Actuators::toggle_operational_led, 1000ms, connecting_state);
    bms_sm.add_cyclic_action(HVBMS::Comms::send_packets, 1000ms, operational_state);
    // Actualizar voltaje y corriente
    //bms_sm.add_cyclic_action(HVBMS::Sensors::update_voltage, 10ms, operational_state);
    // Luego tendre que crear otro de current con una frecuencia distinta
    // De momento se queda asi
    //bms_sm.add_cyclic_action(HVBMS::Sensors::update_current, 1ms, operational_state);

    return bms_sm;
}();

