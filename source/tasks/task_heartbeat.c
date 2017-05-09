/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "board.h"
#include "tasks.h"

#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"

static void init(void){
	  CLOCK_EnableClock(kCLOCK_PortD);                           /* Port D Clock Gate Control: Clock enabled */
	  PORT_SetPinMux(BOARD_LED_BLUE_GPIO_PORT, BOARD_LED_BLUE_GPIO_PIN, kPORT_MuxAsGpio);            /* PORTD (pin 27) is configured as output */
}

void heartbeat_task(void *pvParameters)
{
	const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;

	init();

	LED_BLUE_INIT(1);

    for (;;)
    {
       LED_BLUE_TOGGLE();
       vTaskDelay(xDelay);
    }
}

/*
     Define the init structure for the output LED pin
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, 0,
    };
     Init output LED GPIO.
    GPIO_PinInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN, &led_config);


    for (;;)
    {
        GPIO_TogglePinsOutput(BOARD_LED_BLUE_GPIO, 1u << BOARD_LED_BLUE_GPIO_PIN);
       vTaskDelay(xDelay);
    }
*/
