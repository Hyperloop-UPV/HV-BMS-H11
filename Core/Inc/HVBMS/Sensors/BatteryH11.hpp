#pragma once

#define USE_MC33771C
#include "../../../deps/BCC_SW_Driver/bcc/bcc.h"
#include "HVBMS/BCC/bcc_stlib.hpp"
#include "HVBMS/Data/Data.hpp"
#include "ST-LIB.hpp"

#define RESISTANCE_REFERENCE 1000.0  // Ohmios
#define VOLTAGE_REFERENCE 3.0        // V
#define R0 100.0                     // Ohmios+
#define TCR 0.00385
#define CAPACITY_AH 1000 // cambiar esto 

struct Batteries {
    static inline bcc_drv_config_t bcc_config{};

    struct BatteryData {
        float cells[12];
    };

    static inline float SOC{50.0f};

    static inline float current{};

    static inline float min_cell{};
    static inline float max_cell{};

    static inline float total_voltage{};

    static inline float min_temperature{};
    static inline float max_temperature{};

    static inline BatteryData battery[8]{};
    static inline float temperature[4]{};

    // NOTE: For coulomb counting SOC (do I need this or does the library give me it?)
    static inline uint32_t last_reading_time{};

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
        // TODO
        return BCC_STATUS_SUCCESS;
    }

    static void init() {
        bcc_config.drvInstance = 0U;
        bcc_config.commMode = BCC_MODE_TPL;
        bcc_config.devicesCnt = 8U;
        for (uint8_t i = 0; i < (uint8_t)bcc_config.devicesCnt; i++) {
            bcc_config.device[i] = BCC_DEVICE_MC33771C;
            bcc_config.cellCnt[i] = 12U;
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

    // static float coulomb_counting_SOC(float current) {
    //     uint32_t current_time = HAL_GetTick();

    //     float delta_time = (current_time - last_reading_time) / 1000.0f;
    //     last_reading_time = current_time;

    //     float delta_SOC = current * delta_time / CAPACITY_AH * 3600.0f;
    //     return delta_SOC;
    // }

    static float ocv_battery_SOC() {
        float total_voltage = 0;
        for (uint8_t i = 0; i < (uint8_t)bcc_config.devicesCnt; i++) {
            total_voltage += battery[i].cells[0] + battery[i].cells[1] + battery[i].cells[2] +
                             battery[i].cells[3] + battery[i].cells[4] + battery[i].cells[5] +
                             battery[i].cells[6] + battery[i].cells[7] + battery[i].cells[8] +
                             battery[i].cells[9] + battery[i].cells[10] + battery[i].cells[11];
        }

        // Tengo que dividir por las celdas?
        float x = total_voltage;  
        float result = -62.5 + (14.9 * x) + (21.9 * x * x) + (-4.18 * x * x * x);
        return result;
    }

    static void update_SOC() { SOC = ocv_battery_SOC(); }

    // static void get_max_min_cells() {
    //     float maximum = FLT_MIN;
    //     float minimum = FLT_MIN;
    //     for (unsigned int i = 0; i < ARRAY_LENGTH(LV_BMS::battery[0].cells); i++) {
    //         float v = LV_BMS::battery[0].cells[i];
    //         maximum = std::max(v, maximum);
    //         minimum = std::min(v, minimum);
    //     }

    //     max_cell = maximum;
    //     min_cell = minimum;
    // }

    // static void get_max_min_temperatures() {
    //     float maximum = FLT_MIN;
    //     float minimum = FLT_MIN;
    //     for (unsigned int i = 0; i < ARRAY_LENGTH(LV_BMS::temperature); i++) {
    //         float v = LV_BMS::temperature[i];
    //         maximum = std::max(v, maximum);
    //         minimum = std::min(v, minimum);
    //     }

    //     max_temperature = maximum;
    //     min_temperature = minimum;
    // }

    static void read() {
        //get_max_min_cells();
        update_SOC();
        //get_max_min_temperatures();
    }

    static void read_temperature(const float voltage, float* temperature) {
        auto resistance = (voltage * RESISTANCE_REFERENCE) / (VOLTAGE_REFERENCE - voltage);
        *temperature = (resistance - R0) / (TCR * R0);
    }
};