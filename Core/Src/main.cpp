#include "main.h"

#include "HVBMS/HVBMS.hpp"
#include "HVBMS/Protections/BatteryProtections.hpp"
#include "HVBMS/Sensors/Sensors.hpp"
#include "ST-LIB.hpp"

#define M16 1

#if defined(M16)
#define MASCARA "255.255.0.0"
#elif defined(M24)
#define MASCARA "255.255.255.0"
#else
#error "Mask needs to be defined"
#endif

#define MAC_ADDRESS "68:67:67:67:67:67"  // do NOT put 67 at the start (or 69)

#define IP_ADDRESS "192.168.1.7"

using ST_LIB::EthernetDomain;

#if defined(USE_PHY_LAN8742)
constexpr auto eth =
    EthernetDomain::Ethernet(EthernetDomain::PINSET_H10, MAC_ADDRESS, IP_ADDRESS, MASCARA);
#elif defined(USE_PHY_LAN8700)
constexpr auto eth =
    EthernetDomain::Ethernet(EthernetDomain::PINSET_H10, MAC_ADDRESS, IP_ADDRESS, MASCARA);
#elif defined(USE_PHY_KSZ8041)
constexpr auto eth =
    EthernetDomain::Ethernet(EthernetDomain::PINSET_H11, MAC_ADDRESS, IP_ADDRESS, MASCARA);
#else
#error "No PHY selected for Ethernet pinset selection"
#endif

using myBoard =
    ST_LIB::Board<ST_LIB::FaultPolicy<HVBMS::state_machine, &HVBMS::on_fault_enter>, eth,
                  dc_current_protection, dc_voltage_protection,
#if BATTERIES_CONNECTED
                  battery_temp_max_protection, battery_temp_min_protection,
                  battery_cell_voltage_max_protection, battery_cell_voltage_min_protection,
#endif
                  led_PG13, led_PG9, contactor_PD8, contactor_PD9, contactor_PD10, contactor_PB14,
                  contactor_PB15, aux_contactor_PD12, aux_contactor_PG2, aux_contactor_PD13,
                  aux_contactor_PD14, aux_contactor_PD15, sdc_PB4, adc_PA3, adc_PA5,
                  timer_us_tick_def, timer_imd, timeout_timer_def, sdc_PB5, imd_enable_PE11,
                  imd_ok_PE12, cs_tx_PE4, bms_spi_tx, bms_spi_rx, spi_enable_PE3, battery_intb_PE1>;

int main(void) {
    myBoard::init();
    Diagnostics::install_ethernet_sink(OrderPackets::vcu_tcp);
    DO::operational_led = &myBoard::instance_of<led_PG9>();
    DO::fault_led = &myBoard::instance_of<led_PG13>();
    DO::contactor_high = &myBoard::instance_of<contactor_PD8>();
    DO::contactor_low = &myBoard::instance_of<contactor_PD9>();
    DO::contactor_discharge = &myBoard::instance_of<contactor_PD10>();
    DO::contactor_precharge = &myBoard::instance_of<contactor_PB14>();
    DO::contactor_common_high = &myBoard::instance_of<contactor_PB15>();
    DO::sdc_fw_fault = &myBoard::instance_of<sdc_PB4>();
    DO::cs_tx = &myBoard::instance_of<cs_tx_PE4>();
    DO::spi_enable = &myBoard::instance_of<spi_enable_PE3>();
    DO::imd_enable = &myBoard::instance_of<imd_enable_PE11>();

    DI::aux_contactor_discharge = &myBoard::instance_of<aux_contactor_PD12>();
    DI::aux_contactor_low = &myBoard::instance_of<aux_contactor_PD13>();
    DI::aux_contactor_high = &myBoard::instance_of<aux_contactor_PD14>();
    DI::aux_contactor_precharge = &myBoard::instance_of<aux_contactor_PG2>();
    DI::aux_contactor_common_high = &myBoard::instance_of<aux_contactor_PD15>();
    DI::battery_intb = &myBoard::instance_of<battery_intb_PE1>();

    ADC::adc_voltage_ch1 = &myBoard::instance_of<adc_PA3>();
    ADC::adc_current = &myBoard::instance_of<adc_PA5>();

    NewSPI::bms_wrapper_rx.emplace(myBoard::instance_of<bms_spi_rx>());
    NewSPI::bms_wrapper_tx.emplace(myBoard::instance_of<bms_spi_tx>());

    Eth::eth_instance = &myBoard::instance_of<eth>();

    TimerWrapper<timer_us_tick_def> us_timer = get_timer_instance(myBoard, timer_us_tick_def);
    GlobalTimer::global_us_timer = us_timer.instance->tim;
    us_timer.set_prescaler((uint16_t)(us_timer.get_clock_frequency() / 1000'000) - 1);
    us_timer.counter_enable();

    GlobalTimer::input_timer = get_timer_instance(myBoard, timer_imd);

    GlobalTimer::input_timer.instance->tim->PSC = 600;

    TimerWrapper<timeout_timer_def> battery_timer = get_timer_instance(myBoard, timeout_timer_def);
    GlobalTimer::timeout_timer = battery_timer.instance->tim;
    battery_timer.set_prescaler((uint16_t)(us_timer.get_clock_frequency() / 1000'000) - 1);
    battery_timer.counter_enable();

    SDC::sdc_interrupt =
        &myBoard::instance_of<sdc_PB5>();  // Por culpa de C++ tengo que tener esto fuera

    IMD::ok = &myBoard::instance_of<imd_ok_PE12>();  // Y esto más de lo mismo

    Actuators::init();
    Sensors::init();

    using namespace std::chrono_literals;
    Watchdog::watchdog_time = 100ms;
    // Watchdog::start();

    while (1) {
        FaultController::check_transitions();
        Eth::eth_instance->update();
        HVBMS::update();
        myBoard::evaluate_protections();
        Diagnostics::Hub::flush();
        // El watchdog también esta en bcc_stlib.hpp, hay que ponerlo
        // Watchdog::refresh();
        Scheduler::update();
    }
}

void Error_Handler(void) {
    FAULT("HAL error handler triggered");
    while (1) {
    }
}
