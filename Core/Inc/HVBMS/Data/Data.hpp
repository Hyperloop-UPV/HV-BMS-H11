#pragma once
#include "Communications/Packets/DataPackets.hpp"
#include "ST-LIB.hpp"

using ST_LIB::DigitalOutputDomain;

constexpr DigitalOutputDomain::DigitalOutput led_PG9{ST_LIB::PG9};    // operational
constexpr DigitalOutputDomain::DigitalOutput led_PG13{ST_LIB::PG13};  // fault
// de leds me faltan sleep, flash, can y protection

constexpr DigitalOutputDomain::DigitalOutput contactor_PD8{ST_LIB::PD8};
constexpr DigitalOutputDomain::DigitalOutput contactor_PD9{ST_LIB::PD9};
constexpr DigitalOutputDomain::DigitalOutput contactor_PD10{ST_LIB::PD10};
constexpr DigitalOutputDomain::DigitalOutput contactor_PB14{ST_LIB::PB14};
constexpr DigitalOutputDomain::DigitalOutput contactor_PB15{ST_LIB::PB15};

constexpr DigitalOutputDomain::DigitalOutput sdc_PB4{ST_LIB::PB4};

constexpr DigitalOutputDomain::DigitalOutput cs_tx_PE4{ST_LIB::PE4};
constexpr DigitalOutputDomain::DigitalOutput spi_enable_PE3{ST_LIB::PE3};

constexpr DigitalOutputDomain::DigitalOutput imd_enable_PE11{ST_LIB::PE11};

namespace DO {
inline DigitalOutputDomain::Instance* operational_led;
inline DigitalOutputDomain::Instance* fault_led;
inline DigitalOutputDomain::Instance* contactor_low;
inline DigitalOutputDomain::Instance* contactor_high;
inline DigitalOutputDomain::Instance* contactor_precharge;
inline DigitalOutputDomain::Instance* contactor_discharge;
inline DigitalOutputDomain::Instance* contactor_common_high;
inline DigitalOutputDomain::Instance* sdc_fw_fault;
inline DigitalOutputDomain::Instance* cs_tx;
inline DigitalOutputDomain::Instance* imd_enable;
inline DigitalOutputDomain::Instance* spi_enable;
};  // namespace DO

using ST_LIB::DigitalInputDomain;

constexpr DigitalInputDomain::DigitalInput aux_contactor_PG2{ST_LIB::PG2};
constexpr DigitalInputDomain::DigitalInput aux_contactor_PD12{ST_LIB::PD12};
constexpr DigitalInputDomain::DigitalInput aux_contactor_PD13{ST_LIB::PD13};
constexpr DigitalInputDomain::DigitalInput aux_contactor_PD14{ST_LIB::PD14};
constexpr DigitalInputDomain::DigitalInput aux_contactor_PD15{ST_LIB::PD15};

constexpr DigitalInputDomain::DigitalInput battery_intb_PE1{ST_LIB::PE1};

namespace DI {
inline DigitalInputDomain::Instance* aux_contactor_low;
inline DigitalInputDomain::Instance* aux_contactor_high;
inline DigitalInputDomain::Instance* aux_contactor_precharge;
inline DigitalInputDomain::Instance* aux_contactor_discharge;
inline DigitalInputDomain::Instance* aux_contactor_common_high;
inline DigitalInputDomain::Instance* battery_intb;
}  // namespace DI

using ST_LIB::ADCDomain;

static float voltage_reading_ch1_raw;
static float current_reading_raw;

inline constexpr ADCDomain::ADC adc_PA3{ST_LIB::PA3, voltage_reading_ch1_raw};
inline constexpr ADCDomain::ADC adc_PA5{ST_LIB::PA5, current_reading_raw};

namespace ADC_reading {
inline float voltage_reading{0.0f};
inline float current_reading{0.0f};
}  // namespace ADC_reading

inline constexpr auto dc_voltage_protection =
    Protections::protection<"dc_bus_voltage", ADC_reading::voltage_reading>(
        Protections::Rules::above(420.0f));

inline constexpr auto dc_current_protection =
    Protections::protection<"dc_bus_current", ADC_reading::current_reading>(
        Protections::Rules::above(120.0f));

