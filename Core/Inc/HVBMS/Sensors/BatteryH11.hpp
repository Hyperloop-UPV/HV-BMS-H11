#pragma once

#include <limits>

#include "../../../deps/BCC_SW_Driver/bcc/bcc.h"
#include "HVBMS/BCC/bcc_stlib.hpp"
#include "HVBMS/Data/Data.hpp"
#include "ST-LIB.hpp"

#define BATTERIES_CONNECTED 1
#define H11_N_MODULES 8
#define H11_N_SEGMENTS 12
#define H11_N_HW_CELLS 14
#define H11_N_GPIO 4
#define H11_N_TEMPS 4

#define MAX_VOLTAGE 4.2  // V
#define MIN_VOLTAGE 2.5  // V
#define OCV_POINTS 2048  // 256 is for losers

#define CELLS_IN_PARALLEL 3
#define CELL_CAPACITY_AH 4.2f
#define SEGMENT_CAPACITY_AH (CELL_CAPACITY_AH * CELLS_IN_PARALLEL)  // 12.6 Ah per 3P segment

struct BatteryData {
    float cells[H11_N_SEGMENTS]{};
    float cell_soc[H11_N_SEGMENTS]{};
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
    static inline float coulomb_soc{50.0f};
    static inline bool soc_initialized{false};
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
        uint16_t an_raw[H11_N_GPIO];

        bcc_status_t status = BCC_Reg_Read(
            &bcc_config, (bcc_cid_t)(read_module + 1), MC33771C_MEAS_AN3_OFFSET, 4, an_raw);
        if (status != BCC_STATUS_SUCCESS) {
            return;
        }

        constexpr float A_coef = 1.028444e-3f;
        constexpr float B_coef = 2.392435e-4f;
        constexpr float C_coef = 1.562216e-7f;

        float mod_max = std::numeric_limits<float>::lowest();
        float mod_min = std::numeric_limits<float>::max();

        for (uint8_t gpio = 0; gpio < H11_N_TEMPS; gpio++) {
            float raw = static_cast<float>(an_raw[3 - gpio] & 0x7FFFU);
            float ratio = raw / 32768.0f;
            float r_ntc = 6800.0f * ratio / (1.0f - ratio);
            float ln_r = logf(r_ntc);
            float inv_t = A_coef + B_coef * ln_r + C_coef * ln_r * ln_r * ln_r;
            float temp = (1.0f / inv_t) - 273.15f;
            temperature[read_module * H11_N_TEMPS + gpio] = temp;
            mod_max = std::max(mod_max, temp);
            mod_min = std::min(mod_min, temp);
        }

