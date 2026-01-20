
#include "main.h"
#include "ST-LIB.hpp"
#include "stm32h7xx_hal.h"
#include "core_cm7.h"

// void enable_mpu(void) {
//     MPU_Region_InitTypeDef MPU_InitStruct;

//     HAL_MPU_Disable();

//     MPU_InitStruct.Enable = MPU_REGION_ENABLE;
//     MPU_InitStruct.BaseAddress = 0x20000000;   // SRAM1
//     MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
//     MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
//     MPU_InitStruct.IsBufferable = 0;
//     MPU_InitStruct.IsCacheable = 0;
//     MPU_InitStruct.IsShareable = 0;
//     MPU_InitStruct.Number = MPU_REGION_NUMBER0;
//     MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
//     MPU_InitStruct.SubRegionDisable = 0x00;
//     MPU_InitStruct.DisableExec = 0;

//     HAL_MPU_ConfigRegion(&MPU_InitStruct);

//     HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
// }

// int main(void) {
// #ifdef SIM_ON
//     SharedMemory::start();
// #endif
//     Hard_fault_check();
//     HAL_Init();
//     enable_mpu();
//     DigitalOutput led_on(PB0);
//     volatile uint32_t *ptr = (uint32_t*)0x20000000; // SRAM1, pero NO accesible
//     *ptr = 0xABCD;  // MemManage Fault
//     STLIB::start();
//     Time::register_low_precision_alarm(100, [&]() { led_on.toggle(); 
//     });

//     while (1) {
//         STLIB::update();
//     }
// }
static void enable_div0_trap(void)
{
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
}

int main(void)
{
    Hard_fault_check();
    HAL_Init();

    enable_div0_trap();

    volatile int a = 10;
    volatile int b = 0;
    volatile int c;

    c = a / b;  
    a = c;
    while (1) {
    }
}
void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
