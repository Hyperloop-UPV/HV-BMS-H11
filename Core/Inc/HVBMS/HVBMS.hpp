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
   private:
    static bool are_we_connected() {
        bool faulted = !OrderPackets::vcu_tcp->is_connected() ||
                       !Eth::eth_instance->is_connected();
        if (faulted) FAULT("Connection with VCU/cs lost");
        return faulted;
    }

   public:
    inline static DataPackets::sm_status current_sm_state{DataPackets::sm_status::Connecting};

    static void update();
    static void on_fault_enter();

    // Crear estados
    static constexpr auto connecting_state =
        make_state(DataPackets::sm_status::Connecting,
                   Transition<DataPackets::sm_status>{
                       DataPackets::sm_status::Idle, []() {
                           return OrderPackets::vcu_tcp->is_connected() &&
                                  Eth::eth_instance->is_connected() && Comms::adj_passed;
                       }});

    static constexpr auto idle_state =
        make_state(DataPackets::sm_status::Idle,
                   Transition<DataPackets::sm_status>{
                       DataPackets::sm_status::Ready_To_Precharge,
                       []() { return SDC::status == DataPackets::sdc_status::ENGAGED; }},
                   Transition<DataPackets::sm_status>{DataPackets::sm_status::FAULT,
                                                      []() { return are_we_connected(); }});

    static constexpr auto rtp_state =
        make_state(DataPackets::sm_status::Ready_To_Precharge,
                   Transition<DataPackets::sm_status>{DataPackets::sm_status::Precharging,
                                                      []() { return Actuators::is_precharging(); }},
                   Transition<DataPackets::sm_status>{DataPackets::sm_status::FAULT,
                                                      []() { return are_we_connected(); }});

    static constexpr auto precharging_state = make_state(
        DataPackets::sm_status::Precharging,
        Transition<DataPackets::sm_status>{DataPackets::sm_status::Energized,
                                           []() { return !Actuators::is_precharging(); }},
        Transition<DataPackets::sm_status>{DataPackets::sm_status::FAULT,
                                           []() { return are_we_connected(); }});

    static constexpr auto energized_state =
        make_state(DataPackets::sm_status::Energized,
                   Transition<DataPackets::sm_status>{DataPackets::sm_status::Ready_To_Precharge,
                                                      []() { return Actuators::is_HV_open(); }},
                   Transition<DataPackets::sm_status>{DataPackets::sm_status::FAULT,
                                                      []() { return are_we_connected(); }});

    static constexpr auto fault_state = make_state(DataPackets::sm_status::FAULT);

    // Crear maquina de estados
    static inline constinit auto state_machine = []() consteval {
        auto operational_sm =
            make_state_machine(DataPackets::sm_status::Connecting, connecting_state, idle_state,
                               rtp_state, precharging_state, energized_state, fault_state);

        using namespace std::chrono_literals;

        operational_sm.add_enter_action([]() { Comms::start(); }, connecting_state);

        operational_sm.add_enter_action([]() { DO::operational_led->turn_on(); }, idle_state);

        operational_sm.add_cyclic_action([]() { Actuators::toggle_operational_led(); }, 300ms,
                                         connecting_state);

        operational_sm.add_enter_action(
            []() {
                Sensors::cancel_sensor_task();
                Sensors::create_sensor_task(1000);
            },
            precharging_state);

        operational_sm.add_exit_action(
            []() {
                Sensors::cancel_sensor_task();
                Sensors::create_sensor_task(10000);
            },
            precharging_state);

        return operational_sm;
    }();
};
