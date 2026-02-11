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

namespace DO {
inline DigitalOutputDomain::Instance* operational_led;
inline DigitalOutputDomain::Instance* fault_led;
inline DigitalOutputDomain::Instance* contactor_low;
inline DigitalOutputDomain::Instance* contactor_high;
inline DigitalOutputDomain::Instance* contactor_precharge;
inline DigitalOutputDomain::Instance* contactor_discharge;
inline DigitalOutputDomain::Instance* sdc_obccu;
};  // namespace DO

namespace ADC {
inline ADCDomain::Instance* adc_voltage;
inline ADCDomain::Instance* adc_current;
};  // namespace ADC

// Enums
using States_HVBMS = DataPackets::gsm_status;
using States_BMS = DataPackets::bms_status;
using States_SDC = DataPackets::sdc_status;