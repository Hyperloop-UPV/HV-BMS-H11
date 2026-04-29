#include "main.h"

#include "HVBMS/HVBMS.hpp"
#include "HVBMS/Sensors/Sensors.hpp"
#include "ST-LIB.hpp"

using ST_LIB::EthernetDomain;

#if defined(USE_PHY_LAN8742)
constexpr auto eth = EthernetDomain::Ethernet(EthernetDomain::PINSET_H10, "00:80:e1:00:01:07",
                                              "192.168.1.7", "255.255.0.0");
#elif defined(USE_PHY_LAN8700)
constexpr auto eth = EthernetDomain::Ethernet(EthernetDomain::PINSET_H10, "50:50:71:40:01:67",
                                              "192.168.1.7", "255.255.255.0");
#elif defined(USE_PHY_KSZ8041)
constexpr auto eth = EthernetDomain::Ethernet(EthernetDomain::PINSET_H11, "00:80:e1:00:01:07",
                                              "192.168.1.7", "255.255.0.0");
#else
#error "No PHY selected for Ethernet pinset selection"
#endif

constexpr ST_LIB::TimerPin pwm_in_pin{
    .af = ST_LIB::TimerAF::PWM,
    .pin = ST_LIB::PF11,
    .channel = ST_LIB::TimerChannel::CHANNEL_1,
};
constexpr ST_LIB::TimerDomain::Timer timer_pwm_in_def{
    {
        .request = ST_LIB::TimerRequest::GeneralPurpose32bit_24,
    },
    pwm_in_pin};

constexpr ST_LIB::TimerPin ic_pin{
    .af = ST_LIB::TimerAF::InputCapture,
    .pin = ST_LIB::PF7,
    .channel = ST_LIB::TimerChannel::CHANNEL_2,
};
constexpr ST_LIB::TimerDomain::Timer timer_ic_def{
    {
        .request = ST_LIB::TimerRequest::GeneralPurpose32bit_23,
    },
    ic_pin};

using myBoard = ST_LIB::Board<eth, led_PG7, led_PG8, contactor_PG14, contactor_PG12, contactor_PD4,
                              contactor_PF4, sdc_PA11, adc_PF13, adc_PA0, timer_us_tick_def,
                              bms_spi3, bms_cs_pin, sdc_PB12, imd_PF5, imd_pow_PE2, imd_ok_PA12,
                              timer_pwm_in_def, timer_ic_def>;

int main(void) {
    Hard_fault_check();
    myBoard::init();
    DO::operational_led = &myBoard::instance_of<led_PG8>();
    DO::fault_led = &myBoard::instance_of<led_PG7>();
    DO::contactor_high = &myBoard::instance_of<contactor_PG12>();
    DO::contactor_low = &myBoard::instance_of<contactor_PG14>();
    DO::contactor_precharge = &myBoard::instance_of<contactor_PD4>();
    DO::contactor_discharge = &myBoard::instance_of<contactor_PF4>();
    DO::sdc_obccu = &myBoard::instance_of<sdc_PA11>();
    DO::bms_cs = &myBoard::instance_of<bms_cs_pin>();
    DO::imd_bypass = &myBoard::instance_of<imd_PF5>();
    DO::imd_pow = &myBoard::instance_of<imd_pow_PE2>();

    DI::imd_ok = &myBoard::instance_of<imd_ok_PA12>();

    ADC::adc_voltage = &myBoard::instance_of<adc_PF13>();
    ADC::adc_current = &myBoard::instance_of<adc_PA0>();

    NewSPI::bms_spi_pins = &myBoard::instance_of<bms_spi3>();
    NewSPI::bms_wrapper.emplace(*NewSPI::bms_spi_pins);

    auto eth_instance = &myBoard::instance_of<eth>();

    TimerWrapper<timer_us_tick_def> us_timer = get_timer_instance(myBoard, timer_us_tick_def);
    GlobalTimer::global_us_timer = us_timer.instance->tim;
    us_timer.set_prescaler(us_timer.get_clock_frequency() / 1000'000);
    us_timer.counter_enable();

    // GlobalTimer::input_timer = get_timer_instance(myBoard, timer_imd);
    auto pwm_in_tim = get_timer_instance(myBoard, timer_pwm_in_def);
    auto ic_tim = get_timer_instance(myBoard, timer_ic_def);

    auto pwm_in = pwm_in_tim.template get_pwm<pwm_in_pin>();
    auto ic = ic_tim.template get_input_capture<ic_pin, ST_LIB::TimerChannel::CHANNEL_1>();

    pwm_in.configure(10, 20.0);
    pwm_in.turn_on();
    HAL_Delay(10);

    ic.turn_on();

    // HAL_Delay(4000);

    SDC::sdc_interrupt =
        &myBoard::instance_of<sdc_PB12>();  // hay que hacer esto con un bind y tenerlo privado

    Actuators::init();
    Sensors::init();

    HVBMS::add_protections();

    HVBMS::state_machine.start();

    while (1) {
        eth_instance->update();
        HVBMS::update();
        Scheduler::update();
        [[maybe_unused]] float duty_tmp = ic.get_duty_cycle();
        [[maybe_unused]] float freq_tmp = ic.get_frequency();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}