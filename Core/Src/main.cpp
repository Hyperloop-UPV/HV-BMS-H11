#include "main.h"
#include "ST-LIB.hpp" 
#include "HVBMS/HVBMS.hpp"

using ST_LIB::EthernetDomain;

#if defined(USE_PHY_LAN8742)
constexpr auto eth =
    EthernetDomain::Ethernet(EthernetDomain::PINSET_H10, "00:80:e1:00:01:07",
                             "192.168.1.7", "255.255.0.0");
#elif defined(USE_PHY_LAN8700)
constexpr auto eth =
    EthernetDomain::Ethernet(EthernetDomain::PINSET_H10, "00:80:e1:00:01:07",
                             "192.168.1.7", "255.255.0.0");
#elif defined(USE_PHY_KSZ8041)
constexpr auto eth =
    EthernetDomain::Ethernet(EthernetDomain::PINSET_H11, "00:80:e1:00:01:07",
                             "192.168.1.7", "255.255.0.0");
#else
#error "No PHY selected for Ethernet pinset selection"
#endif

using myBoard = ST_LIB::Board<eth, led_PG7, led_PG8, 
                contactor_PG14, contactor_PG12, contactor_PD4, contactor_PF4,
                sdc_PA11>;   
                 
int main(void) {
#ifdef SIM_ON
    SharedMemory::start();  
#endif  

    myBoard::init();   
    DO::operational_led = &myBoard::instance_of<led_PG8>();
    DO::fault_led = &myBoard::instance_of<led_PG7>();
    DO::contactor_high = &myBoard::instance_of<contactor_PG12>();
    DO::contactor_low = &myBoard::instance_of<contactor_PG14>();
    DO::contactor_precharge = &myBoard::instance_of<contactor_PD4>();
    DO::contactor_discharge = &myBoard::instance_of<contactor_PF4>();
    DO::sdc_obccu = &myBoard::instance_of<sdc_PA11>();
    auto eth_instance = &myBoard::instance_of<eth>();

    Hard_fault_check();

    HVBMS::state_machine.start();

    Scheduler::register_task(1000000, []() { HVBMS::state_machine.check_transitions(); });

    Scheduler::start();
    
    while (1) {
        eth_instance->update();
        Scheduler::update();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}

