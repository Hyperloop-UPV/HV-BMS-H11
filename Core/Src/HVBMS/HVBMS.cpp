#include "HVBMS/HVBMS.hpp"

#include "HVBMS/Data/Data.hpp"
#include "HVBMS/Sensors/BatteryH11.hpp"


void HVBMS::update() {

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
                // if (ADC_reading::voltage_reading / Sensors::batteries.total_voltage >= 0.95) {
                //     Scheduler::cancel_timeout(id_timeout_precharge);
                //     Actuators::close_HV();
                //     Scheduler::unregister_task(id_check_precharge);
                // }
            });
        }
    }
    if (OrderPackets::open_contactors_flag) {
        OrderPackets::open_contactors_flag = false;
        Actuators::open_HV();
        Scheduler::cancel_timeout(id_timeout_precharge);
        Scheduler::unregister_task(id_check_precharge);
    }
    if (OrderPackets::check_faults_flag) {
        bcc_status_t status;
        for (uint8_t cid = 1; cid <= Batteries::bcc_config.devicesCnt; cid++) {
            status = BCC_Fault_GetStatus(&Batteries::bcc_config, (bcc_cid_t)cid, &Batteries::faults);
            if (status != BCC_STATUS_SUCCESS) {
                FAULT("Could not read fault status: %s", get_bcc_error_str(status));
                return;
            }

            INFO("FAULT %u: %u", cid, Batteries::faults);
        }
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
