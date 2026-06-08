#pragma once

#include "../../../deps/BCC_SW_Driver/bcc/bcc.h"
#include "HVBMS/BCC/bcc_stlib.hpp"
#include "HVBMS/Data/Data.hpp"
#include "ST-LIB.hpp"

#define BATTERIES_CONNECTED 1
#define H11_N_MODULES 1
#define H11_N_SEGMENTS 14// Each segment has 3 cells
#define H11_N_GPIO 4
#define H11_N_TEMPS 2
// Tengo que revisar estos parametros
#define H11_CAPACITY_AH 4.2f * 3.0f // Como esta en paralelo, tengo que multiplicar por 3

struct BatteryData {
    float cells[H11_N_SEGMENTS]{};
    float total_voltage{};
    float max_temp{};
    float min_temp{};
    int32_t coulomb_counter{};
    uint16_t analog_input[H11_N_GPIO]{};
    float ic_temperature{};
    int32_t adcia_voltage{};
    int32_t adcib_voltage{};
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
    static inline uint16_t faults{};
    

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

        uint64_t guid;
        status = BCC_GUID_Read(&bcc_config, (bcc_cid_t)1, &guid);
        if (status != BCC_STATUS_SUCCESS) {
            WARNING("Could not read device guid: %s", get_bcc_error_str(status));
            return;
        } else {
            INFO("BCC device guid: %02X%04X%04X", (uint16_t)((guid >> 32) & 0x001FU),
                 (uint16_t)((guid >> 16) & 0xFFFFU), (uint16_t)(guid & 0xFFFFU));
        }
    }


    static void start() {
        bcc_status_t status = BCC_Meas_StartConversionGlobal(&bcc_config,
                                                              MC33771C_ADC_CFG_INIT_VALUE);
        if (status != BCC_STATUS_SUCCESS) {
            FAULT("Could not start BCC conversion: %s", get_bcc_error_str(status));
        }

    }


    static void bcc_start_measurements(bcc_cid_t cid) {
        bcc_status_t status =
            BCC_Meas_StartConversion(&bcc_config, cid, BCC_AVG_8);
        if (status != BCC_STATUS_SUCCESS) {
            WARNING("Could not start BCC conversion on CID %u: %s", cid,
                    get_bcc_error_str(status));
        }
    }

    static void bcc_get_measurements() {
        static uint8_t current_cid = 1;
        const uint8_t idx = current_cid - 1;

        bool completed = false;
        bcc_status_t status =
            BCC_Meas_IsConverting(&bcc_config, (bcc_cid_t)current_cid, &completed);
        if (!completed) {
            return;
        }

        uint16_t measurements[BCC_MEAS_CNT];
        status = BCC_Meas_GetRawValues(&bcc_config, (bcc_cid_t)current_cid, measurements);
        if (status != BCC_STATUS_SUCCESS) {
            WARNING("Could not get bcc measurements for CID %u: %s", current_cid,
                    get_bcc_error_str(status));
            bcc_start_measurements((bcc_cid_t)current_cid);
            current_cid = (current_cid % (uint8_t)bcc_config.devicesCnt) + 1;
            return;
        }

        float device_voltage = 0.0f;
        for (uint8_t cell = 0; cell < H11_N_SEGMENTS; cell++) {
            if (cell == 4 || cell == 5) {
                battery[idx].cells[cell] = 0.0f;
                continue;
            }
            battery[idx].cells[cell] =
                static_cast<float>(
                    BCC_GET_VOLT(measurements[BCC_MSR_CELL_VOLT1 + cell])) /
                1000000.0f;
            device_voltage += battery[idx].cells[cell];
        }
        battery[idx].total_voltage = device_voltage;

        battery[idx].coulomb_counter = BCC_GET_COULOMB_CNT(
            measurements[BCC_MSR_COULOMB_CNT1], measurements[BCC_MSR_COULOMB_CNT2]);

        for (uint8_t gpio = 0; gpio < H11_N_GPIO; gpio++) {
            battery[idx].analog_input[gpio] = measurements[BCC_MSR_AN0 + gpio];
        }

        battery[idx].ic_temperature =
            static_cast<float>(BCC_GET_IC_TEMP_C(measurements[BCC_MSR_ICTEMP])) /
            10.0f;

        battery[idx].adcia_voltage =
            BCC_GET_VOLT(measurements[BCC_MSR_VBGADC1A]);
        battery[idx].adcib_voltage =
            BCC_GET_VOLT(measurements[BCC_MSR_VBGADC1B]);

        bcc_start_measurements((bcc_cid_t)current_cid);
        current_cid = (current_cid % (uint8_t)bcc_config.devicesCnt) + 1;
    }

    static void read_analog() {
        for (uint8_t cid = 0; cid < bcc_config.devicesCnt; cid++) {
            float mod_max = std::numeric_limits<float>::lowest();
            float mod_min = std::numeric_limits<float>::max();
            for (uint8_t gpio = 0; gpio < H11_N_TEMPS; gpio++) {
                float voltage =
                    static_cast<float>(battery[cid].analog_input[gpio]) * 5.0f /
                    65535.0f;
                float resistance =
                    (voltage * RESISTANCE_REFERENCE) / (VOLTAGE_REFERENCE - voltage);
                float temp = (resistance - R0) / (TCR * R0);
                temperature[cid * H11_N_TEMPS + gpio] = temp;
                mod_max = std::max(mod_max, temp);
                mod_min = std::min(mod_min, temp);
            }
            battery[cid].max_temp = mod_max;
            battery[cid].min_temp = mod_min;
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

    static void read() {
        bcc_get_measurements();

        float min_v = std::numeric_limits<float>::max();
        float max_v = std::numeric_limits<float>::lowest();
        float voltage_sum = 0.0f;

        for (uint8_t i = 0; i < bcc_config.devicesCnt; i++) {
            voltage_sum += battery[i].total_voltage;
            for (uint8_t cell = 0; cell < H11_N_SEGMENTS; cell++) {
                if (cell == 4 || cell == 5) continue;
                float v = battery[i].cells[cell];
                min_v = std::min(min_v, v);
                max_v = std::max(max_v, v);
            }
        }

        total_voltage = voltage_sum;
        min_cell = min_v;
        max_cell = max_v;

        read_analog();
        get_max_min_temperatures();
        update_SOC();

        period_ms = static_cast<int32_t>(HAL_GetTick() - last_reading_time);
        last_reading_time = HAL_GetTick();
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