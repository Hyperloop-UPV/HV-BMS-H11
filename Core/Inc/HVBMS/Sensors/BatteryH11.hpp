#pragma once

#include "../../../deps/BCC_SW_Driver/bcc/bcc.h"
#include "HVBMS/BCC/bcc_stlib.hpp"
#include "HVBMS/Data/Data.hpp"
#include "ST-LIB.hpp"

#define H11_N_MODULES 1
#define H11_N_SEGMENTS 12 // Each segment has 3 cells
#define H11_N_GPIO 4
#define H11_N_TEMPS 2
// Tengo que revisar estos parametros
#define H11_CAPACITY_AH 4.2f * 3.0f // Como esta en paralelo, tengo que multiplicar por 3
#define H11_MIN_VOLTAGE 22.0f
#define H11_MAX_VOLTAGE 25.0f

struct BatteryData {
    float cells[H11_N_SEGMENTS]{};
    float total_voltage{};
    float conv_rate{};
};

struct Batteries {
    static inline bcc_drv_config_t bcc_config{};
    static inline BatteryData battery[H11_N_MODULES]{};
    static inline float temperature[H11_N_MODULES * H11_N_TEMPS]{};

    static inline float SOC{50.0f};
    static inline float current{};
    static inline float min_cell{};
    static inline float max_cell{};
    static inline float total_voltage{};
    static inline float min_temperature{};
    static inline float max_temperature{};

    static inline uint32_t last_reading_time{};
    static inline int32_t period_ms{};

    static bcc_status_t Init_BCC_Registers() {
        bcc_status_t status;

        for (uint8_t cid = 1; cid <= bcc_config.devicesCnt; cid++) {
            for (uint8_t i = 0; i < (uint8_t)ARRAY_LENGTH(bcc_init_regs); i++) {
                if (bcc_init_regs[i].value != bcc_init_regs[i].defaultVal) {
                    status = BCC_Reg_Write(&bcc_config, (bcc_cid_t)cid, bcc_init_regs[i].address,
                                           bcc_init_regs[i].value);
                    if (status != BCC_STATUS_SUCCESS) {
                        return status;
                    }
                }
            }
        }

        return BCC_STATUS_SUCCESS;
    }

    static bcc_status_t Clear_BCC_FaultRegisters() {
        bcc_status_t status;

        for (uint8_t cid = 1; cid <= bcc_config.devicesCnt; cid++) {
            status = BCC_Fault_ClearStatus(&bcc_config, (bcc_cid_t)cid, BCC_FS_CELL_OV);
            if (status != BCC_STATUS_SUCCESS) {
                return status;
            }

            status = BCC_Fault_ClearStatus(&bcc_config, (bcc_cid_t)cid, BCC_FS_CELL_UV);
            if (status != BCC_STATUS_SUCCESS) {
                return status;
            }

            status = BCC_Fault_ClearStatus(&bcc_config, (bcc_cid_t)cid, BCC_FS_CB_OPEN);
            if (status != BCC_STATUS_SUCCESS) {
                return status;
            }

            status = BCC_Fault_ClearStatus(&bcc_config, (bcc_cid_t)cid, BCC_FS_CB_SHORT);
            if (status != BCC_STATUS_SUCCESS) {
                return status;
            }

            status = BCC_Fault_ClearStatus(&bcc_config, (bcc_cid_t)cid, BCC_FS_GPIO_STATUS);
            if (status != BCC_STATUS_SUCCESS) {
                return status;
            }

            status = BCC_Fault_ClearStatus(&bcc_config, (bcc_cid_t)cid, BCC_FS_AN_OT_UT);
            if (status != BCC_STATUS_SUCCESS) {
                return status;
            }

            status = BCC_Fault_ClearStatus(&bcc_config, (bcc_cid_t)cid, BCC_FS_GPIO_SHORT);
            if (status != BCC_STATUS_SUCCESS) {
                return status;
            }

            status = BCC_Fault_ClearStatus(&bcc_config, (bcc_cid_t)cid, BCC_FS_COMM);
            if (status != BCC_STATUS_SUCCESS) {
                return status;
            }

            status = BCC_Fault_ClearStatus(&bcc_config, (bcc_cid_t)cid, BCC_FS_FAULT1);
            if (status != BCC_STATUS_SUCCESS) {
                return status;
            }

            status = BCC_Fault_ClearStatus(&bcc_config, (bcc_cid_t)cid, BCC_FS_FAULT2);
            if (status != BCC_STATUS_SUCCESS) {
                return status;
            }

            status = BCC_Fault_ClearStatus(&bcc_config, (bcc_cid_t)cid, BCC_FS_FAULT3);
            if (status != BCC_STATUS_SUCCESS) {
                return status;
            }
        }

        return BCC_STATUS_SUCCESS;
    }

