#include "main.h"
#include "ST-LIB.hpp" 
#include "BMSStateMachine.hpp"

int main(void) {
#ifdef SIM_ON
    SharedMemory::start();
#endif
    
    
    STLIB::start();

    

    
    while (1) {
        STLIB::update();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
