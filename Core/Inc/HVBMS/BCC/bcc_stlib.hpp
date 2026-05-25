#ifndef BCC_STLIB_H
#define BCC_STLIB_H

#include "HVBMS/Data/Data.hpp"
#include "ST-LIB.hpp"

inline bool bcc_exceeded_timeout = false;

typedef struct {
    const uint8_t address;
    const uint16_t defaultVal;
    const uint16_t value;
} bcc_init_reg_t;

#define MC33771C_INIT_CONF_REG_CNT 59U
extern bcc_init_reg_t bcc_init_regs[MC33771C_INIT_CONF_REG_CNT];

const char* get_bcc_error_str(bcc_status_t status) {
    switch (status) {
        case BCC_STATUS_SUCCESS:
            return "BCC Success";
        case BCC_STATUS_PARAM_RANGE:
            return "BCC Error: Param out of Range";
        case BCC_STATUS_SPI_FAIL:
            return "BCC Error: SPI Fail";
        case BCC_STATUS_COM_TIMEOUT:
            return "BCC Error: Communication Timeout";
        case BCC_STATUS_COM_ECHO:
            return "BCC Error: Received \"echo\" frame does not correspond to the sent frame";
        case BCC_STATUS_COM_CRC:
            return "BCC Error: Wrong CRC in the received SPI frame";
        case BCC_STATUS_COM_MSG_CNT:
            return "BCC Error: Received frame has a valid CRC but the message counter value does "
                   "not match to the expected one";
        case BCC_STATUS_COM_NULL:
            return "BCC Error: Invalid request from MCU or else check bcc.h BCC_STATUS_COM_NULL";
        case BCC_STATUS_DIAG_FAIL:
            return "BCC Error: You're not allowed to enter diagnostic mode";
        case BCC_STATUS_EEPROM_ERROR:
            return "BCC Error: An error occurred during communication to EEPROM";
        case BCC_STATUS_EEPROM_PRESENT:
            return "BCC Error: No EEPROM detected";
        case BCC_STATUS_DATA_RDY:
            return "BCC Error: A new sequence of conversions is currently running";
        case BCC_STATUS_TIMEOUT_START:
            return "BCC Error: Error reported in BCC_MCU_StartTimeout function";
    }
    return "BCC Error: Unknown";
}

void timeout_timer_callback(void* rawinfo) {
    (void)rawinfo;
    bcc_exceeded_timeout = true;
}

/*!
 * @brief Returns SCG system clock frequency.
 *
 * @return SCG system clock frequency.
 */
uint32_t BCC_MCU_GetSystemClockFreq(void) { return SystemCoreClock; }

/*!
 * @brief Waits for specified amount of seconds.
 *
 * @param delay Number of seconds to wait.
 */
void BCC_MCU_WaitSec(uint16_t delay) {
    uint32_t total = delay * 1000;
    uint32_t i = 0;
    for (; i < total; i += UINT16_MAX) {
        BCC_MCU_WaitMs(i);
    }
    BCC_MCU_WaitMs(i - total);
}

/*!
 * @brief Waits for specified amount of milliseconds.
 *
 * @param delay Number of milliseconds to wait.
 */
void BCC_MCU_WaitMs(uint16_t delay) {
    // NOTE: Assume the counter for the timer has started
    // NOTE: This also assumes the timer is counting in microseconds per CNT step
    BCC_MCU_Assert((GlobalTimer::global_us_timer->CR1 & TIM_CR1_CEN) != 0);
    BCC_MCU_WaitUs((uint32_t)delay * 1000UL);
}

/*!
 * @brief Waits for specified amount of microseconds.
 *
 * @param delay Number of microseconds to wait.
 */
void BCC_MCU_WaitUs(uint32_t delay) {
    // NOTE: Assume the counter for the timer has started
    // NOTE: This also assumes the timer is counting in microseconds per CNT step
    // BCC_MCU_Assert((global_us_timer->CR1 & TIM_CR1_CEN) != 0);
    uint32_t start = GlobalTimer::global_us_timer->CNT;
    uint32_t end = start + delay;
    if (start > end) [[unlikely]] {
        while (GlobalTimer::global_us_timer->CNT > end) /* wait */
            ;
    }
    while (GlobalTimer::global_us_timer->CNT < end) /* wait */
        ;
}

/*!
 * @brief Starts a non-blocking timeout mechanism. After expiration of the time
 * passed as a parameter, function BCC_MCU_TimeoutExpired should signalize an
 * expired timeout.
 *
 * @param timeoutUs Length of the timeout in microseconds.
 *
 * @return Returns BCC_STATUS_TIMEOUT_START in case of error, BCC_STATUS_SUCCESS
 *         otherwise.
 */
