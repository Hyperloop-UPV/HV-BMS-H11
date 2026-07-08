#pragma once

#include "HVBMS/Actuators/Actuators.hpp"
#include "HVBMS/Actuators/Contactor.hpp"
#include "HVBMS/Comms/Comms.hpp"
#include "HVBMS/Data/Data.hpp"
#include "HVBMS/Sensors/BatteryPack.hpp"
#include "HVBMS/Sensors/SDC.hpp"
#include "HVBMS/Sensors/Sensors.hpp"
#include "Protections/Protection.hpp"
#include "ST-LIB.hpp"

class HVBMS {
   public:
    inline static DataPackets::gsm_status current_gsm_state{DataPackets::gsm_status::CONNECTING};
    inline static DataPackets::nested_sm_status current_nested_sm_state{
        DataPackets::nested_sm_status::OPERATIONAL};

    inline static bool are_we_precharging{false};
    inline static uint16_t fault_sensor_task_id{Scheduler::INVALID_ID};
    static void update();
    static void on_fault_enter();

    inline static void control_station_disconnected() {
    if (!OrderPackets::control_station_tcp->is_connected() || !Eth::eth_instance->is_connected()) {
            FAULT("Control station disconnected");
        }
    }

// Crear estados
    static constexpr auto connecting_state =
        make_state(DataPackets::gsm_status::CONNECTING,
                   Transition<DataPackets::gsm_status>{
                       DataPackets::gsm_status::OPERATIONAL, []() {
                           return OrderPackets::control_station_tcp->is_connected() &&
                                  Eth::eth_instance->is_connected();
                       }});

    static constexpr auto operational_state = make_state(DataPackets::gsm_status::OPERATIONAL);

    static constexpr auto nested_precharge_state = make_state(
        DataPackets::nested_sm_status::PRECHARGE,
        Transition<DataPackets::nested_sm_status>{DataPackets::nested_sm_status::OPERATIONAL,
                                                  []() { return !are_we_precharging; }});

    static constexpr auto nested_operational_state = make_state(
        DataPackets::nested_sm_status::OPERATIONAL,
        Transition<DataPackets::nested_sm_status>{DataPackets::nested_sm_status::PRECHARGE,
                                                  []() { return are_we_precharging; }});

    // Crear maquina de estados operacional
    static inline constinit StateMachine<DataPackets::nested_sm_status, 2U, 2U>
        nested_state_machine = []() consteval {
            StateMachine<DataPackets::nested_sm_status, 2U, 2U> operational_sm =
                make_state_machine(DataPackets::nested_sm_status::OPERATIONAL,
                                   nested_operational_state, nested_precharge_state);

            using namespace std::chrono_literals;
            operational_sm.add_cyclic_action([]() { Sensors::update_sensors(); }, 10ms,
                                             nested_operational_state);

            operational_sm.add_cyclic_action([]() { Sensors::update_sensors(); }, 1ms,
                                             nested_precharge_state);
            return operational_sm;
        }();

    // Crear maquina de estados general
    static inline constinit StateMachine<DataPackets::gsm_status, 2U, 1U,
                                         StateMachine<DataPackets::nested_sm_status, 2U, 2U>>
        state_machine = []() consteval {
            NestedMachineBinding<DataPackets::gsm_status,
                                 StateMachine<DataPackets::nested_sm_status, 2U, 2U>>
                nested = StateMachineHelper::add_nesting(operational_state, nested_state_machine);
            StateMachine<DataPackets::gsm_status, 2U, 1U,
                         StateMachine<DataPackets::nested_sm_status, 2U, 2U>>
                bms_sm = make_state_machine(DataPackets::gsm_status::CONNECTING,
                                            StateMachineHelper::add_nested_machines(nested),
                                            connecting_state, operational_state);

            // Acciones ON ENTRY
            // CONNECTING
            bms_sm.add_enter_action([]() { Comms::start(); }, connecting_state);

            // OPERATIONAL
            bms_sm.add_enter_action([]() { DO::operational_led->turn_on(); }, operational_state);

            // Acciones CÍCLICAS
            using namespace std::chrono_literals;
            // CONNECTING
            bms_sm.add_cyclic_action([]() { Actuators::toggle_operational_led(); }, 300ms,
                                     connecting_state);

            bms_sm.add_cyclic_action([]() { Sensors::update_sensors(); }, 10ms, connecting_state);

            // OPERATIONAL
            // Si me desconecto me tengo que ir a fault
            bms_sm.add_cyclic_action([]() { control_station_disconnected(); }, 50ms,
                                     operational_state);
            return bms_sm;
        }();
}
;
