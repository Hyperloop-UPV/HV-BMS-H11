#include "HVBMS/Comms/Comms.hpp"

#include "HVBMS/HVBMS.hpp"

void Comms::start() {
    DataPackets::battery_1_init(
        Sensors::batteries.SoCs[0].second, Sensors::batteries.batteries[0].cells[0],
        Sensors::batteries.batteries[0].cells[1], Sensors::batteries.batteries[0].cells[2],
        Sensors::batteries.batteries[0].cells[3], Sensors::batteries.batteries[0].cells[4],
        Sensors::batteries.batteries[0].cells[5], Sensors::batteries.batteries[0].total_voltage,
        Sensors::batteries.batteries[0].conv_rate);

    DataPackets::battery_2_init(
        Sensors::batteries.SoCs[1].second, Sensors::batteries.batteries[1].cells[0],
        Sensors::batteries.batteries[1].cells[1], Sensors::batteries.batteries[1].cells[2],
        Sensors::batteries.batteries[1].cells[3], Sensors::batteries.batteries[1].cells[4],
        Sensors::batteries.batteries[1].cells[5], Sensors::batteries.batteries[1].total_voltage,
        Sensors::batteries.batteries[1].conv_rate);

    DataPackets::battery_3_init(
        Sensors::batteries.SoCs[2].second, Sensors::batteries.batteries[2].cells[0],
        Sensors::batteries.batteries[2].cells[1], Sensors::batteries.batteries[2].cells[2],
        Sensors::batteries.batteries[2].cells[3], Sensors::batteries.batteries[2].cells[4],
        Sensors::batteries.batteries[2].cells[5], Sensors::batteries.batteries[2].total_voltage,
        Sensors::batteries.batteries[2].conv_rate);

    DataPackets::battery_4_init(
        Sensors::batteries.SoCs[3].second, Sensors::batteries.batteries[3].cells[0],
        Sensors::batteries.batteries[3].cells[1], Sensors::batteries.batteries[3].cells[2],
        Sensors::batteries.batteries[3].cells[3], Sensors::batteries.batteries[3].cells[4],
        Sensors::batteries.batteries[3].cells[5], Sensors::batteries.batteries[3].total_voltage,
        Sensors::batteries.batteries[3].conv_rate);

    DataPackets::battery_5_init(
        Sensors::batteries.SoCs[4].second, Sensors::batteries.batteries[4].cells[0],
        Sensors::batteries.batteries[4].cells[1], Sensors::batteries.batteries[4].cells[2],
        Sensors::batteries.batteries[4].cells[3], Sensors::batteries.batteries[4].cells[4],
        Sensors::batteries.batteries[4].cells[5], Sensors::batteries.batteries[4].total_voltage,
        Sensors::batteries.batteries[4].conv_rate);

    DataPackets::battery_6_init(
        Sensors::batteries.SoCs[5].second, Sensors::batteries.batteries[5].cells[0],
        Sensors::batteries.batteries[5].cells[1], Sensors::batteries.batteries[5].cells[2],
        Sensors::batteries.batteries[5].cells[3], Sensors::batteries.batteries[5].cells[4],
        Sensors::batteries.batteries[5].cells[5], Sensors::batteries.batteries[5].total_voltage,
        Sensors::batteries.batteries[5].conv_rate);

    DataPackets::battery_7_init(
        Sensors::batteries.SoCs[6].second, Sensors::batteries.batteries[6].cells[0],
        Sensors::batteries.batteries[6].cells[1], Sensors::batteries.batteries[6].cells[2],
        Sensors::batteries.batteries[6].cells[3], Sensors::batteries.batteries[6].cells[4],
        Sensors::batteries.batteries[6].cells[5], Sensors::batteries.batteries[6].total_voltage,
        Sensors::batteries.batteries[6].conv_rate);

    DataPackets::battery_8_init(
        Sensors::batteries.SoCs[7].second, Sensors::batteries.batteries[7].cells[0],
        Sensors::batteries.batteries[7].cells[1], Sensors::batteries.batteries[7].cells[2],
        Sensors::batteries.batteries[7].cells[3], Sensors::batteries.batteries[7].cells[4],
        Sensors::batteries.batteries[7].cells[5], Sensors::batteries.batteries[7].total_voltage,
        Sensors::batteries.batteries[7].conv_rate);

    DataPackets::battery_9_init(
        Sensors::batteries.SoCs[8].second, Sensors::batteries.batteries[8].cells[0],
        Sensors::batteries.batteries[8].cells[1], Sensors::batteries.batteries[8].cells[2],
        Sensors::batteries.batteries[8].cells[3], Sensors::batteries.batteries[8].cells[4],
        Sensors::batteries.batteries[8].cells[5], Sensors::batteries.batteries[8].total_voltage,
        Sensors::batteries.batteries[8].conv_rate);

    DataPackets::battery_10_init(
        Sensors::batteries.SoCs[9].second, Sensors::batteries.batteries[9].cells[0],
        Sensors::batteries.batteries[9].cells[1], Sensors::batteries.batteries[9].cells[2],
        Sensors::batteries.batteries[9].cells[3], Sensors::batteries.batteries[9].cells[4],
        Sensors::batteries.batteries[9].cells[5], Sensors::batteries.batteries[9].total_voltage,
        Sensors::batteries.batteries[9].conv_rate);

    DataPackets::battery_11_init(
        Sensors::batteries.SoCs[10].second, Sensors::batteries.batteries[10].cells[0],
        Sensors::batteries.batteries[10].cells[1], Sensors::batteries.batteries[10].cells[2],
        Sensors::batteries.batteries[10].cells[3], Sensors::batteries.batteries[10].cells[4],
        Sensors::batteries.batteries[10].cells[5], Sensors::batteries.batteries[10].total_voltage,
        Sensors::batteries.batteries[10].conv_rate);

    DataPackets::battery_12_init(
        Sensors::batteries.SoCs[11].second, Sensors::batteries.batteries[11].cells[0],
        Sensors::batteries.batteries[11].cells[1], Sensors::batteries.batteries[11].cells[2],
        Sensors::batteries.batteries[11].cells[3], Sensors::batteries.batteries[11].cells[4],
        Sensors::batteries.batteries[11].cells[5], Sensors::batteries.batteries[11].total_voltage,
        Sensors::batteries.batteries[11].conv_rate);

    DataPackets::battery_13_init(
        Sensors::batteries.SoCs[12].second, Sensors::batteries.batteries[12].cells[0],
        Sensors::batteries.batteries[12].cells[1], Sensors::batteries.batteries[12].cells[2],
        Sensors::batteries.batteries[12].cells[3], Sensors::batteries.batteries[12].cells[4],
        Sensors::batteries.batteries[12].cells[5], Sensors::batteries.batteries[12].total_voltage,
        Sensors::batteries.batteries[12].conv_rate);

    DataPackets::battery_14_init(
        Sensors::batteries.SoCs[13].second, Sensors::batteries.batteries[13].cells[0],
        Sensors::batteries.batteries[13].cells[1], Sensors::batteries.batteries[13].cells[2],
        Sensors::batteries.batteries[13].cells[3], Sensors::batteries.batteries[13].cells[4],
        Sensors::batteries.batteries[13].cells[5], Sensors::batteries.batteries[13].total_voltage,
        Sensors::batteries.batteries[13].conv_rate);

    DataPackets::battery_15_init(
        Sensors::batteries.SoCs[14].second, Sensors::batteries.batteries[14].cells[0],
        Sensors::batteries.batteries[14].cells[1], Sensors::batteries.batteries[14].cells[2],
        Sensors::batteries.batteries[14].cells[3], Sensors::batteries.batteries[14].cells[4],
        Sensors::batteries.batteries[14].cells[5], Sensors::batteries.batteries[14].total_voltage,
        Sensors::batteries.batteries[14].conv_rate);

    DataPackets::battery_16_init(
        Sensors::batteries.SoCs[15].second, Sensors::batteries.batteries[15].cells[0],
        Sensors::batteries.batteries[15].cells[1], Sensors::batteries.batteries[15].cells[2],
        Sensors::batteries.batteries[15].cells[3], Sensors::batteries.batteries[15].cells[4],
        Sensors::batteries.batteries[15].cells[5], Sensors::batteries.batteries[15].total_voltage,
        Sensors::batteries.batteries[15].conv_rate);

    DataPackets::battery_17_init(
        Sensors::batteries.SoCs[16].second, Sensors::batteries.batteries[16].cells[0],
        Sensors::batteries.batteries[16].cells[1], Sensors::batteries.batteries[16].cells[2],
        Sensors::batteries.batteries[16].cells[3], Sensors::batteries.batteries[16].cells[4],
        Sensors::batteries.batteries[16].cells[5], Sensors::batteries.batteries[16].total_voltage,
        Sensors::batteries.batteries[16].conv_rate);

    DataPackets::battery_18_init(
        Sensors::batteries.SoCs[17].second, Sensors::batteries.batteries[17].cells[0],
        Sensors::batteries.batteries[17].cells[1], Sensors::batteries.batteries[17].cells[2],
        Sensors::batteries.batteries[17].cells[3], Sensors::batteries.batteries[17].cells[4],
        Sensors::batteries.batteries[17].cells[5], Sensors::batteries.batteries[17].total_voltage,
        Sensors::batteries.batteries[17].conv_rate);

    DataPackets::batteries_voltage_init(Sensors::batteries.total_voltage);

    DataPackets::voltage_sensor_init(Sensors::voltage_sensor.reading);

    DataPackets::current_sensor_init(Sensors::current_sensor.reading);

    DataPackets::general_state_machine_init(HVBMS::current_gsm_state);

    DataPackets::driver_diagnosis_init(Sensors::batteries.period);

    DataPackets::sdc_init(Sensors::sdc.status);

    DataPackets::minimum_soc_init(Sensors::batteries.minimum_soc);

    DataPackets::bms_init(HVBMS::current_BMS_state);

    DataPackets::batteries_data_init(
        Sensors::batteries.minimum_cell_voltage, Sensors::batteries.maximum_cell_voltage,
        Sensors::batteries.minimum_temp, Sensors::batteries.maximum_temp);

    DataPackets::start();
    OrderPackets::start();
}
