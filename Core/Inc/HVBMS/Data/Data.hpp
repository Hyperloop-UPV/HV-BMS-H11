#pragma once
#include "Communications/Packets/DataPackets.hpp"
#include "ST-LIB.hpp"

using ST_LIB::DigitalOutputDomain;

constexpr DigitalOutputDomain::DigitalOutput led_PG8{ST_LIB::PG8};
constexpr DigitalOutputDomain::DigitalOutput led_PG7{ST_LIB::PG7};
constexpr DigitalOutputDomain::DigitalOutput contactor_PG14{ST_LIB::PG14};
constexpr DigitalOutputDomain::DigitalOutput contactor_PG12{ST_LIB::PG12};
constexpr DigitalOutputDomain::DigitalOutput contactor_PD4{ST_LIB::PD4};
constexpr DigitalOutputDomain::DigitalOutput contactor_PF4{ST_LIB::PF5};
constexpr DigitalOutputDomain::DigitalOutput sdc_PA11{ST_LIB::PA11};

using ST_LIB::ADCDomain;

inline constinit float voltage_reading{0.0f};
inline constinit float current_reading{0.0f};

constexpr ADCDomain::ADC adc_PF13{ST_LIB::PF13, voltage_reading};
constexpr ADCDomain::ADC adc_PA0{ST_LIB::PA0, current_reading};

using ST_LIB::TimerDomain;
using ST_LIB::TimerRequest;
using ST_LIB::TimerWrapper;

constexpr TimerDomain::Timer timer_us_tick_def{{
    .request = TimerRequest::GeneralPurpose32bit_5,
}};

using ST_LIB::DMA_Domain;
using ST_LIB::SPIDomain;

// Pin de CS para el BMS
constexpr DigitalOutputDomain::DigitalOutput bms_cs_pin{ST_LIB::PD3};

// Configuración SPI para LTC6810
consteval SPIDomain::SPIConfig get_bms_config() {
    SPIDomain::SPIConfig c{
        SPIDomain::ClockPolarity::HIGH, SPIDomain::ClockPhase::SECOND_EDGE,
        SPIDomain::BitOrder::MSB_FIRST,
        SPIDomain::NSSMode::SOFTWARE  // Manejamos CS manualmente
    };
    c.data_size = ST_LIB::SPIDomain::DataSize::SIZE_8BIT;
    return c;
}

// Dispositivo SPI3
inline constexpr auto bms_spi3 =
    SPIDomain::Device<DMA_Domain::Stream::dma2_stream0, DMA_Domain::Stream::dma2_stream1>(
        SPIDomain::SPIMode::MASTER, SPIDomain::SPIPeripheral::spi3, 1000000, ST_LIB::PC10,
        ST_LIB::PC11, ST_LIB::PC12, get_bms_config());

namespace DO {
inline DigitalOutputDomain::Instance* operational_led;
inline DigitalOutputDomain::Instance* fault_led;
inline DigitalOutputDomain::Instance* contactor_low;
inline DigitalOutputDomain::Instance* contactor_high;
inline DigitalOutputDomain::Instance* contactor_precharge;
inline DigitalOutputDomain::Instance* contactor_discharge;
inline DigitalOutputDomain::Instance* sdc_obccu;
inline DigitalOutputDomain::Instance* bms_cs;
};  // namespace DO

namespace ADC {
inline ADCDomain::Instance* adc_voltage;
inline ADCDomain::Instance* adc_current;
};  // namespace ADC

namespace GlobalTimer {
// inline TimerWrapper<timer_us_tick_def> global_us_timer;
inline TIM_TypeDef* global_us_timer;
};  // namespace GlobalTimer

namespace NewSPI {
inline SPIDomain::Instance* bms_spi_pins;
inline std::optional<SPIDomain::SPIWrapper<bms_spi3>> bms_wrapper;
}  // namespace NewSPI

#define GetMicroseconds() GlobalTimer::global_us_timer->CNT

// Enums
using States_HVBMS = DataPackets::gsm_status;
using States_BMS = DataPackets::bms_status;
// using States_SDC = DataPackets::sdc_status;