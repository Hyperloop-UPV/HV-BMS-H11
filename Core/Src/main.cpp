#include "main.h"

#include "HVBMS/HVBMS.hpp"
#include "HVBMS/Sensors/Sensors.hpp"
#include "ST-LIB.hpp"

using ST_LIB::EthernetDomain;

#if defined(USE_PHY_LAN8742)
constexpr auto eth = EthernetDomain::Ethernet(EthernetDomain::PINSET_H10, "00:80:e1:00:01:07",
                                              "192.168.1.7", "255.255.0.0");
#elif defined(USE_PHY_LAN8700)
constexpr auto eth = EthernetDomain::Ethernet(EthernetDomain::PINSET_H11, "50:50:71:40:01:67",
                                              "192.168.1.7", "255.255.0.0");
#elif defined(USE_PHY_KSZ8041)
constexpr auto eth = EthernetDomain::Ethernet(EthernetDomain::PINSET_H11, "00:80:e1:00:01:07",
                                              "192.168.1.7", "255.255.0.0");
#else
#error "No PHY selected for Ethernet pinset selection"
#endif

using myBoard = ST_LIB::Board<eth, led_PG13, led_PG9, contactor_PD8, contactor_PD9, contactor_PD10,
                              contactor_PB14, aux_contactor_PD12, aux_contactor_PG2,
                              aux_contactor_PD13, aux_contactor_PD14, sdc_PB4, adc_PA4, adc_PA5,
                              timer_us_tick_def, timer_imd, sdc_PB5, imd_enable_PE11, imd_ok_PE12, cs_tx_PE4, bms_spi_tx,
                              bms_spi_rx>;

int main(void) {
    Hard_fault_check();
    myBoard::init();
    DO::operational_led = &myBoard::instance_of<led_PG9>();
    DO::fault_led = &myBoard::instance_of<led_PG13>();
    DO::contactor_high = &myBoard::instance_of<contactor_PD8>();
    DO::contactor_low = &myBoard::instance_of<contactor_PD9>();
    DO::contactor_discharge = &myBoard::instance_of<contactor_PD10>();
    DO::contactor_precharge = &myBoard::instance_of<contactor_PB14>();
    DO::sdc_fw_fault = &myBoard::instance_of<sdc_PB4>();
    DO::cs_tx = &myBoard::instance_of<cs_tx_PE4>();
    // DO::imd_bypass = &myBoard::instance_of<imd_PF5>();
    DO::imd_enable = &myBoard::instance_of<imd_enable_PE11>();

    DI::imd_ok = &myBoard::instance_of<imd_ok_PE12>();
    DI::aux_contactor_discharge = &myBoard::instance_of<aux_contactor_PD12>();
    DI::aux_contactor_low = &myBoard::instance_of<aux_contactor_PD13>();
    DI::aux_contactor_high = &myBoard::instance_of<aux_contactor_PD14>();
    DI::aux_contactor_precharge = &myBoard::instance_of<aux_contactor_PG2>();

    ADC::adc_voltage_ch2 = &myBoard::instance_of<adc_PA4>();
    ADC::adc_current = &myBoard::instance_of<adc_PA5>();

    NewSPI::cs_tx_pin = &myBoard::instance_of<bms_spi_tx>();
    NewSPI::bms_wrapper_tx.emplace(*NewSPI::cs_tx_pin);
    NewSPI::bms_wrapper_rx.emplace(myBoard::instance_of<bms_spi_rx>());

    auto eth_instance = &myBoard::instance_of<eth>();

    TimerWrapper<timer_us_tick_def> us_timer = get_timer_instance(myBoard, timer_us_tick_def);
    GlobalTimer::global_us_timer = us_timer.instance->tim;
    us_timer.set_prescaler((uint16_t)(us_timer.get_clock_frequency() / 1000'000) - 1);
    us_timer.counter_enable();

    GlobalTimer::input_timer = get_timer_instance(myBoard, timer_imd);

    SDC::sdc_interrupt =
        &myBoard::instance_of<sdc_PB5>();  // hay que hacer esto con un bind y tenerlo privado

    Actuators::init();
    Sensors::init();

    HVBMS::add_protections();

    HVBMS::state_machine.start();

    while (1) {
        eth_instance->update();
        HVBMS::update();
        Scheduler::update();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
