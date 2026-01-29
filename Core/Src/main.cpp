#include "main.h"
#include "ST-LIB.hpp" 
#include "BMSStateMachine.hpp"
#include "Comms/Comms.hpp"

int main(void) {
#ifdef SIM_ON
    SharedMemory::start();
#endif
    
    Hard_fault_check();

    HVBMS::Actuators::led_fault();
    HVBMS::Actuators::led_operational();
    HVBMS::Actuators::led_fault().turn_on();
    HVBMS::Actuators::led_operational().turn_on();

    STLIB::start("00:aa:6b:ae:19:6b", "192.168.1.7");   
    
    //HVBMS::Comms::start();


    //BSM.start();

    
    while (1) {
        STLIB::update();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
