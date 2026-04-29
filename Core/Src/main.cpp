#include "main.h"

#include "ST-LIB.hpp"

using namespace ST_LIB;

constexpr auto led = ST_LIB::DigitalOutputDomain::DigitalOutput(ST_LIB::PF13);

#if 0
// TODO: This pin doesn't output a pwm???
constexpr ST_LIB::TimerPin pwm_out_pin{
    .af = ST_LIB::TimerAF::PWM,
    .pin = ST_LIB::PC12,
    .channel = ST_LIB::TimerChannel::CHANNEL_1,
};
constexpr ST_LIB::TimerDomain::Timer timer_pwm_out_def{{
    .request = ST_LIB::TimerRequest::GeneralPurpose_15,
}, pwm_out_pin};
#else
constexpr ST_LIB::TimerPin pwm_out_pin{
    .af = ST_LIB::TimerAF::PWM,
    .pin = ST_LIB::PF6,
    .channel = ST_LIB::TimerChannel::CHANNEL_1,
};
constexpr ST_LIB::TimerDomain::Timer timer_pwm_out_def{
    {
        .request = ST_LIB::TimerRequest::GeneralPurpose_16,
    },
    pwm_out_pin};

#endif

#if 0
constexpr ST_LIB::TimerPin pwm_in_pin{
    .af = ST_LIB::TimerAF::PWM,
    .pin = ST_LIB::PG12,
    .channel = ST_LIB::TimerChannel::CHANNEL_1,
};
constexpr ST_LIB::TimerDomain::Timer timer_pwm_in_def{{
    .request = ST_LIB::TimerRequest::GeneralPurpose32bit_23,
}, pwm_in_pin};

constexpr ST_LIB::TimerPin ic_pin{
    .af = ST_LIB::TimerAF::InputCapture,
    .pin = ST_LIB::PC9,
    .channel = ST_LIB::TimerChannel::CHANNEL_4,
};
constexpr ST_LIB::TimerDomain::Timer timer_ic_def{{
    .request = ST_LIB::TimerRequest::GeneralPurpose_3,
}, ic_pin};
#else
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
#endif

using MainBoard = ST_LIB::Board<timer_pwm_out_def, timer_pwm_in_def, timer_ic_def>;

TIM_TypeDef* tim3 = TIM3;
#if !defined(EXAMPLE_ADC) && !defined(EXAMPLE_ETHERNET) && !defined(EXAMPLE_MPU) && \
    !defined(EXAMPLE_HARDFAULT) && !defined(EXAMPLE_EXTI)
int main(void) {
    MainBoard::init();

    if (!tim3) {
        ErrorHandler("asodnoadn");
    }

    auto pwm_out_tim = get_timer_instance(MainBoard, timer_pwm_out_def);
    auto pwm_in_tim = get_timer_instance(MainBoard, timer_pwm_in_def);
    auto ic_tim = get_timer_instance(MainBoard, timer_ic_def);

    auto pwm_out = pwm_out_tim.template get_pwm<pwm_out_pin>();
    auto pwm_in = pwm_in_tim.template get_pwm<pwm_in_pin>();
    auto ic = ic_tim.template get_input_capture<ic_pin, ST_LIB::TimerChannel::CHANNEL_1>();

    // pwm_out.configure(100*1000, 20.0f);
    // pwm_out.turn_on();
    // for(;;);

#define PWM_FREQ 10
    // #define PWM_FREQ 280
    ic_tim.instance->tim->PSC = 2000;
#define PWM_DUTY 20.0f  // 3.5f
                        // #define PWM_DUTY 3.5f
    pwm_in.configure(PWM_FREQ, PWM_DUTY);
    pwm_in.turn_on();

    HAL_Delay(10);

    ic.turn_on();

    HAL_Delay(4000);

    uint32_t freq = ic.get_frequency();
    float duty = ic.get_duty_cycle();
    pwm_out.configure(freq, duty);
    pwm_out.turn_on();

    float inc_dec = 1.0f;
    duty = 11.0f;
    while (1) {
#define PARTY_MODE 0
#if PARTY_MODE != 0
        HAL_Delay(200);
        duty += inc_dec;
        if ((duty > 90.0f) || (duty < 10.0f)) inc_dec = -inc_dec;

        pwm_in.set_duty_cycle(duty);
        pwm_out.set_duty_cycle(ic.get_duty_cycle());
        pwm_out.set_timer_frequency(ic.get_frequency());
#elif 0
        pwm_in.set_duty_cycle(duty);
        HAL_Delay(10);
        float duty_recieved = ic.get_duty_cycle();
        pwm_out.set_duty_cycle(duty_recieved);
        duty -= inc_dec;
        inc_dec *= 0.75f;
        if (duty == 0.0f) break;
#else
        (void)inc_dec;
        float duty_tmp = ic.get_duty_cycle();
        float freq_tmp = ic.get_frequency();
        pwm_out.set_duty_cycle(duty_tmp);

        pwm_out.set_timer_frequency(freq_tmp);
#endif
    }

    for (;;);
}
#endif

extern "C" void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}