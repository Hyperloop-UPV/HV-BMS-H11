#include "HVBMS/Comms/Comms.hpp"

#include "HVBMS/HVBMS.hpp"

void Comms::start() {
    DataPackets::High_Voltage_System_init(
        Batteries::get_min_voltage(), Batteries::get_max_voltage(), Batteries::get_min_temp(),
        Batteries::get_max_temp(), ADC_reading::current_reading, ADC_reading::voltage_reading,
        Sensors::battery_h11.total_global_voltage, HVBMS::current_sm_state);

    DataPackets::SOC_init(Batteries::SOC);

    DataPackets::SDC_init(Sensors::sdc.status);

    DataPackets::High_Voltage_Batteries_init(
        Sensors::battery_h11.battery[0].cells[0], Sensors::battery_h11.battery[0].cells[1],
        Sensors::battery_h11.battery[0].cells[2], Sensors::battery_h11.battery[0].cells[3],
        Sensors::battery_h11.battery[0].cells[4], Sensors::battery_h11.battery[0].cells[5],
        Sensors::battery_h11.battery[0].cells[6], Sensors::battery_h11.battery[0].cells[7],
        Sensors::battery_h11.battery[0].cells[8], Sensors::battery_h11.battery[0].cells[9],
        Sensors::battery_h11.battery[0].cells[10], Sensors::battery_h11.battery[0].cells[11],
        Sensors::battery_h11.battery[0].total_voltage, Sensors::battery_h11.battery[0].max_temp,
        Sensors::battery_h11.battery[0].min_temp, Sensors::battery_h11.battery[1].cells[0],
        Sensors::battery_h11.battery[1].cells[1], Sensors::battery_h11.battery[1].cells[2],
        Sensors::battery_h11.battery[1].cells[3], Sensors::battery_h11.battery[1].cells[4],
        Sensors::battery_h11.battery[1].cells[5], Sensors::battery_h11.battery[1].cells[6],
        Sensors::battery_h11.battery[1].cells[7], Sensors::battery_h11.battery[1].cells[8],
        Sensors::battery_h11.battery[1].cells[9], Sensors::battery_h11.battery[1].cells[10],
        Sensors::battery_h11.battery[1].cells[11], Sensors::battery_h11.battery[1].total_voltage,
        Sensors::battery_h11.battery[1].max_temp, Sensors::battery_h11.battery[1].min_temp,
        Sensors::battery_h11.battery[2].cells[0], Sensors::battery_h11.battery[2].cells[1],
        Sensors::battery_h11.battery[2].cells[2], Sensors::battery_h11.battery[2].cells[3],
        Sensors::battery_h11.battery[2].cells[4], Sensors::battery_h11.battery[2].cells[5],
        Sensors::battery_h11.battery[2].cells[6], Sensors::battery_h11.battery[2].cells[7],
        Sensors::battery_h11.battery[2].cells[8], Sensors::battery_h11.battery[2].cells[9],
        Sensors::battery_h11.battery[2].cells[10], Sensors::battery_h11.battery[2].cells[11],
        Sensors::battery_h11.battery[2].total_voltage, Sensors::battery_h11.battery[2].max_temp,
        Sensors::battery_h11.battery[2].min_temp, Sensors::battery_h11.battery[3].cells[0],
        Sensors::battery_h11.battery[3].cells[1], Sensors::battery_h11.battery[3].cells[2],
        Sensors::battery_h11.battery[3].cells[3], Sensors::battery_h11.battery[3].cells[4],
        Sensors::battery_h11.battery[3].cells[5], Sensors::battery_h11.battery[3].cells[6],
        Sensors::battery_h11.battery[3].cells[7], Sensors::battery_h11.battery[3].cells[8],
        Sensors::battery_h11.battery[3].cells[9], Sensors::battery_h11.battery[3].cells[10],
        Sensors::battery_h11.battery[3].cells[11], Sensors::battery_h11.battery[3].total_voltage,
        Sensors::battery_h11.battery[3].max_temp, Sensors::battery_h11.battery[3].min_temp,
        Sensors::battery_h11.battery[4].cells[0], Sensors::battery_h11.battery[4].cells[1],
        Sensors::battery_h11.battery[4].cells[2], Sensors::battery_h11.battery[4].cells[3],
        Sensors::battery_h11.battery[4].cells[4], Sensors::battery_h11.battery[4].cells[5],
        Sensors::battery_h11.battery[4].cells[6], Sensors::battery_h11.battery[4].cells[7],
        Sensors::battery_h11.battery[4].cells[8], Sensors::battery_h11.battery[4].cells[9],
        Sensors::battery_h11.battery[4].cells[10], Sensors::battery_h11.battery[4].cells[11],
        Sensors::battery_h11.battery[4].total_voltage, Sensors::battery_h11.battery[4].max_temp,
        Sensors::battery_h11.battery[4].min_temp, Sensors::battery_h11.battery[5].cells[0],
        Sensors::battery_h11.battery[5].cells[1], Sensors::battery_h11.battery[5].cells[2],
        Sensors::battery_h11.battery[5].cells[3], Sensors::battery_h11.battery[5].cells[4],
        Sensors::battery_h11.battery[5].cells[5], Sensors::battery_h11.battery[5].cells[6],
        Sensors::battery_h11.battery[5].cells[7], Sensors::battery_h11.battery[5].cells[8],
        Sensors::battery_h11.battery[5].cells[9], Sensors::battery_h11.battery[5].cells[10],
        Sensors::battery_h11.battery[5].cells[11], Sensors::battery_h11.battery[5].total_voltage,
        Sensors::battery_h11.battery[5].max_temp, Sensors::battery_h11.battery[5].min_temp,
        Sensors::battery_h11.battery[6].cells[0], Sensors::battery_h11.battery[6].cells[1],
        Sensors::battery_h11.battery[6].cells[2], Sensors::battery_h11.battery[6].cells[3],
        Sensors::battery_h11.battery[6].cells[4], Sensors::battery_h11.battery[6].cells[5],
        Sensors::battery_h11.battery[6].cells[6], Sensors::battery_h11.battery[6].cells[7],
        Sensors::battery_h11.battery[6].cells[8], Sensors::battery_h11.battery[6].cells[9],
        Sensors::battery_h11.battery[6].cells[10], Sensors::battery_h11.battery[6].cells[11],
        Sensors::battery_h11.battery[6].total_voltage, Sensors::battery_h11.battery[6].max_temp,
        Sensors::battery_h11.battery[6].min_temp, Sensors::battery_h11.battery[7].cells[0],
        Sensors::battery_h11.battery[7].cells[1], Sensors::battery_h11.battery[7].cells[2],
        Sensors::battery_h11.battery[7].cells[3], Sensors::battery_h11.battery[7].cells[4],
        Sensors::battery_h11.battery[7].cells[5], Sensors::battery_h11.battery[7].cells[6],
        Sensors::battery_h11.battery[7].cells[7], Sensors::battery_h11.battery[7].cells[8],
        Sensors::battery_h11.battery[7].cells[9], Sensors::battery_h11.battery[7].cells[10],
        Sensors::battery_h11.battery[7].cells[11], Sensors::battery_h11.battery[7].total_voltage,
        Sensors::battery_h11.battery[7].max_temp, Sensors::battery_h11.battery[7].min_temp);

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
}
