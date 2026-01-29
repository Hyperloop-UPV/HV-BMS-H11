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
    //BSM.start();
    HVBMS::Comms::start();

    [[maybe_unused]]bool funciona = false;

    while (1) {
        STLIB::update();
        funciona = HVBMS::Comms::tcp_connected();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
