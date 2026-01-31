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
    using myBoard = ST_LIB::Board<led_PG7, led_PG8, 
                    contactor_PG14, contactor_PG12, contactor_PD4, contactor_PF4,
                    sdc_PA11>;    
    myBoard::init();   
    HVBMS::Global::operational_led = &myBoard::instance_of<led_PG8>();
    HVBMS::Global::fault_led = &myBoard::instance_of<led_PG7>();
    HVBMS::Global::contactor_high = &myBoard::instance_of<contactor_PG12>();
    HVBMS::Global::contactor_low = &myBoard::instance_of<contactor_PG14>();
    HVBMS::Global::contactor_precharge = &myBoard::instance_of<contactor_PD4>();
    HVBMS::Global::contactor_discharge = &myBoard::instance_of<contactor_PF4>();
    HVBMS::Global::sdc_obccu = &myBoard::instance_of<sdc_PA11>();

    Hard_fault_check();
    
    STLIB::start("00:aa:6b:ae:19:6b", "192.168.1.7");   

    state_machine.start();

    Scheduler::register_task(1000000, [](){
        state_machine.check_transitions();
    });
    
    Scheduler::start();
    
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

