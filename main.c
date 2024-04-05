#include  <stdint.h>
#include  <stdbool.h>

#include <FreeRTOS.h>
#include <task.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include  "BitMath.h"
#include  "tm4c123gh6pm.h"

#include "Application/Application_interface.h"

#include "UART_0/UART_0_interface.h"

uint32_t SystemCoreClock = 16000000;

#define BUILT_IN_LED_1  1
#define BUILT_IN_LED_2  2
#define BUILT_IN_LED_3  3

void vPeriodicTask(void *pvParameters)
{

    // Establish the task's period.
    const TickType_t xDelay = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {

        Tog_Bit(GPIO_PORTF_DATA_R, BUILT_IN_LED_3);

        uart_sendString("helllllllllo\n");

        // Block until the next release time.
        //vTaskDelayUntil(&xLastWakeTime, xDelay);
        vTaskDelay(xDelay);
    }

}

//----------------------
//------------
int main()
{

    // Enable Clock for PORTF
    Set_Bit(SYSCTL_RCGCGPIO_R, 5);

//-----------------
    // Enable Digital PF3
    Set_Bit(GPIO_PORTF_DEN_R, BUILT_IN_LED_3);

    // Enable Digital PF2
    Set_Bit(GPIO_PORTF_DEN_R, BUILT_IN_LED_2);

//----------------------
    // Disable alternative function for PF3
    Clr_Bit(GPIO_PORTF_AFSEL_R, BUILT_IN_LED_3);
//--------------------
    // PF3 as output
    Set_Bit(GPIO_PORTF_DIR_R, BUILT_IN_LED_3);

    // PF2 as output
    Set_Bit(GPIO_PORTF_DIR_R, BUILT_IN_LED_2);

//    char cThisChar;

    uart_init();

    xTaskCreate(vPeriodicTask, "My Task", 256, NULL, 1, NULL);

    // Startup of the FreeRTOS scheduler.  The program should block here.
    vTaskStartScheduler();

    // The following line should never be reached.  Failure to allocate enough
    //  memory from the heap would be one reason.
    for (;;)
        ;

}