namespace ADC {
inline ADCDomain::Instance* adc_voltage_ch1;
inline ADCDomain::Instance* adc_current;
};  // namespace ADC

using ST_LIB::TimerAF;
using ST_LIB::TimerChannel;
using ST_LIB::TimerDomain;
using ST_LIB::TimerPin;
using ST_LIB::TimerRequest;
using ST_LIB::TimerWrapper;

constexpr TimerDomain::Timer timer_us_tick_def{{.request = TimerRequest::GeneralPurpose32bit_5}};

constexpr TimerDomain::Timer timeout_timer_def{{.request = TimerRequest::GeneralPurpose32bit_23}};

namespace GlobalTimer {
constexpr TimerPin ic_pin = {
    .af = TimerAF::InputCapture, .pin = ST_LIB::PA6, .channel = TimerChannel::CHANNEL_1};
};

inline constexpr TimerDomain::Timer timer_imd{{.request = TimerRequest::GeneralPurpose_3},
                                              GlobalTimer::ic_pin};

namespace GlobalTimer {
// inline TimerWrapper<timer_us_tick_def> global_us_timer;
inline TIM_TypeDef* global_us_timer;
inline TIM_TypeDef* timeout_timer;
inline TimerWrapper<timer_imd> input_timer;
};  // namespace GlobalTimer

#define GetMicroseconds() GlobalTimer::global_us_timer->CNT

using ST_LIB::DMA_Domain;
using ST_LIB::SPIDomain;

consteval SPIDomain::SPIConfig get_tx_config() {
    SPIDomain::SPIConfig c{
        SPIDomain::ClockPolarity::LOW, SPIDomain::ClockPhase::SECOND_EDGE,
        SPIDomain::BitOrder::MSB_FIRST,
        SPIDomain::NSSMode::SOFTWARE  // Manejamos CS manualmente
    };
    c.data_size = ST_LIB::SPIDomain::DataSize::SIZE_8BIT;
    return c;
}

// Configuración para el lado de RECEPCIÓN (Slave)
consteval SPIDomain::SPIConfig get_rx_config() {
    SPIDomain::SPIConfig c{
        SPIDomain::ClockPolarity::LOW, SPIDomain::ClockPhase::SECOND_EDGE,
        SPIDomain::BitOrder::MSB_FIRST,
        SPIDomain::NSSMode::HARDWARE  // Este CS lo maneja el MC33664
    };
    c.data_size = ST_LIB::SPIDomain::DataSize::SIZE_8BIT;
    c.nss_polarity = ST_LIB::SPIDomain::NSSPolarity::ACTIVE_LOW;
    return c;
}

inline constexpr auto bms_spi_tx =
    SPIDomain::Device<DMA_Domain::Stream::dma2_stream0, DMA_Domain::Stream::dma2_stream1>(
        SPIDomain::SPIMode::MASTER, SPIDomain::SPIPeripheral::spi4, 2000000,
        ST_LIB::PE2,  // SCLK_TX
        ST_LIB::PE5,  // ESTE PIN NO ESTA EN USO!
        ST_LIB::PE6,  // DATA_TX
        get_tx_config());

inline constexpr auto bms_spi_rx =
    SPIDomain::Device<DMA_Domain::Stream::dma1_stream5, DMA_Domain::Stream::dma1_stream6>(
        SPIDomain::SPIMode::SLAVE, SPIDomain::SPIPeripheral::spi5, 2000000,
        ST_LIB::PF7,  // SCLK_RX
        ST_LIB::PF8,  // ESTE PIN NO ESTA EN USO!
        ST_LIB::PF9,  // DATA_RX
        ST_LIB::PF6,  // El CS pin
        get_rx_config());

namespace NewSPI {
inline std::optional<SPIDomain::SPIWrapper<bms_spi_tx>> bms_wrapper_tx;
inline std::optional<SPIDomain::SPIWrapper<bms_spi_rx>> bms_wrapper_rx;
}  // namespace NewSPI

// Tasks and timeouts id
inline uint16_t id_timeout_precharge;
inline uint16_t id_check_precharge;
