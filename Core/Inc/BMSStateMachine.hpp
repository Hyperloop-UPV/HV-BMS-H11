#pragma once
#include "Actions/operational.hpp"
#include "Actions/connecting.hpp"
#include "Actions/fault.hpp"
#include "Guards/guards.hpp"
#include "Sensors/Sensors.hpp"
#include "Actuators/Actuators.hpp"
#include "Comms/Comms.hpp"
#include "ST-LIB.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionManager.hpp"

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

constexpr auto operational_state = make_state(BMSState::OPERATIONAL);


constexpr auto fault_state = make_state(BMSState::FAULT);

// Crear maquina de estados
static constinit auto state_machine = []() consteval {
    auto bms_sm = make_state_machine(
        BMSState::CONNECTING,
        connecting_state,
        operational_state,
        fault_state
    );


    // -- ACCIONES --

    // Al entrar a FAULT
    bms_sm.add_enter_action([](){HVBMS::Actuators::open_HV();}, fault_state);
    bms_sm.add_enter_action([](){HVBMS::Actuators::open_sdc();}, fault_state);
    bms_sm.add_enter_action([](){HVBMS::Actuators::fault_led();}, fault_state);

    // Al entrar a CONNECTING
    bms_sm.add_enter_action([](){HVBMS::Comms::start();}, connecting_state);

    // Al entrar a OPERATIONAL
    //bms_sm.add_enter_action([](){HVBMS::Actuators::operational_led();}, operational_state);
    bms_sm.add_enter_action([](){HVBMS::Actuators::init();}, operational_state);
    bms_sm.add_enter_action([](){HVBMS::Sensors::init();}, operational_state);

    // Al salir de OPERATIONAL
    bms_sm.add_exit_action([](){HVBMS::Actuators::operational_led();}, operational_state);

    // Acciones CÍCLICAS
    using namespace std::chrono_literals;
    bms_sm.add_cyclic_action(HVBMS::Actuators::operational_led, 1000ms, connecting_state);
    // Actualizar voltaje y corriente
    bms_sm.add_cyclic_action(HVBMS::Sensors::update_voltage, 10ms, operational_state);
    // Luego tendre que crear otro de current con una frecuencia distinta
    // De momento se queda asi
    bms_sm.add_cyclic_action(HVBMS::Sensors::update_current, 1ms, operational_state);

    return bms_sm;
}();