bcc_status_t BCC_MCU_StartTimeout(uint32_t timeoutUs) {
    bcc_exceeded_timeout = false;
    timeout_timer->CNT = 0;
    timeout_timer->ARR = timeoutUs;
    SET_BIT(timeout_timer->CR1, TIM_CR1_CEN);
    return BCC_STATUS_SUCCESS;
}

/*!
 * @brief Returns state of the timeout mechanism started by the function
 * BCC_MCU_StartTimeout.
 *
 * @return True if timeout expired, false otherwise.
 */
bool BCC_MCU_TimeoutExpired(void) { return bcc_exceeded_timeout; }

/*!
 * @brief This function performs one 48b transfer via SPI bus. Intended for SPI
 * mode only.
 *
 * The byte order of buffers is given by BCC_MSG_* macros (in bcc.h).
 *
 * @param drvInstance Instance of BCC driver.
 * @param txBuf       Pointer to TX data buffer (of BCC_MSG_SIZE size).
 * @param rxBuf       Pointer to RX data buffer (of BCC_MSG_SIZE size).
 *
 * @return bcc_status_t Error code.
 */
bcc_status_t BCC_MCU_TransferSpi(const uint8_t drvInstance, volatile uint8_t txBuf[],
                                 volatile uint8_t rxBuf[]) {
    return BCC_STATUS_TPL_FAIL;
}

// HVBMS does use TPL
bcc_status_t BCC_MCU_TransferTpl(const uint8_t drvInstance, volatile uint8_t txBuf[],
                                 volatile uint8_t rxBuf[], const uint16_t rxTrCnt) {
    // No entiendo muy bien estom, pero el chat esta convencido
    // Creo que es porque tiene 6 bytes cada mensaje?
    uint16_t total_rx_bytes = rxTrCnt * 6;

    // 2. Preparar el Esclavo (RX) antes que nada
    // Esto es lo más importante para no perder el Eco
    NewSPI::bms_wrapper_rx->receive_dma((uint8_t*)rxBuf, total_rx_bytes);

    // 3. Pequeño retardo de seguridad (microsegundos) para que el DMA esté listo
    BCC_MCU_WaitUs(5);

    // 4. Iniciar la transmisión del comando (Master)
    // BCC_MCU_WriteCsbPin ya habrá bajado el CS_TX
    bool tx_ok = NewSPI::bms_wrapper_tx->transmit_dma((uint8_t*)txBuf, 6);

    if (!tx_ok) return BCC_STATUS_SPI_FAIL;

    // 5. Esperar a que el Slave termine (Eco + Respuestas)
    uint32_t timeout = rxTrCnt * 1000;  // 1ms por trama es muy generoso
    while (NewSPI::bms_wrapper_rx->is_busy() && timeout > 0) {
        BCC_MCU_WaitUs(1);
        timeout--;
    }

    if (timeout == 0) return BCC_STATUS_COM_TIMEOUT;

    return BCC_STATUS_SUCCESS;
}

#define BCC_MCU_Assert(expr)                            \
    do {                                                \
        if (!(expr)) {                                  \
            FAULT("BCC assert fail: " stringify(expr)); \
        }                                               \
    } while (0)

/*!
 * @brief Writes logic 0 or 1 to the CSB (SPI mode) or CSB_TX pin (TPL mode).
 *
 * @param drvInstance Instance of BCC driver.
 * @param value       Zero or one to be set to CSB (CSB_TX) pin.
 */
void BCC_MCU_WriteCsbPin(const uint8_t drvInstance, const uint8_t value) {
    if (value) {
        NewSPI::cs_tx_pin->turn_on();
    } else {
        NewSPI::cs_tx_pin->turn_off();
    }
}

// NOTE: Unused since I don't have SPI
void BCC_MCU_WriteRstPin(const uint8_t drvInstance, const uint8_t value) {
    // I do not have a RST pin
}

void BCC_MCU_WriteEnPin(const uint8_t drvInstance, const uint8_t value) {
    if (value) {
        DO::spi_enable->turn_on();
    } else {
        DO::spi_enable->turn_off();
    }
}

uint32_t BCC_MCU_ReadIntbPin(const uint8_t drvInstance) {
    return (uint32_t)DI::battery_intb->read();
}

// CONFIGURACION MINIMA HECHA POR EL CHAT NO HE REVISADO NADA SOLO QUIERO QUE COMPILE