        battery[read_module].max_temp = mod_max;
        battery[read_module].min_temp = mod_min;
    }

    static float& get_max_voltage() {
        max_total_voltage = std::numeric_limits<float>::min();
        for (uint8_t m = 0; m < modules_read; m++) {
            max_total_voltage = std::max(battery[m].max_voltage, max_total_voltage);
        }
        return max_total_voltage;
    }

    static float& get_min_voltage() {
        min_total_voltage = std::numeric_limits<float>::max();
        for (uint8_t m = 0; m < modules_read; m++) {
            min_total_voltage = std::min(battery[m].min_voltage, min_total_voltage);
        }
        return min_total_voltage;
    }

    static float& get_min_temp() { 
        min_temperature = std::numeric_limits<float>::max();
        for (uint16_t i = 0; i < modules_read * H11_N_TEMPS; i++) {
            min_temperature = std::min(min_temperature, temperature[i]);
        }
        return min_temperature;
     }

    static float& get_max_temp(){
        max_temperature = std::numeric_limits<float>::lowest();
        for (uint16_t i = 0; i < modules_read * H11_N_TEMPS; i++) {
            max_temperature = std::max(max_temperature, temperature[i]);
        }
        return max_temperature;
    }

    static void read_current() {
        int32_t isense_uv;
        bcc_status_t status = BCC_Meas_GetIsenseVoltage(&bcc_config, (bcc_cid_t)1, &isense_uv);
        if (status == BCC_STATUS_SUCCESS) {
            current = static_cast<float>(isense_uv) / 1000.0f;
        }
    }

    static void update_coulomb_counting() {
        uint32_t now = GetMicroseconds();
        if (last_reading_time == 0) {
            last_reading_time = now;
            return;
        }
        uint32_t elapsed = now - last_reading_time;
        last_reading_time = now;

        float dt = static_cast<float>(elapsed) / 1'000'000.0f;
        float pack_current = ADC_reading::current_reading;
        float delta_soc =
            (pack_current * dt * 100.0f) / (SEGMENT_CAPACITY_AH * 3600.0f);
        coulomb_soc -= delta_soc;
        if (coulomb_soc < 0.0f) coulomb_soc = 0.0f;
        if (coulomb_soc > 100.0f) coulomb_soc = 100.0f;
        SOC = coulomb_soc;
    }

    static void read_LUT(){
        for (uint8_t c = 0; c < H11_N_SEGMENTS; c++) {
            battery[read_module].cell_soc[c] = lookup_OCV(battery[read_module].cells[c] / 1000.0f);
        }
        coulomb_soc = compute_ocv_soc();
        SOC = coulomb_soc;
    }
    static void read() {
        read_cells();
        read_analog();

        if (modules_read < bcc_config.devicesCnt) {
            modules_read++;
        } else {
            if (!soc_initialized) {
                read_LUT();
                soc_initialized = true;
            } else {
                if (ADC_reading::current_reading > 0.2){
                    update_coulomb_counting();
                }
                else{
                    read_LUT();
                }
            }
        }
        read_module = (read_module + 1) % bcc_config.devicesCnt;
    }
    
    static void prueba_columb(){
        if (!soc_initialized) {
            for (uint8_t c = 0; c < H11_N_SEGMENTS; c++) {
                battery[read_module].cell_soc[c] =
                    lookup_OCV(battery[read_module].cells[c] / 1000.0f);
            }
            coulomb_soc = compute_ocv_soc();
            SOC = coulomb_soc;
            soc_initialized = true;
        } else {
            update_coulomb_counting();
        }
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
            bcc_status_t status = BCC_CB_Enable(&bcc_config, (bcc_cid_t)cid, true);
            if (status != BCC_STATUS_SUCCESS) {
                WARNING("Could not enable CB for device %u: %s", cid, get_bcc_error_str(status));
                continue;
            }

            for (uint8_t hw = 0; hw < H11_N_HW_CELLS; hw++) {
                if (hw == 4 || hw == 5) {
                    status =
                        BCC_CB_SetIndividual(&bcc_config, (bcc_cid_t)cid, hw, false, balance_timer);
                } else {
                    uint8_t sw = (hw < 4) ? hw : (hw - 2);
                    bool should_balance = battery[cid - 1].cells[sw] >
                                          battery[cid - 1].min_voltage + balance_threshold;
                    status = BCC_CB_SetIndividual(&bcc_config, (bcc_cid_t)cid, hw, should_balance,
                                                  balance_timer);
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

    template <size_t points>
    static constexpr array<float, points> calculate_OCV() {
        float A{2.0000857323f};
        float B{-26.585900707f};
        float C{128.754813f};
        float D{-271.11938173f};
        float E{214.69606092f};

        constexpr float total_capacity_ah = 4.2f;

        auto delta = (MAX_VOLTAGE - MIN_VOLTAGE) / (points - 1);
        array<float, points> result;
        for (size_t i{0}; i < points; ++i) {
            auto x = MIN_VOLTAGE + i * delta;
            auto missing_ah = (A * x * x * x * x) + (B * x * x * x) + (C * x * x) + (D * x) + E;
            auto soc = 100.0f * (1.0f - missing_ah / total_capacity_ah);
            soc = std::max(0.0, std::min(100.0, soc));  // clamp
            result[i] = soc;
        }

        return result;
    }

    static inline array<float, OCV_POINTS> ocv{calculate_OCV<OCV_POINTS>()};

    static float lookup_OCV(float voltage) {
        if (voltage <= MIN_VOLTAGE) return ocv.front();
        if (voltage >= MAX_VOLTAGE) return ocv.back();

        constexpr float delta = (MAX_VOLTAGE - MIN_VOLTAGE) / (OCV_POINTS - 1);
        size_t index = static_cast<size_t>((voltage - MIN_VOLTAGE) / delta);

        return ocv[index];
    }

    static float compute_ocv_soc() {
        if (modules_read == 0) return SOC;
        float sum_soc = 0;
        for (uint8_t m = 0; m < modules_read; m++) {
            for (uint8_t c = 0; c < H11_N_SEGMENTS; c++) {
                sum_soc += battery[m].cell_soc[c];
            }
        }
        return sum_soc / static_cast<float>(modules_read * H11_N_SEGMENTS);
    }
};