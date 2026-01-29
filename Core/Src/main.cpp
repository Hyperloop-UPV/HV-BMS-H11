#include "main.h"
#include "ST-LIB.hpp" 
#include "BMSStateMachine.hpp"
#include "Comms/Comms.hpp"

int main(void) {
#ifdef SIM_ON
    SharedMemory::start();
#endif
    
    Hard_fault_check();

    STLIB::start("00:aa:6b:ae:19:6b", "192.168.1.7");   

    
   // HVBMS::Comms::start();

    Scheduler::register_task(1000, [](){
        state_machine.check_transitions();
    });

    state_machine.start();
    

    
    while (1) {
        STLIB::update();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
