/* Library includes. */

#include  <stdint.h>
#include  <stdbool.h>

/* FreeRTOS includes. */

#include <FreeRTOS.h>
#include <task.h>

/* other includes. */

// Application Logic
#include "Application/Application_interface.h"

// Drivers
#include "GPIO/GPIO.h"
#include "UART_0/UART_0_interface.h"
#include "SERVO/ServoMotor_Interface.h"
#include "LCD/LCD_interface.h"
#include "SERVO/ServoMotor_Interface.h"
#include "Ultrasonic/ultrasonic.h"
#include "ADC/ADC_interface.h"

// Utils

/* #defines  */
#define BS_LED GPIO_PIN_2
#define POV_LED GPIO_PIN_1

uint32_t SystemCoreClock = 16000000;

// Tasks Implementation

//---------------------------------------------------------------------------------------------------

BlinkingLED_t BlinkState_LED1 = LED_Stop;
uint8_t global_VehicalSpeed;
VehicleMode_t global_DrivingState;

VehicleState_t GL_vehicle_sate;
BlinkingLED_t GL_Blinking_LED2;

// LED Indicator Tasks
// TODO Test Validity of multiple occurrences of this task

void pxTaskBlinkLED1(void *pvParameters)
{
    TickType_t xDelay;
    for (;;)
    {
        switch (BlinkState_LED1)
        {
        case LED_Full:
            xDelay = 1;
            break;
        case LED_Stop:
            xDelay = 0;
            break;
        case LED_BlinkFast:

            xDelay = 100 / portTICK_PERIOD_MS;
            break;
        case LED_BlinkMid:

            xDelay = 400 / portTICK_PERIOD_MS;
            break;
        case LED_BlinkSlow:

            xDelay = 800 / portTICK_PERIOD_MS;
            break;
        default:
            break;
        }
        if (xDelay > 1)
        {

            GPIOPinWrite(GPIO_PORTF_BASE, POV_LED, POV_LED);
            vTaskDelay(xDelay);
            GPIOPinWrite(GPIO_PORTF_BASE, POV_LED, 0x00);
            vTaskDelay(xDelay);
        }
        else if (xDelay == 1)
        {
            //LED is On
            GPIOPinWrite(GPIO_PORTF_BASE, POV_LED, POV_LED);
        }
        else if (xDelay == 0)
        {
            //LED is Off
            GPIOPinWrite(GPIO_PORTF_BASE, POV_LED, 0x00);

        }
        else
        {
            UART_0_SendString("Error LED state\n");
        }
    }
}

//---------------------------------------------------------------------------------------------------

void pxTaskBlinkLED2(void *pvParameters)
{
    TickType_t xDelay;
    for (;;)
    {
        switch (GL_Blinking_LED2)
        {
        case LED_Full:
            xDelay = 1;
            break;
        case LED_Stop:
            xDelay = 0;
            break;
        case LED_BlinkFast:

            xDelay = 100 / portTICK_PERIOD_MS;
            break;
        case LED_BlinkMid:

            xDelay = 400 / portTICK_PERIOD_MS;
            break;
        case LED_BlinkSlow:

            xDelay = 800 / portTICK_PERIOD_MS;
            break;
        default:
            break;
        }
        if (xDelay > 1)
        {

            GPIOPinWrite(GPIO_PORTF_BASE, BS_LED, BS_LED);
            vTaskDelay(xDelay);
            GPIOPinWrite(GPIO_PORTF_BASE, BS_LED, 0x00);
            vTaskDelay(xDelay);
        }
        else if (xDelay == 1)
        {
            //LED is On
            GPIOPinWrite(GPIO_PORTF_BASE, BS_LED, BS_LED);
        }
        else if (xDelay == 0)
        {
            //LED is Off
            GPIOPinWrite(GPIO_PORTF_BASE, BS_LED, 0x00);

        }
        else
        {
            UART_0_SendString("Error LED state\n");
        }
    }
}
//---------------------------------------------------------------------------------------------------
void pxTaskPOV(void *pvParameters)
{
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

    for (;;)
    {
//        Blind_Spot_Monitoring();
        uint32_t distance = UltrasonicFront_u32GetDistance();
        uint8_t distance_str[12];
// Not-Tested --------
        Speed_State_t state = Update_Frontal_POV(distance);
        Perform_Action(state);
//--------------------
        uint32_t adcValue =  ADC_GetChannelRead();
        adcValue = ADC_MapValue(adcValue, 0, 4096, 0, 100);
        global_VehicalSpeed = adcValue;

        uint8_t adc_str[12];

        itoa((adcValue), adc_str);
        itoa((distance), distance_str);

        if (distance < 5)
        {
            BlinkState_LED1 = LED_BlinkFast;
        }
        else if (distance > 10)
        {
            BlinkState_LED1 = LED_Stop;
        }
        UART_0_SendString(distance_str);
        UART_0_SendString(" cm  ");
        UART_0_SendString(adc_str);
        UART_0_SendString("% \t");

        vTaskDelay(xDelay);
    }
}
//---------------------------------------------------------------------------------------------------

int main()
{

//Init Section

    //Init ADC
    ADC_Init(GPIO_PIN_5, ADC_CTL_CH8);

    //Wheels Init
    Wheels_Init();

    //Init Led
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    // Init UltraSonic
    Ultrasonic_Init();

    //Init UART
    UART_0_Init();

//Create Tasks

    xTaskCreate(pxTaskPOV, "23", 256, NULL, 1, NULL);
    xTaskCreate(pxTaskBlinkLED1, "24", 256, NULL, 1, NULL);
    xTaskCreate(pxTaskBlinkLED2, "99", 512, NULL, 1, NULL);

    // Startup of the FreeRTOS scheduler.  The program should block here.
    vTaskStartScheduler();

    // The following line should never be reached.  Failure to allocate enough
    //  memory from the heap would be one reason.
    for (;;)
        ;

}