    static void init() {
        bcc_config.drvInstance = 0U;
        bcc_config.commMode = BCC_MODE_TPL;
        bcc_config.devicesCnt = H11_N_MODULES;
        for (uint8_t i = 0; i < (uint8_t)bcc_config.devicesCnt; i++) {
            bcc_config.device[i] = BCC_DEVICE_MC33771C;
            bcc_config.cellCnt[i] = H11_N_SEGMENTS;
        }

        bcc_status_t status = BCC_Init(&bcc_config);
        if (status != BCC_STATUS_SUCCESS) {
            FAULT("Could not init BCC: %s", get_bcc_error_str(status));
            return;
        }

        status = Init_BCC_Registers();
        if (status != BCC_STATUS_SUCCESS) {
            FAULT("Could not init BCC registers: %s", get_bcc_error_str(status));
            return;
        }

        status = Clear_BCC_FaultRegisters();
        if (status != BCC_STATUS_SUCCESS) {
            FAULT("Could not clear BCC fault registers: %s", get_bcc_error_str(status));
            return;
        }
    }

    static void start() {
        bcc_status_t status = BCC_Meas_StartConversionGlobal(&bcc_config,
                                                              MC33771C_ADC_CFG_INIT_VALUE);
        if (status != BCC_STATUS_SUCCESS) {
            FAULT("Could not start BCC conversion: %s", get_bcc_error_str(status));
        }
    }

    static void read_cells() {
        uint32_t cell_voltages[H11_N_SEGMENTS];
        float voltage_sum = 0.0f;
        float min_v = std::numeric_limits<float>::max();
        float max_v = std::numeric_limits<float>::lowest();

        for (uint8_t cid = 1; cid <= bcc_config.devicesCnt; cid++) {
            bcc_status_t status =
                BCC_Meas_GetCellVoltages(&bcc_config, (bcc_cid_t)cid, cell_voltages);
            if (status != BCC_STATUS_SUCCESS) {
                continue;
            }

            float device_voltage = 0.0f;
            for (uint8_t cell = 0; cell < H11_N_SEGMENTS; cell++) {
                battery[cid - 1].cells[cell] =
                    static_cast<float>(cell_voltages[cell]) / 1000.0f;
                device_voltage += battery[cid - 1].cells[cell];
                min_v = std::min(min_v, battery[cid - 1].cells[cell]);
                max_v = std::max(max_v, battery[cid - 1].cells[cell]);
            }
            battery[cid - 1].total_voltage = device_voltage;
            voltage_sum += device_voltage;
        }

        total_voltage = voltage_sum;
        min_cell = min_v;   
        max_cell = max_v;
    }

    static void read_analog() {
        uint32_t an_voltages[H11_N_GPIO];

        for (uint8_t cid = 1; cid <= bcc_config.devicesCnt; cid++) {
            bcc_status_t status =
                BCC_Meas_GetAnVoltages(&bcc_config, (bcc_cid_t)cid, an_voltages);
            if (status != BCC_STATUS_SUCCESS) {
                continue;
            }

            for (uint8_t gpio = 0; gpio < H11_N_TEMPS; gpio++) {
                float voltage = static_cast<float>(an_voltages[gpio]) / 1000.0f;
                float resistance = (voltage * 1000.0f) / (3.0f - voltage);
                temperature[(cid - 1) * H11_N_TEMPS + gpio] =
                    (resistance - 100.0f) / (0.00385f * 100.0f);
            }
        }
    }

    static float ocv_battery_SOC() {
        float sum_voltage = 0;
        for (uint8_t i = 0; i < (uint8_t)bcc_config.devicesCnt; i++) {
            sum_voltage += battery[i].total_voltage;
        }

        float avg_voltage = sum_voltage / static_cast<float>(bcc_config.devicesCnt);
        float x = avg_voltage;
        // Esta formula es mentira!!! Hay que hacer una curva de OCV real y meterla aqui
        float result = -62.5f + (14.9f * x) + (21.9f * x * x) + (-4.18f * x * x * x);
        if (result < 0.0f) result = 0.0f;
        if (result > 100.0f) result = 100.0f;
        return result;
    }

    static void update_SOC() { SOC = ocv_battery_SOC(); }

    static float get_min_cell() { return min_cell; }
    static float get_max_cell() { return max_cell; }
    static float get_total_voltage() { return total_voltage; }
    static float get_min_temperature() { return min_temperature; }
    static float get_max_temperature() { return max_temperature; }
    static float get_SOC() { return SOC; }

    static void read_current() {
        int32_t isense_uv;
        bcc_status_t status =
            BCC_Meas_GetIsenseVoltage(&bcc_config, (bcc_cid_t)1, &isense_uv);
        if (status == BCC_STATUS_SUCCESS) {
            current = static_cast<float>(isense_uv) / 1000.0f;
        }
    }

    static void read() {
        bcc_status_t status =
            BCC_Meas_StartAndWait(&bcc_config, (bcc_cid_t)1, BCC_AVG_8);
        if (status != BCC_STATUS_SUCCESS) {
            FAULT("Batteries could not be read");
            return;
        }

        read_cells();
        read_analog();
        get_max_min_temperatures();
        update_SOC();
    }

    static void get_max_min_temperatures() {
        float min_t = std::numeric_limits<float>::max();
        float max_t = std::numeric_limits<float>::lowest();
        for (uint16_t i = 0; i < bcc_config.devicesCnt * H11_N_TEMPS; i++) {
            min_t = std::min(min_t, temperature[i]);
            max_t = std::max(max_t, temperature[i]);
        } 
        min_temperature = min_t;
        max_temperature = max_t;
    }
};