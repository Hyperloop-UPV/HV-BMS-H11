#include "main.h"
#include "ST-LIB.hpp" 
#include "BMSStateMachine.hpp"
#include "Comms/Comms.hpp"
#include "Actuators/Actuators.hpp"
#include "HVBMS.hpp"



int main(void) {
#ifdef SIM_ON
    SharedMemory::start();
#endif  
    using myBoard = ST_LIB::Board<led_PG7, led_PG8>;
    myBoard::init();   
    HVBMS::Global.operational_led = &myBoard::instance_of<led_PG8>();
    HVBMS::Global.fault_led = &myBoard::instance_of<led_PG7>();

    Hard_fault_check();

    STLIB::start("00:aa:6b:ae:19:6b", "192.168.1.7");   

    Scheduler::register_task(1000, [](){
        state_machine.check_transitions();
    });

    Scheduler::register_task(1000, HVBMS::Comms::send_packets);

    Scheduler::start();

    state_machine.start();

    HVBMS::Global.operational_led->turn_on();
    HVBMS::Global.fault_led->turn_on();

    while (1) {
        STLIB::update();
        Scheduler::update();
        
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}

