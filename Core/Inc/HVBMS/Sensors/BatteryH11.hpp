#pragma once

#include "ST-LIB.hpp"
#include "HVBMS/Data/Data.hpp"


#include "../../../deps/BCC_SW_Driver/bcc/bcc.h"

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

    bcc_status_t Init_BCC_Registers() {
        bcc_status_t status;

        for (uint8_t cid = 1; cid <= bcc_config.devicesCnt; cid++) {
            for (uint8_t i = 0; i < (uint8_t)ARRAY_LENGTH(bcc_init_regs); i++) {
                if (bcc_init_regs[i].value != bcc_init_regs[i].defaultVal) {
                    status = BCC_Reg_Write(&bcc_config, (bcc_cid_t)cid,
                                           bcc_init_regs[i].address, bcc_init_regs[i].value);
                    if (status != BCC_STATUS_SUCCESS) {
                        return status;
                    }
                }
            }
        }

        return BCC_STATUS_SUCCESS;
    }

    bcc_status_t Clear_BCC_FaultRegisters() {
        // TODO
        return BCC_STATUS_SUCCESS;
    }

    static void init(){
        bcc_config.drvInstance = 0U;
        bcc_config.commMode = BCC_MODE_TPL;
        bcc_config.devicesCnt = 8U;
        bcc_config.device[0] = BCC_DEVICE_MC33771C;
        bcc_config.cellCnt[0] = 12U;

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

    static float coulomb_counting_SOC(float current);
    static float ocv_battery_SOC();
    static void update_SOC();

    static void get_max_min_cells();
    static void get_max_min_temperatures();

    static void read();
    static void read_temperature(const float voltage, float* temperature);
};  
