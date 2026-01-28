#include "main.h"
#include "ST-LIB.hpp" 
#include "BMSStateMachine.hpp"

int main(void) {
#ifdef SIM_ON
    SharedMemory::start();
#endif
    
    Hard_fault_check();
    STLIB::start("00:aa:6b:ae:19:6b");    
    BSM.start();
    
    while (1) {
        STLIB::update();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
