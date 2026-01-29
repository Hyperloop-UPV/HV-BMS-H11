#include "main.h"
#include "ST-LIB.hpp" 
#include "BMSStateMachine.hpp"
#include "Comms/Comms.hpp"

int main(void) {
#ifdef SIM_ON
    SharedMemory::start();
#endif
    
    Hard_fault_check();
    STLIB::start();   
    
    //HVBMS::Comms::start();


    //BSM.start();
    HVBMS::Actuators::led_fault();
    HVBMS::Actuators::led_operational();
    HVBMS::Actuators::led_fault().turn_on();
    HVBMS::Actuators::led_operational().turn_on();
    
    while (1) {
        STLIB::update();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
