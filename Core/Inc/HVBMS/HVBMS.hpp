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

    static void update();
    static void on_fault_enter();

    inline static void control_station_disconnected() {
        if (!OrderPackets::control_station_tcp->is_connected()) {
            FAULT("Control station disconnected");
        }
    }

    // Crear estados
    static constexpr auto connecting_state =
        make_state(DataPackets::gsm_status::CONNECTING,
                   Transition<DataPackets::gsm_status>{
                       DataPackets::gsm_status::OPERATIONAL, []() {
                           return OrderPackets::control_station_tcp != nullptr &&
                                  OrderPackets::control_station_tcp->is_connected();
                       }});

    static constexpr auto operational_state =
        make_state(DataPackets::gsm_status::OPERATIONAL);


    // Crear maquina de estados
    static inline constinit StateMachine<DataPackets::gsm_status, 2U, 1U> state_machine =
        []() consteval {
            StateMachine<DataPackets::gsm_status, 2U, 1U> bms_sm =
                make_state_machine(DataPackets::gsm_status::CONNECTING, connecting_state,
                                   operational_state);

            // Acciones ON ENTRY
            // CONNECTING
            bms_sm.add_enter_action(
                 []() {
                     Comms::start();
                 },
                 connecting_state);

            // OPERATIONAL
            bms_sm.add_enter_action([]() { DO::operational_led->turn_on(); }, operational_state);

            // Acciones CÍCLICAS    
            using namespace std::chrono_literals;
            // CONNECTING
            bms_sm.add_cyclic_action([]() { Actuators::toggle_operational_led(); }, 300ms,
                                     connecting_state);

            // OPERATIONAL
            // Si me desconecto me tengo que ir a fault
            bms_sm.add_cyclic_action([]() { control_station_disconnected();}, 50ms, operational_state);
            return bms_sm;
        }();

};
