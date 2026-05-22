#include "HVBMS/HVBMS.hpp"

#include "HVBMS/Data/Data.hpp"


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
                FAULT("Precharge failed");
            });

            id_check_precharge = Scheduler::register_task(100, []() {
                if (ADC_reading::voltage_reading / Sensors::batteries.total_voltage >= 0.95) {
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
        if (OrderPackets::bypass_imd_flag) {
            // OrderPackets::bypass_imd_flag = false;
            // DO::imd_bypass->toggle(); no tengo bypass aqui
        }
        Scheduler::cancel_timeout(id_timeout_precharge);
        Scheduler::unregister_task(id_check_precharge);
    }
    if (OrderPackets::bypass_imd_flag) {
        //OrderPackets::bypass_imd_flag = false;
        //DO::imd_bypass->toggle(); no tengo bypass aqui
    }
    if (OrderPackets::FAULT_flag) {
        FAULT("FAULT order triggered");
    }

    current_gsm_state = state_machine.get_current_state();
}


void HVBMS::on_fault_enter(){
    Actuators::open_HV();
    DO::sdc_fw_fault->turn_off();
    DO::operational_led->turn_off();
    DO::fault_led->turn_on();
}
