#pragma once

#include <limits>

#include "../../../deps/BCC_SW_Driver/bcc/bcc.h"
#include "HVBMS/BCC/bcc_stlib.hpp"
#include "HVBMS/Data/Data.hpp"
#include "ST-LIB.hpp"

#define BATTERIES_CONNECTED 0
#define H11_N_MODULES 8
#define H11_N_SEGMENTS 12
#define H11_N_HW_CELLS 14
#define H11_N_GPIO 4
#define H11_N_TEMPS 4
#define H11_CAPACITY_AH (4.2f * 3.0f)

struct BatteryData {
    float cells[H11_N_SEGMENTS]{};
    float total_voltage{};
    float conv_rate{};
    float max_temp{};
    float min_temp{};
    float min_voltage{};
    float max_voltage{};
};

struct Batteries {
    static inline bcc_drv_config_t bcc_config{};
    static inline BatteryData battery[H11_N_MODULES]{};
    static inline float temperature[H11_N_MODULES * H11_N_TEMPS]{};

    static inline float SOC{50.0f};
    static inline float current{};
    static inline float total_voltage{};
    static inline float min_temperature{};
    static inline float max_temperature{};
    static inline float max_total_voltage{};
    static inline float min_total_voltage{};
    static inline uint16_t faults[11]{};