// Configuración de Sistema 1: Mediciones continuas, medida de corriente activada, equilibrado
// desactivado.
#define MC33771C_SYS_CFG1_INIT_VALUE                                                      \
    (MC33771C_SYS_CFG1_CYCLIC_TIMER(MC33771C_SYS_CFG1_CYCLIC_TIMER_CONTINUOUS_ENUM_VAL) | \
     MC33771C_SYS_CFG1_DIAG_TIMEOUT(MC33771C_SYS_CFG1_DIAG_TIMEOUT_1S_ENUM_VAL) |         \
     MC33771C_SYS_CFG1_I_MEAS_EN(MC33771C_SYS_CFG1_I_MEAS_EN_ENABLED_ENUM_VAL) |          \
     MC33771C_SYS_CFG1_CB_DRVEN(MC33771C_SYS_CFG1_CB_DRVEN_DISABLED_ENUM_VAL) |           \
     MC33771C_SYS_CFG1_GO2DIAG(MC33771C_SYS_CFG1_GO2DIAG_EXIT_ENUM_VAL) |                 \
     MC33771C_SYS_CFG1_SOFT_RST(MC33771C_SYS_CFG1_SOFT_RST_DISABLED_ENUM_VAL) |           \
     MC33771C_SYS_CFG1_WAVE_DC_BITX(MC33771C_SYS_CFG1_WAVE_DC_BITX_500US_ENUM_VAL))

// Configuración de Sistema 2: Timeout de com a 256ms para cadena larga (8 módulos)
#define MC33771C_SYS_CFG2_INIT_VALUE                                                 \
    (MC33771C_SYS_CFG2_TIMEOUT_COMM(MC33771C_SYS_CFG2_TIMEOUT_COMM_256MS_ENUM_VAL) | \
     MC33771C_SYS_CFG2_FLT_RST_CFG(MC33771C_SYS_CFG2_FLT_RST_CFG_OSC_MON_ENUM_VAL) | \
     MC33771C_SYS_CFG2_NUMB_ODD(MC33771C_SYS_CFG2_NUMB_ODD_EVEN_ENUM_VAL) |          \
     MC33771C_SYS_CFG2_HAMM_ENCOD(MC33771C_SYS_CFG2_HAMM_ENCOD_DECODE_ENUM_VAL))

// Configuración ADC: 16 bits de resolución, sin promediado (para rapidez inicial)
#define MC33771C_ADC_CFG_INIT_VALUE                                             \
    (MC33771C_ADC_CFG_AVG(MC33771C_ADC_CFG_AVG_NO_AVERAGING_ENUM_VAL) |         \
     MC33771C_ADC_CFG_ADC1_A_DEF(MC33771C_ADC_CFG_ADC1_A_DEF_16_BIT_ENUM_VAL) | \
     MC33771C_ADC_CFG_ADC1_B_DEF(MC33771C_ADC_CFG_ADC1_B_DEF_16_BIT_ENUM_VAL) | \
     MC33771C_ADC_CFG_ADC2_DEF(MC33771C_ADC_CFG_ADC2_DEF_16_BIT_ENUM_VAL) |     \
     MC33771C_ADC_CFG_PGA_GAIN(MC33771C_ADC_CFG_PGA_GAIN_AUTO_ENUM_VAL))

// Compensación de Offset (ADC2)
#define MC33771C_ADC2_OFFSET_COMP_INIT_VALUE                                                 \
    (MC33771C_ADC2_OFFSET_COMP_FREE_CNT(MC33771C_ADC2_OFFSET_COMP_FREE_CNT_CLAMP_ENUM_VAL) | \
     MC33771C_ADC2_OFFSET_COMP_ADC2_OFFSET_COMP(0))

// Habilitar solo 12 celdas (CT1 a CT12). CT13 y CT14 se desactivan (0x0FFF)
#define MC33771C_OV_UV_EN_INIT_VALUE                                                           \
    (0x0FFF | MC33771C_OV_UV_EN_COMMON_OV_TH(MC33771C_OV_UV_EN_COMMON_OV_TH_COMMON_ENUM_VAL) | \
     MC33771C_OV_UV_EN_COMMON_UV_TH(MC33771C_OV_UV_EN_COMMON_UV_TH_COMMON_ENUM_VAL))

// Umbrales por defecto (Valores POR: 2.5V UV / 4.2V OV)
#define MC33771C_TH_CTX_INIT_VALUE MC33771C_TH_ALL_CT_POR_VAL
#define MC33771C_TH_ANX_OT_INIT_VALUE MC33771C_TH_AN0_OT_POR_VAL
#define MC33771C_TH_ANX_UT_INIT_VALUE MC33771C_TH_AN0_UT_POR_VAL
#define MC33771C_TH_ISENSE_OC_INIT_VALUE MC33771C_TH_ISENSE_OC_POR_VAL
#define MC33771C_TH_COULOMB_CNT_MSB_INIT_VALUE MC33771C_TH_COULOMB_CNT_MSB_POR_VAL
#define MC33771C_TH_COULOMB_CNT_LSB_INIT_VALUE MC33771C_TH_COULOMB_CNT_LSB_POR_VAL
#define MC33771C_CBX_CFG_INIT_VALUE 0x0000

// 2. ARRAY DE REGISTROS DE INICIALIZACIÓN
// -------------------------------------------------------------------------
// Nota: BCC_INIT_REG es una macro que simplifica: {Dirección, Valor_Por_Defecto, Valor_A_Escribir}

