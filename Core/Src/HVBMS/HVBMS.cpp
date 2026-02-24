#include "HVBMS/HVBMS.hpp"

#include "HVBMS/Data/Data.hpp"

#define set_protection_name(protection, name)                 \
    {                                                         \
        protection->set_name((char*)malloc(name.size() + 1)); \
        sprintf(protection->get_name(), "%s", name.c_str());  \
    }

void HVBMS::update() {
    state_machine.check_transitions();

    if (OrderPackets::start_precharge_flag) {
        OrderPackets::start_precharge_flag = false;

        if (SDC::status == DataPackets::sdc_status::DISENGAGED) {
            WARNING("SDC is disengaged, cannot start precharge");
        } else {
            Actuators::start_precharge();
            id_timeout_precharge = Scheduler::set_timeout(4000000, []() {
                Scheduler::unregister_task(id_check_precharge);
                Actuators::open_HV();
                HVBMS::state_machine.force_change_state(
                    (std::size_t)DataPackets::gsm_status::FAULT);
            });

            id_check_precharge = Scheduler::register_task(100, []() {
                if (Sensors::voltage_sensor.reading / Sensors::batteries.total_voltage >= 0.95) {
                    Scheduler::cancel_timeout(id_timeout_precharge);
                    Actuators::close_HV();
                    Scheduler::unregister_task(id_check_precharge);
                }
            });
        }
    }
    if (OrderPackets::open_contactors_flag) {
        OrderPackets::open_contactors_flag = false;
        Actuators::open_HV();
        Scheduler::cancel_timeout(id_timeout_precharge);
        Scheduler::unregister_task(id_check_precharge);
    }
    // DANGEROUS
    if (OrderPackets::close_contactors_flag) {
        OrderPackets::close_contactors_flag = false;
        Actuators::close_HV();
    }
    if (OrderPackets::bypass_imd_flag) {
        OrderPackets::bypass_imd_flag = false;
        DO::imd_bypass->toggle();
    }
    if (OrderPackets::FAULT_flag) {
        ProtectionManager::fault_and_propagate();
    }

    current_gsm_state = state_machine.get_current_state();
}

void HVBMS::add_protections() {
    ProtectionManager::link_state_machine(HVBMS::state_machine,
                                          static_cast<uint8_t>(DataPackets::gsm_status::FAULT));

    //ProtectionManager::add_standard_protections();
    ProtectionManager::initialize();

    // DC bus voltage
    ProtectionManager::_add_protection(&Sensors::voltage_sensor.reading,
                                       Boundary<float, ABOVE>{410});

    // Batteries current
    ProtectionManager::_add_protection(&Sensors::current_sensor.reading,
                                       Boundary<float, OUT_OF_RANGE>{-15, 70});


    Scheduler::register_task(1000, [](){ProtectionManager::check_protections();});
    
    // // SoCs
    // auto id{1};
    // for (auto& [_, soc] : Sensors::batteries.SoCs) {
    //     ProtectionManager::_add_protection(&soc, Boundary<float, BELOW>(0.24));
    //     ++id;
    // }

    // // Batteries conversion rate
    // id = 1;
    // for (auto& battery : Sensors::batteries.batteries) {
    //     ProtectionManager::_add_protection(&battery.conv_rate, Boundary<float, BELOW>(0.5));

    //     ++id;
    // }

    // // Batteries temperature
    // id = 1;
    // for (auto& temp : Sensors::batteries.batteries_temp) {
    //     ProtectionManager::_add_protection(&temp[0], Boundary<float, ABOVE>(60.0));
    //     ProtectionManager::_add_protection(&temp[1], Boundary<float, ABOVE>(60.0));
    //     ++id;
    // }
}