    static inline uint32_t last_reading_time{};
    static inline int32_t period_ms{};
    static inline uint8_t read_module{};
    static inline uint8_t modules_read{};

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
            bcc_config.cellCnt[i] = H11_N_HW_CELLS;
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
        bcc_status_t status =
            BCC_Meas_StartConversionGlobal(&bcc_config, MC33771C_ADC_CFG_INIT_VALUE);
        if (status != BCC_STATUS_SUCCESS) {
            FAULT("Could not start BCC conversion: %s", get_bcc_error_str(status));
        }
    }

    static void read_cells() {
        uint32_t cell_voltages[H11_N_HW_CELLS];

        // Leer voltaje 
        bcc_status_t status =
            BCC_Meas_GetCellVoltages(&bcc_config, (bcc_cid_t)(read_module + 1), cell_voltages);
        if (status != BCC_STATUS_SUCCESS) {
            WARNING("Could not read module %u", (bcc_cid_t)(read_module + 1));
            return;
        }

        float device_voltage = 0.0f;
        uint8_t sw = 0;
        // Guardar voltaje menos de la 4 y la 5 (por diseño de hardware)
        for (uint8_t hw = 0; hw < H11_N_HW_CELLS; hw++) {
            if (hw == 4 || hw == 5) continue;
            battery[read_module].cells[sw] = static_cast<float>(cell_voltages[hw]) / 1000.0f;
            if (hw == H11_N_HW_CELLS - 1) battery[read_module].cells[sw] += 255;
            device_voltage += battery[read_module].cells[sw];
            sw++;
        }
        battery[read_module].total_voltage = device_voltage;

        float voltage_sum = 0.0f;
        float min_v = std::numeric_limits<float>::max();
        float max_v = std::numeric_limits<float>::lowest();
        
        // Calcular voltaje minimo de ese module
        for (uint8_t c = 0; c < H11_N_SEGMENTS; c++) {
            min_v = std::min(min_v, battery[read_module].cells[c]);
            max_v = std::max(max_v, battery[read_module].cells[c]);
        }
        battery[read_module].min_voltage = min_v;
        battery[read_module].max_voltage = max_v;

        // Calcular voltaje total de todos los modulos
        for (uint8_t m = 0; m < modules_read; m++) {
            voltage_sum += battery[m].total_voltage;
        }
        total_voltage = voltage_sum;
    }

    static void read_analog() {
        uint32_t an_voltages[H11_N_GPIO];

        bcc_status_t status =
            BCC_Meas_GetAnVoltages(&bcc_config, (bcc_cid_t)(read_module + 1), an_voltages);
        if (status != BCC_STATUS_SUCCESS) {
            return;
        }

        float mod_max = std::numeric_limits<float>::lowest();
        float mod_min = std::numeric_limits<float>::max();

        for (uint8_t gpio = 0; gpio < H11_N_TEMPS; gpio++) {
            float voltage = static_cast<float>(an_voltages[gpio]) / 1000.0f;
            float resistance = (voltage * 1000.0f) / (3.0f - voltage);
            float temp = (resistance - 100.0f) / (0.00385f * 100.0f);
            temperature[read_module * H11_N_TEMPS + gpio] = temp;
            mod_max = std::max(mod_max, temp);
            mod_min = std::min(mod_min, temp);
        }

        battery[read_module].max_temp = mod_max;
        battery[read_module].min_temp = mod_min;
    }

    static float ocv_battery_SOC() {
        float sum_voltage = 0;
        for (uint8_t i = 0; i < (uint8_t)bcc_config.devicesCnt; i++) {
            sum_voltage += battery[i].total_voltage;
        }

        float avg_voltage = sum_voltage / static_cast<float>(bcc_config.devicesCnt);
        float x = avg_voltage;
        float result = -62.5f + (14.9f * x) + (21.9f * x * x) + (-4.18f * x * x * x);
        if (result < 0.0f) result = 0.0f;
        if (result > 100.0f) result = 100.0f;
        return result;
    }

    static void update_SOC() { SOC = ocv_battery_SOC(); }

    static float &get_max_voltage() {
        max_total_voltage = std::numeric_limits<float>::min(); 
        for (uint8_t m = 0; m < modules_read; m++) {
            max_total_voltage = std::max(battery[m].max_voltage, max_total_voltage);
        }
        return max_total_voltage;
    }

    static float &get_min_voltage(){
        min_total_voltage = std::numeric_limits<float>::max();
        for (uint8_t m = 0; m < modules_read; m++) {
            min_total_voltage = std::min(battery[m].max_voltage, min_total_voltage);
        }
        return min_total_voltage;
    }
    static float get_total_voltage() { return total_voltage; }
    static float get_min_temperature() { return min_temperature; }
    static float get_max_temperature() { return max_temperature; }
    static float get_SOC() { return SOC; }

    static void read_current() {
        int32_t isense_uv;
        bcc_status_t status = BCC_Meas_GetIsenseVoltage(&bcc_config, (bcc_cid_t)1, &isense_uv);
        if (status == BCC_STATUS_SUCCESS) {
            current = static_cast<float>(isense_uv) / 1000.0f;
        }
    }

    static void read() {
        read_cells();
        // read_analog();
        // get_max_min_temperatures();
        // read_current();
        if (modules_read < bcc_config.devicesCnt) modules_read++;
        read_module = (read_module + 1) % bcc_config.devicesCnt;
    }

    static void stop_cell_balance() {
        constexpr uint16_t balance_timer = 0U;
        for (uint8_t cid = 1; cid <= bcc_config.devicesCnt; cid++) {
            bcc_status_t status = BCC_CB_Enable(&bcc_config, (bcc_cid_t)cid, false);
            if (status != BCC_STATUS_SUCCESS) {
                WARNING("Could not disable CB for device %u: %s", cid, get_bcc_error_str(status));
            }

            for (uint8_t hw = 0; hw < H11_N_HW_CELLS; hw++) {
                status =
                        BCC_CB_SetIndividual(&bcc_config, (bcc_cid_t)cid, hw, false, balance_timer);
                
                if (status != BCC_STATUS_SUCCESS) {
                    WARNING("Could not disble CB for device %u cell %u: %s", cid, hw,
                            get_bcc_error_str(status));
                }
            }
        }
    }

    static void start_cell_balance() {
        constexpr float balance_threshold = 0.01f;
        constexpr uint16_t balance_timer = 0U;

        for (uint8_t cid = 1; cid <= bcc_config.devicesCnt; cid++) {
            bcc_status_t status =
                BCC_CB_Enable(&bcc_config, (bcc_cid_t)cid, true);
            if (status != BCC_STATUS_SUCCESS) {
                WARNING("Could not enable CB for device %u: %s", cid,
                        get_bcc_error_str(status));
                continue;
            }

            for (uint8_t hw = 0; hw < H11_N_HW_CELLS; hw++) {
                if (hw == 4 || hw == 5) {
                    status = BCC_CB_SetIndividual(&bcc_config, (bcc_cid_t)cid, hw,
                                                  false, balance_timer);
                } else {
                    uint8_t sw = (hw < 4) ? hw : (hw - 2);
                    bool should_balance =
                        battery[cid - 1].cells[sw] > battery[cid - 1].min_voltage + balance_threshold;
                    status = BCC_CB_SetIndividual(&bcc_config, (bcc_cid_t)cid, hw,
                                                  should_balance, balance_timer);
                }
                if (status != BCC_STATUS_SUCCESS) {
                    WARNING("Could not set CB for device %u cell %u: %s", cid, hw,
                            get_bcc_error_str(status));
                }
            }

            Scheduler::set_timeout(300000000, stop_cell_balance);

            INFO("Cell balancing in module %d configured to %.3f V)", cid,
                     battery[cid - 1].min_voltage);
        }
    }

    static void get_max_min_temperatures() {
        float min_t = std::numeric_limits<float>::max();
        float max_t = std::numeric_limits<float>::lowest();
        for (uint16_t i = 0; i < modules_read * H11_N_TEMPS; i++) {
            min_t = std::min(min_t, temperature[i]);
            max_t = std::max(max_t, temperature[i]);
        }
        min_temperature = min_t;
        max_temperature = max_t;
    }
};