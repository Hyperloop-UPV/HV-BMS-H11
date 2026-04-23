#pragma once
#include "Communications/Packets/DataPackets.hpp"
#include "ST-LIB.hpp"

using ST_LIB::DigitalOutputDomain;

constexpr DigitalOutputDomain::DigitalOutput led_PG9{ST_LIB::PG9}; // operational
constexpr DigitalOutputDomain::DigitalOutput led_PG13{ST_LIB::PG13}; // fault
// de leds me faltan sleep, flash, can y protection

constexpr DigitalOutputDomain::DigitalOutput contactor_PD8{ST_LIB::PD8};
constexpr DigitalOutputDomain::DigitalOutput contactor_PD9{ST_LIB::PD9};
constexpr DigitalOutputDomain::DigitalOutput contactor_PD10{ST_LIB::PD10};
constexpr DigitalOutputDomain::DigitalOutput contactor_PB14{ST_LIB::PB14};


constexpr DigitalOutputDomain::DigitalOutput sdc_PB4{ST_LIB::PB4};

//constexpr DigitalOutputDomain::DigitalOutput bms_cs_pin{ST_LIB::PD3}; // dual spi??

//constexpr DigitalOutputDomain::DigitalOutput imd_PF5{ST_LIB::PF5}; No encuentro bypass
constexpr DigitalOutputDomain::DigitalOutput imd_enable_PE11{ST_LIB::PE11};

namespace DO {
inline DigitalOutputDomain::Instance* operational_led;
inline DigitalOutputDomain::Instance* fault_led;
inline DigitalOutputDomain::Instance* contactor_low;
inline DigitalOutputDomain::Instance* contactor_high;
inline DigitalOutputDomain::Instance* contactor_precharge;
inline DigitalOutputDomain::Instance* contactor_discharge;
inline DigitalOutputDomain::Instance* sdc_fw_fault;
inline DigitalOutputDomain::Instance* bms_cs;
//inline DigitalOutputDomain::Instance* imd_bypass;
inline DigitalOutputDomain::Instance* imd_enable;
};  // namespace DO

using ST_LIB::DigitalInputDomain;

constexpr DigitalInputDomain::DigitalInput imd_ok_PE12{ST_LIB::PE12};
constexpr DigitalInputDomain::DigitalInput aux_contactor_PG2{ST_LIB::PG2};
constexpr DigitalInputDomain::DigitalInput aux_contactor_PD12{ST_LIB::PD12};
constexpr DigitalInputDomain::DigitalInput aux_contactor_PD13{ST_LIB::PD13};
constexpr DigitalInputDomain::DigitalInput aux_contactor_PD14{ST_LIB::PD14};

namespace DI {
inline DigitalInputDomain::Instance* imd_ok;
inline DigitalInputDomain::Instance* aux_contactor_low;
inline DigitalInputDomain::Instance* aux_contactor_high;
inline DigitalInputDomain::Instance* aux_contactor_precharge;
inline DigitalInputDomain::Instance* aux_contactor_discharge;
}

using ST_LIB::ADCDomain;

inline constinit float voltage_reading_ch2{0.0f};
inline constinit float current_reading{0.0f};

constexpr ADCDomain::ADC adc_PA4{ST_LIB::PA4, voltage_reading_ch2};
constexpr ADCDomain::ADC adc_PA5{ST_LIB::PA5, current_reading};

namespace ADC {
inline ADCDomain::Instance* adc_voltage_ch2;
inline ADCDomain::Instance* adc_current;
};  // namespace ADC

using ST_LIB::TimerAF;
using ST_LIB::TimerChannel;
using ST_LIB::TimerDomain;
using ST_LIB::TimerPin;
using ST_LIB::TimerRequest;
using ST_LIB::TimerWrapper;

constexpr TimerDomain::Timer timer_us_tick_def{{
    .request = TimerRequest::GeneralPurpose32bit_5
}};

namespace GlobalTimer{
constexpr TimerPin ic_pin = {
    .af = TimerAF::InputCapture, .pin = ST_LIB::PA6, .channel = TimerChannel::CHANNEL_1};
};


// constexpr TimerDomain::Timer timer_imd{{
//     .request = TimerRequest::GeneralPurpose32bit_24
// }, GlobalTimer::ic_pin};

namespace GlobalTimer {
// inline TimerWrapper<timer_us_tick_def> global_us_timer;
inline TIM_TypeDef* global_us_timer;
//inline TimerWrapper<timer_imd> input_timer;
};  // namespace GlobalTimer

#define GetMicroseconds() GlobalTimer::global_us_timer->CNT

using ST_LIB::DMA_Domain;
using ST_LIB::SPIDomain;

// cambiar todo el spi
// Configuración SPI para LTC6810
// consteval SPIDomain::SPIConfig get_bms_config() {
//     SPIDomain::SPIConfig c{
//         SPIDomain::ClockPolarity::HIGH, SPIDomain::ClockPhase::SECOND_EDGE,
//         SPIDomain::BitOrder::MSB_FIRST,
//         SPIDomain::NSSMode::SOFTWARE  // Manejamos CS manualmente
//     };
//     c.data_size = ST_LIB::SPIDomain::DataSize::SIZE_8BIT;
//     return c;
// }

// // Dispositivo SPI3
// inline constexpr auto bms_spi3 =
//     SPIDomain::Device<DMA_Domain::Stream::dma2_stream0, DMA_Domain::Stream::dma2_stream1>(
//         SPIDomain::SPIMode::MASTER, SPIDomain::SPIPeripheral::spi3, 1000000, ST_LIB::PC10,
//         ST_LIB::PC11, ST_LIB::PC12, get_bms_config());

// namespace NewSPI {
// inline SPIDomain::Instance* bms_spi_pins;
// inline std::optional<SPIDomain::SPIWrapper<bms_spi3>> bms_wrapper;
// }  // namespace NewSPI

// Tasks and timeouts id
inline uint16_t id_timeout_precharge;
inline uint16_t id_check_precharge;
