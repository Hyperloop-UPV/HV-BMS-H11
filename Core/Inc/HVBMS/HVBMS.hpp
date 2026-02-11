#pragma once

#include "HVBMS/Actuators/Actuators.hpp"
#include "HVBMS/Actuators/Contactor.hpp"
#include "HVBMS/Comms/Comms.hpp"
#include "HVBMS/Data/Data.hpp"
#include "HVBMS/Sensors/ADCLinearSensor.hpp"
#include "HVBMS/Sensors/BatteryPack.hpp"
#include "HVBMS/Sensors/SDC.hpp"
#include "HVBMS/Sensors/Sensors.hpp"
#include "ST-LIB.hpp"
#include "Protections/Protection.hpp"
#include "Protections/ProtectionManager.hpp"

class HVBMS {
   public:
    inline static States_HVBMS current_gsm_state{States_HVBMS::CONNECTING};
    inline static States_BMS current_BMS_state{States_BMS::OK};
    inline static std::vector<Protection*> protections;

    static void update();
    static void check_bms_status();
    static void add_protections();

    // Crear estados
    static constexpr auto connecting_state = make_state(
        DataPackets::gsm_status::CONNECTING,
        Transition<DataPackets::gsm_status>{
            DataPackets::gsm_status::OPERATIONAL,
            []() { return DataPackets::control_station_tcp->is_connected(); }},

        Transition<DataPackets::gsm_status>{DataPackets::gsm_status::FAULT,
                                            []() { return Sensors::sdc.is_sdc_open(); }});

    static constexpr auto operational_state = make_state(
        DataPackets::gsm_status::OPERATIONAL,
        Transition<DataPackets::gsm_status>{
            DataPackets::gsm_status::FAULT,
            []() { return !DataPackets::control_station_tcp->is_connected(); }},
        Transition<DataPackets::gsm_status>{DataPackets::gsm_status::FAULT,
                                            []() { return Sensors::sdc.is_sdc_open(); }});

    static constexpr auto fault_state = make_state(DataPackets::gsm_status::FAULT);

    // Crear maquina de estados
    static inline constinit StateMachine<DataPackets::gsm_status, 3U, 4U> state_machine = []() consteval {
        StateMachine<DataPackets::gsm_status, 3U, 4U> bms_sm = make_state_machine(
            DataPackets::gsm_status::CONNECTING, connecting_state, operational_state, fault_state);

        // Acciones ON ENTRY
        // CONNECTING
        bms_sm.add_enter_action(
            []() {
                Comms::start();
                Sensors::sdc.enable();
                DO::sdc_obccu->turn_on();
            },
            connecting_state);

        // OPERATIONAL
        bms_sm.add_enter_action([]() { DO::operational_led->turn_on(); },
                                operational_state);

        // FAULT
        bms_sm.add_enter_action(
            []() {
                Actuators::open_HV();
                DO::sdc_obccu->turn_off();
                DO::fault_led->turn_on();
            },
            fault_state);

        // Acciones CÍCLICAS
        using namespace std::chrono_literals;
        // CONNECTING
        bms_sm.add_cyclic_action(Actuators::toggle_operational_led, 1000ms,
                                 connecting_state);

        // OPERATIONAL
        bms_sm.add_cyclic_action(Sensors::update_sensors, 10ms, operational_state);
        bms_sm.add_cyclic_action(Sensors::update_batteries, 10ms,
        operational_state);

        // Acciones ON EXIT
        // OPERATIONAL
        bms_sm.add_exit_action([]() { DO::operational_led->turn_off(); },
                               operational_state);

        return bms_sm;
    }();
};