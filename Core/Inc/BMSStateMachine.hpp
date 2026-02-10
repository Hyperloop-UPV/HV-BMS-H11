#pragma once
#include "Actuators/Actuators.hpp"
#include "Communications/Packets/DataPackets.hpp"
#include "Communications/Packets/OrderPackets.hpp"
#include "Control/Control.hpp"
#include "HVBMS.hpp"
#include "Protections/ProtectionManager.hpp"
#include "ST-LIB.hpp"
#include "Sensors/Sensors.hpp"


namespace HVBMS {
namespace SM {
static DataPackets::gsm_status gsm_state{DataPackets::gsm_status::CONNECTING};

// Crear estados
constexpr auto connecting_state = make_state(
    DataPackets::gsm_status::CONNECTING,
    Transition<DataPackets::gsm_status>{DataPackets::gsm_status::OPERATIONAL,
                                        []() { return DataPackets; }},
    Transition<DataPackets::gsm_status>{DataPackets::gsm_status::FAULT,
                                        []() { return HVBMS::Sensors::sdc.is_sdc_open(); }});

constexpr auto operational_state = make_state(
    DataPackets::gsm_status::OPERATIONAL,
    Transition<DataPackets::gsm_status>{DataPackets::gsm_status::FAULT,
                                        []() { return !HVBMS::Comms::tcp_connected(); }},
    Transition<DataPackets::gsm_status>{DataPackets::gsm_status::FAULT,
                                        []() { return HVBMS::Sensors::sdc.is_sdc_open(); }});

constexpr auto fault_state = make_state(DataPackets::gsm_status::FAULT);

// Crear maquina de estados
inline constinit StateMachine<DataPackets::gsm_status, 3U, 4U> state_machine = []() consteval {
    StateMachine<DataPackets::gsm_status, 3U, 4U> bms_sm = make_state_machine(
        DataPackets::gsm_status::CONNECTING, connecting_state, operational_state, fault_state);

    // Acciones ON ENTRY
    // CONNECTING
    bms_sm.add_enter_action(
        []() {
            HVBMS::Comms::start();
            HVBMS::Sensors::sdc.enable();
            HVBMS::Global::sdc_obccu->turn_on();
            HVBMS::Comms::create_packets();
            ProtectionManager::link_state_machine(state_machine, DataPackets::gsm_status::FAULT);
            HVBMS::Control::add_protections();
        },
        connecting_state);

    // OPERATIONAL
    bms_sm.add_enter_action([]() { HVBMS::Global::operational_led->turn_on(); }, operational_state);

    // FAULT
    bms_sm.add_enter_action(
        []() {
            HVBMS::Actuators::open_HV();
            HVBMS::Global::sdc_obccu->turn_off();
            HVBMS::Global::fault_led->turn_on();
        },
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
    bms_sm.add_exit_action([]() { HVBMS::Global::operational_led->turn_off(); }, operational_state);

    return bms_sm;
}();
};  // namespace SM
};  // namespace HVBMS