#define BCC_INIT_REG(x) {MC33771C_##x##_OFFSET, MC33771C_##x##_POR_VAL, MC33771C_##x##_INIT_VALUE}

bcc_init_reg_t bcc_init_regs[MC33771C_INIT_CONF_REG_CNT] = {
    BCC_INIT_REG(SYS_CFG1),
    BCC_INIT_REG(SYS_CFG2),
    BCC_INIT_REG(ADC_CFG),
    BCC_INIT_REG(ADC2_OFFSET_COMP),
    BCC_INIT_REG(OV_UV_EN),
    BCC_INIT_REG(TH_ALL_CT),
    {MC33771C_GPIO_CFG1_OFFSET, MC33771C_GPIO_CFG1_POR_VAL, 0x0000},
    {MC33771C_GPIO_CFG2_OFFSET, MC33771C_GPIO_CFG2_POR_VAL, 0x0000},
    {MC33771C_FAULT_MASK1_OFFSET, MC33771C_FAULT_MASK1_POR_VAL, 0x0000},
    {MC33771C_FAULT_MASK2_OFFSET, MC33771C_FAULT_MASK2_POR_VAL, 0x0000},
    {MC33771C_FAULT_MASK3_OFFSET, MC33771C_FAULT_MASK3_POR_VAL, 0x0000},
    {MC33771C_WAKEUP_MASK1_OFFSET, MC33771C_WAKEUP_MASK1_POR_VAL, 0x0000},
    {MC33771C_WAKEUP_MASK2_OFFSET, MC33771C_WAKEUP_MASK2_POR_VAL, 0x0000},
    {MC33771C_WAKEUP_MASK3_OFFSET, MC33771C_WAKEUP_MASK3_POR_VAL, 0x0000},
    {MC33771C_TH_ISENSE_OC_OFFSET, MC33771C_TH_ISENSE_OC_POR_VAL, MC33771C_TH_ISENSE_OC_INIT_VALUE},
    {MC33771C_TH_COULOMB_CNT_MSB_OFFSET, MC33771C_TH_COULOMB_CNT_MSB_POR_VAL,
     MC33771C_TH_COULOMB_CNT_MSB_INIT_VALUE},
    {MC33771C_TH_COULOMB_CNT_LSB_OFFSET, MC33771C_TH_COULOMB_CNT_LSB_POR_VAL,
     MC33771C_TH_COULOMB_CNT_LSB_INIT_VALUE},
    // Umbrales de Celdas (CT1-CT14)
    {0x4C, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x4D, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x4E, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x4F, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x50, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x51, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x52, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x53, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x54, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x55, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x56, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x57, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x58, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    {0x59, 0xD780, MC33771C_TH_CTX_INIT_VALUE},
    // Temperaturas (AN0-AN6) OT/UT
    {0x5A, 0x00ED, MC33771C_TH_ANX_OT_INIT_VALUE},
    {0x5B, 0x00ED, MC33771C_TH_ANX_OT_INIT_VALUE},
    {0x5C, 0x00ED, MC33771C_TH_ANX_OT_INIT_VALUE},
    {0x5D, 0x00ED, MC33771C_TH_ANX_OT_INIT_VALUE},
    {0x5E, 0x00ED, MC33771C_TH_ANX_OT_INIT_VALUE},
    {0x5F, 0x00ED, MC33771C_TH_ANX_OT_INIT_VALUE},
    {0x60, 0x00ED, MC33771C_TH_ANX_OT_INIT_VALUE},
    {0x61, 0x030E, MC33771C_TH_ANX_UT_INIT_VALUE},
    {0x62, 0x030E, MC33771C_TH_ANX_UT_INIT_VALUE},
    {0x63, 0x030E, MC33771C_TH_ANX_UT_INIT_VALUE},
    {0x64, 0x030E, MC33771C_TH_ANX_UT_INIT_VALUE},
    {0x65, 0x030E, MC33771C_TH_ANX_UT_INIT_VALUE},
    {0x66, 0x030E, MC33771C_TH_ANX_UT_INIT_VALUE},
    {0x67, 0x030E, MC33771C_TH_ANX_UT_INIT_VALUE},
    // Configuración de Balanceo (CB1-CB14) - Todo desactivado por ahora
    {0x0C, 0, 0},
    {0x0D, 0, 0},
    {0x0E, 0, 0},
    {0x0F, 0, 0},
    {0x10, 0, 0},
    {0x11, 0, 0},
    {0x12, 0, 0},
    {0x13, 0, 0},
    {0x14, 0, 0},
    {0x15, 0, 0},
    {0x16, 0, 0},
    {0x17, 0, 0},
    {0x18, 0, 0},
    {0x19, 0, 0}};
#endif