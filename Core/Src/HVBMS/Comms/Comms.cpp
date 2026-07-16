#include "HVBMS/Comms/Comms.hpp"

#include "../../Runes/generated_metadata.cpp"
#include "HVBMS/HVBMS.hpp"

#define ADJ_CHECK 1

void Comms::start() {
    DataPackets::High_Voltage_System_init(
        Batteries::get_min_voltage(), Batteries::get_max_voltage(), Batteries::get_min_temp(),
        Batteries::get_max_temp(), ADC_reading::current_reading, ADC_reading::voltage_reading,
        Sensors::battery_h11.total_global_voltage, HVBMS::current_sm_state);

    DataPackets::SOC_init(Batteries::SOC);

    DataPackets::SDC_init(Sensors::sdc.status);

    DataPackets::High_Voltage_Batteries_init(
        Sensors::battery_h11.battery[2].cells[0], Sensors::battery_h11.battery[2].cells[1],
        Sensors::battery_h11.battery[2].cells[2], Sensors::battery_h11.battery[2].cells[3],
        Sensors::battery_h11.battery[2].cells[4], Sensors::battery_h11.battery[2].cells[5],
        Sensors::battery_h11.battery[2].cells[6], Sensors::battery_h11.battery[2].cells[7],
        Sensors::battery_h11.battery[2].cells[8], Sensors::battery_h11.battery[2].cells[9],
        Sensors::battery_h11.battery[2].cells[10], Sensors::battery_h11.battery[2].cells[11],
        Sensors::battery_h11.battery[2].total_voltage, Sensors::battery_h11.temperature[10],
        Sensors::battery_h11.temperature[1], Sensors::battery_h11.temperature[2],
        Sensors::battery_h11.temperature[3], Sensors::battery_h11.battery[1].cells[0],
        Sensors::battery_h11.battery[1].cells[1], Sensors::battery_h11.battery[1].cells[2],
        Sensors::battery_h11.battery[1].cells[3], Sensors::battery_h11.battery[1].cells[4],
        Sensors::battery_h11.battery[1].cells[5], Sensors::battery_h11.battery[1].cells[6],
        Sensors::battery_h11.battery[1].cells[7], Sensors::battery_h11.battery[1].cells[8],
        Sensors::battery_h11.battery[1].cells[9], Sensors::battery_h11.battery[1].cells[10],
        Sensors::battery_h11.battery[1].cells[11], Sensors::battery_h11.battery[1].total_voltage,
        Sensors::battery_h11.temperature[4], Sensors::battery_h11.temperature[5],
        Sensors::battery_h11.temperature[6], Sensors::battery_h11.temperature[7],
        Sensors::battery_h11.battery[2].cells[0], Sensors::battery_h11.battery[2].cells[1],
        Sensors::battery_h11.battery[2].cells[2], Sensors::battery_h11.battery[2].cells[3],
        Sensors::battery_h11.battery[2].cells[4], Sensors::battery_h11.battery[2].cells[5],
        Sensors::battery_h11.battery[2].cells[6], Sensors::battery_h11.battery[2].cells[7],
        Sensors::battery_h11.battery[2].cells[8], Sensors::battery_h11.battery[2].cells[9],
        Sensors::battery_h11.battery[2].cells[10], Sensors::battery_h11.battery[2].cells[11],
        Sensors::battery_h11.battery[2].total_voltage, Sensors::battery_h11.temperature[8],
        Sensors::battery_h11.temperature[9], Sensors::battery_h11.temperature[10],
        Sensors::battery_h11.temperature[11], Sensors::battery_h11.battery[3].cells[0],
        Sensors::battery_h11.battery[3].cells[1], Sensors::battery_h11.battery[3].cells[2],
        Sensors::battery_h11.battery[3].cells[3], Sensors::battery_h11.battery[3].cells[4],
        Sensors::battery_h11.battery[3].cells[5], Sensors::battery_h11.battery[3].cells[6],
        Sensors::battery_h11.battery[3].cells[7], Sensors::battery_h11.battery[3].cells[8],
        Sensors::battery_h11.battery[3].cells[9], Sensors::battery_h11.battery[3].cells[10],
        Sensors::battery_h11.battery[3].cells[11], Sensors::battery_h11.battery[3].total_voltage,
        Sensors::battery_h11.temperature[12], Sensors::battery_h11.temperature[13],
        Sensors::battery_h11.temperature[14], Sensors::battery_h11.temperature[15],
        Sensors::battery_h11.battery[6].cells[0], Sensors::battery_h11.battery[6].cells[1],
        Sensors::battery_h11.battery[6].cells[2], Sensors::battery_h11.battery[6].cells[3],
        Sensors::battery_h11.battery[6].cells[4], Sensors::battery_h11.battery[6].cells[5],
        Sensors::battery_h11.battery[6].cells[6], Sensors::battery_h11.battery[6].cells[7],
        Sensors::battery_h11.battery[6].cells[8], Sensors::battery_h11.battery[6].cells[9],
        Sensors::battery_h11.battery[6].cells[10], Sensors::battery_h11.battery[6].cells[11],
        Sensors::battery_h11.battery[6].total_voltage, Sensors::battery_h11.temperature[16],
        Sensors::battery_h11.temperature[17], Sensors::battery_h11.temperature[18],
        Sensors::battery_h11.temperature[19], Sensors::battery_h11.battery[5].cells[0],
        Sensors::battery_h11.battery[5].cells[1], Sensors::battery_h11.battery[5].cells[2],
        Sensors::battery_h11.battery[5].cells[3], Sensors::battery_h11.battery[5].cells[4],
        Sensors::battery_h11.battery[5].cells[5], Sensors::battery_h11.battery[5].cells[6],
        Sensors::battery_h11.battery[5].cells[7], Sensors::battery_h11.battery[5].cells[8],
        Sensors::battery_h11.battery[5].cells[9], Sensors::battery_h11.battery[5].cells[10],
        Sensors::battery_h11.battery[5].cells[11], Sensors::battery_h11.battery[5].total_voltage,
        Sensors::battery_h11.temperature[20], Sensors::battery_h11.temperature[21],
        Sensors::battery_h11.temperature[22], Sensors::battery_h11.temperature[23],
        Sensors::battery_h11.battery[6].cells[0], Sensors::battery_h11.battery[6].cells[1],
        Sensors::battery_h11.battery[6].cells[2], Sensors::battery_h11.battery[6].cells[3],
        Sensors::battery_h11.battery[6].cells[4], Sensors::battery_h11.battery[6].cells[5],
        Sensors::battery_h11.battery[6].cells[6], Sensors::battery_h11.battery[6].cells[7],
        Sensors::battery_h11.battery[6].cells[8], Sensors::battery_h11.battery[6].cells[9],
        Sensors::battery_h11.battery[6].cells[10], Sensors::battery_h11.battery[6].cells[11],
        Sensors::battery_h11.battery[6].total_voltage, Sensors::battery_h11.temperature[24],
        Sensors::battery_h11.temperature[25], Sensors::battery_h11.temperature[26],
        Sensors::battery_h11.temperature[27], Sensors::battery_h11.battery[7].cells[0],
        Sensors::battery_h11.battery[7].cells[1], Sensors::battery_h11.battery[7].cells[2],
        Sensors::battery_h11.battery[7].cells[3], Sensors::battery_h11.battery[7].cells[4],
        Sensors::battery_h11.battery[7].cells[5], Sensors::battery_h11.battery[7].cells[6],
        Sensors::battery_h11.battery[7].cells[7], Sensors::battery_h11.battery[7].cells[8],
        Sensors::battery_h11.battery[7].cells[9], Sensors::battery_h11.battery[7].cells[10],
        Sensors::battery_h11.battery[7].cells[11], Sensors::battery_h11.battery[7].total_voltage,
        Sensors::battery_h11.temperature[28], Sensors::battery_h11.temperature[29],
        Sensors::battery_h11.temperature[30], Sensors::battery_h11.temperature[31]);

    DataPackets::IMD_init(Sensors::imd.status, Sensors::imd.resistance, Sensors::imd.is_ok,
                          Sensors::imd.duty, Sensors::imd.freq);

    DataPackets::Contactor_Status_init(
        Actuators::get_contactor_discharge_state(), Actuators::get_contactor_precharge_state(),
        Actuators::get_contactor_low_state(), Actuators::get_contactor_high_state(),
        Actuators::get_contactor_common_high_state());

    DataPackets::start();

    OrderPackets::Open_Contactors_init();
    OrderPackets::Cell_Balance_init();
    OrderPackets::Start_Precharge_init();
    OrderPackets::FAULT_init();
    OrderPackets::Check_Faults_init();

    OrderPackets::start();

    FaultController::register_fault_propagation(OrderPackets::vcu_tcp, OrderPackets::FAULT_order);
#if ADJ_CHECK
    adj_commit_order = new HeapOrder(0xFFFF, &check_adj_commit, &adj_remote_id);
#else
    adj_passed = true;
#endif
}

