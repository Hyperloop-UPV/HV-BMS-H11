#pragma once
#include "Actuators/Actuators.hpp"
#include "Comms/Comms.hpp"
#include "Sensors/Sensors.hpp"
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
    Transition<BMSState>{BMSState::OPERATIONAL, [](){return HVBMS::Comms::tcp_connected();}},
    Transition<BMSState>{BMSState::FAULT, [](){return HVBMS::Sensors::sdc.is_sdc_open();}}
);

constexpr auto operational_state = make_state(BMSState::OPERATIONAL,
    Transition<BMSState>{BMSState::FAULT, [](){return !HVBMS::Comms::tcp_connected();}},
    Transition<BMSState>{BMSState::FAULT, [](){return HVBMS::Sensors::sdc.is_sdc_open();}}
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

    // Acciones ON ENTRY
    // CONNECTING
    bms_sm.add_enter_action([](){HVBMS::Comms::start();
                                 HVBMS::Sensors::sdc.enable();
                                 HVBMS::Global::sdc_obccu->turn_on();
                                 HVBMS::Comms::create_packets();},
                            connecting_state);

    // OPERATIONAL
    bms_sm.add_enter_action([](){HVBMS::Global::operational_led->turn_on();},
                            operational_state);
   
    // FAULT
    bms_sm.add_enter_action([](){HVBMS::Actuators::open_HV();
                                 HVBMS::Global::sdc_obccu->turn_off();
                                 HVBMS::Global::fault_led->turn_on();},
                            fault_state);
    
    // Acciones CÍCLICAS
    using namespace std::chrono_literals;
    // CONNECTING
    bms_sm.add_cyclic_action(HVBMS::Actuators::toggle_operational_led, 1000ms, connecting_state);
    
    // OPERATIONAL
    bms_sm.add_cyclic_action(HVBMS::Comms::send_packets, 1000ms, operational_state);
    bms_sm.add_cyclic_action(HVBMS::Sensors::update_sensors, 10ms, operational_state);
    bms_sm.add_cyclic_action(HVBMS::Sensors::update_batteries, 10ms, operational_state);

    // Acciones ON EXIT
    // OPERATIONAL
    bms_sm.add_exit_action([](){HVBMS::Global::operational_led->turn_off();}, operational_state);

    return bms_sm;
}();

