#include "HVBMS/Comms/Comms.hpp"

#include "HVBMS/HVBMS.hpp"

void Comms::start() {
    DataPackets::battery_1_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[0].cells[0],
        Sensors::battery_h11.battery[0].cells[1],
        Sensors::battery_h11.battery[0].cells[2],
        Sensors::battery_h11.battery[0].cells[3],
        Sensors::battery_h11.battery[0].cells[4],
        Sensors::battery_h11.battery[0].cells[5],
        Sensors::battery_h11.battery[0].total_voltage,
        Sensors::battery_h11.battery[0].conv_rate);

    DataPackets::battery_2_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[0].cells[6],
        Sensors::battery_h11.battery[0].cells[7],
        Sensors::battery_h11.battery[0].cells[8],
        Sensors::battery_h11.battery[0].cells[9],
        Sensors::battery_h11.battery[0].cells[10],
        Sensors::battery_h11.battery[0].cells[11],
        Sensors::battery_h11.battery[0].total_voltage,
        Sensors::battery_h11.battery[0].conv_rate);

    DataPackets::battery_3_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[1].cells[0],
        Sensors::battery_h11.battery[1].cells[1],
        Sensors::battery_h11.battery[1].cells[2],
        Sensors::battery_h11.battery[1].cells[3],
        Sensors::battery_h11.battery[1].cells[4],
        Sensors::battery_h11.battery[1].cells[5],
        Sensors::battery_h11.battery[1].total_voltage,
        Sensors::battery_h11.battery[1].conv_rate);

    DataPackets::battery_4_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[1].cells[6],
        Sensors::battery_h11.battery[1].cells[7],
        Sensors::battery_h11.battery[1].cells[8],
        Sensors::battery_h11.battery[1].cells[9],
        Sensors::battery_h11.battery[1].cells[10],
        Sensors::battery_h11.battery[1].cells[11],
        Sensors::battery_h11.battery[1].total_voltage,
        Sensors::battery_h11.battery[1].conv_rate);

    DataPackets::battery_5_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[2].cells[0],
        Sensors::battery_h11.battery[2].cells[1],
        Sensors::battery_h11.battery[2].cells[2],
        Sensors::battery_h11.battery[2].cells[3],
        Sensors::battery_h11.battery[2].cells[4],
        Sensors::battery_h11.battery[2].cells[5],
        Sensors::battery_h11.battery[2].total_voltage,
        Sensors::battery_h11.battery[2].conv_rate);

    DataPackets::battery_6_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[2].cells[6],
        Sensors::battery_h11.battery[2].cells[7],
        Sensors::battery_h11.battery[2].cells[8],
        Sensors::battery_h11.battery[2].cells[9],
        Sensors::battery_h11.battery[2].cells[10],
        Sensors::battery_h11.battery[2].cells[11],
        Sensors::battery_h11.battery[2].total_voltage,
        Sensors::battery_h11.battery[2].conv_rate);

    DataPackets::battery_7_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[3].cells[0],
        Sensors::battery_h11.battery[3].cells[1],
        Sensors::battery_h11.battery[3].cells[2],
        Sensors::battery_h11.battery[3].cells[3],
        Sensors::battery_h11.battery[3].cells[4],
        Sensors::battery_h11.battery[3].cells[5],
        Sensors::battery_h11.battery[3].total_voltage,
        Sensors::battery_h11.battery[3].conv_rate);

    DataPackets::battery_8_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[3].cells[6],
        Sensors::battery_h11.battery[3].cells[7],
        Sensors::battery_h11.battery[3].cells[8],
        Sensors::battery_h11.battery[3].cells[9],
        Sensors::battery_h11.battery[3].cells[10],
        Sensors::battery_h11.battery[3].cells[11],
        Sensors::battery_h11.battery[3].total_voltage,
        Sensors::battery_h11.battery[3].conv_rate);

    DataPackets::battery_9_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[4].cells[0],
        Sensors::battery_h11.battery[4].cells[1],
        Sensors::battery_h11.battery[4].cells[2],
        Sensors::battery_h11.battery[4].cells[3],
        Sensors::battery_h11.battery[4].cells[4],
        Sensors::battery_h11.battery[4].cells[5],
        Sensors::battery_h11.battery[4].total_voltage,
        Sensors::battery_h11.battery[4].conv_rate);

    DataPackets::battery_10_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[4].cells[6],
        Sensors::battery_h11.battery[4].cells[7],
        Sensors::battery_h11.battery[4].cells[8],
        Sensors::battery_h11.battery[4].cells[9],
        Sensors::battery_h11.battery[4].cells[10],
        Sensors::battery_h11.battery[4].cells[11],
        Sensors::battery_h11.battery[4].total_voltage,
        Sensors::battery_h11.battery[4].conv_rate);

    DataPackets::battery_11_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[5].cells[0],
        Sensors::battery_h11.battery[5].cells[1],
        Sensors::battery_h11.battery[5].cells[2],
        Sensors::battery_h11.battery[5].cells[3],
        Sensors::battery_h11.battery[5].cells[4],
        Sensors::battery_h11.battery[5].cells[5],
        Sensors::battery_h11.battery[5].total_voltage,
        Sensors::battery_h11.battery[5].conv_rate);

    DataPackets::battery_12_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[5].cells[6],
        Sensors::battery_h11.battery[5].cells[7],
        Sensors::battery_h11.battery[5].cells[8],
        Sensors::battery_h11.battery[5].cells[9],
        Sensors::battery_h11.battery[5].cells[10],
        Sensors::battery_h11.battery[5].cells[11],
        Sensors::battery_h11.battery[5].total_voltage,
        Sensors::battery_h11.battery[5].conv_rate);

    DataPackets::battery_13_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[6].cells[0],
        Sensors::battery_h11.battery[6].cells[1],
        Sensors::battery_h11.battery[6].cells[2],
        Sensors::battery_h11.battery[6].cells[3],
        Sensors::battery_h11.battery[6].cells[4],
        Sensors::battery_h11.battery[6].cells[5],
        Sensors::battery_h11.battery[6].total_voltage,
        Sensors::battery_h11.battery[6].conv_rate);

    DataPackets::battery_14_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[6].cells[6],
        Sensors::battery_h11.battery[6].cells[7],
        Sensors::battery_h11.battery[6].cells[8],
        Sensors::battery_h11.battery[6].cells[9],
        Sensors::battery_h11.battery[6].cells[10],
        Sensors::battery_h11.battery[6].cells[11],
        Sensors::battery_h11.battery[6].total_voltage,
        Sensors::battery_h11.battery[6].conv_rate);

    DataPackets::battery_15_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[7].cells[0],
        Sensors::battery_h11.battery[7].cells[1],
        Sensors::battery_h11.battery[7].cells[2],
        Sensors::battery_h11.battery[7].cells[3],
        Sensors::battery_h11.battery[7].cells[4],
        Sensors::battery_h11.battery[7].cells[5],
        Sensors::battery_h11.battery[7].total_voltage,
        Sensors::battery_h11.battery[7].conv_rate);

    DataPackets::battery_16_init(
        Sensors::battery_h11.SOC,
        Sensors::battery_h11.battery[7].cells[6],
        Sensors::battery_h11.battery[7].cells[7],
        Sensors::battery_h11.battery[7].cells[8],
        Sensors::battery_h11.battery[7].cells[9],
        Sensors::battery_h11.battery[7].cells[10],
        Sensors::battery_h11.battery[7].cells[11],
        Sensors::battery_h11.battery[7].total_voltage,
        Sensors::battery_h11.battery[7].conv_rate);

    DataPackets::batteries_voltage_init(Sensors::battery_h11.total_voltage);

    DataPackets::voltage_sensor_init(ADC_reading::voltage_reading);

    DataPackets::current_sensor_init(ADC_reading::current_reading);

    DataPackets::general_state_machine_init(HVBMS::current_gsm_state);

    DataPackets::driver_diagnosis_init(Sensors::battery_h11.period_ms);

    DataPackets::sdc_init(Sensors::sdc.status);

    DataPackets::imd_init(Sensors::imd.status, Sensors::imd.resistance, Sensors::imd.is_ok, Sensors::imd.duty, Sensors::imd.freq);

    DataPackets::minimum_soc_init(Sensors::battery_h11.SOC);

    DataPackets::contactor_status_init(
        Actuators::get_contactor_discharge_state(), Actuators::get_contactor_precharge_state(),
        Actuators::get_contactor_low_state(), Actuators::get_contactor_high_state());

    DataPackets::batteries_data_init(
        Sensors::battery_h11.min_cell,
        Sensors::battery_h11.max_cell,
        Sensors::battery_h11.min_temperature,
        Sensors::battery_h11.max_temperature);

    DataPackets::start();

    OrderPackets::open_contactors_init();
    OrderPackets::start_precharge_init();
    OrderPackets::bypass_imd_init();
    OrderPackets::FAULT_init();

    OrderPackets::start();
}