void Comms::check_adj_commit() {
    uint64_t hash_flat =
        ((uint64_t)ADJ_COMMIT_HASH[0]) | ((uint64_t)ADJ_COMMIT_HASH[1] << 8) |
        ((uint64_t)ADJ_COMMIT_HASH[2] << 16) | ((uint64_t)ADJ_COMMIT_HASH[3] << 24) |
        ((uint64_t)ADJ_COMMIT_HASH[4] << 32) | ((uint64_t)ADJ_COMMIT_HASH[5] << 40) |
        ((uint64_t)ADJ_COMMIT_HASH[6] << 48) | ((uint64_t)ADJ_COMMIT_HASH[7] << 56);
    bool ok = adj_remote_id == hash_flat;
    if (!ok) {
        char buf[16];
        buf[0] = ((adj_remote_id >> 0) & 0xFF);
        buf[1] = ((adj_remote_id >> 8) & 0xFF);
        buf[2] = ((adj_remote_id >> 16) & 0xFF);
        buf[3] = ((adj_remote_id >> 24) & 0xFF);
        buf[4] = ((adj_remote_id >> 32) & 0xFF);
        buf[5] = ((adj_remote_id >> 40) & 0xFF);
        buf[6] = ((adj_remote_id >> 48) & 0xFF);
        buf[7] = ((adj_remote_id >> 56) & 0xFF);
        buf[8] = 0;
        FAULT("ADJ commit doesn't match: %s (remote) and %s (HVBMS)", buf, ADJ_COMMIT_HASH);
    } else {
        adj_passed = true;
    }
}