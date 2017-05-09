/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "board.h"
#include "tasks.h"

#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"
#include "fsl_adc16.h"

#define BATT_FULL_LED_GPIO GPIOB
#define BATT_FULL_LED_GPIO_PORT PORTB
#define BATT_FULL_LED_PIN 10U

#define BATT_MED_LED_GPIO GPIOB
#define BATT_MED_LED_GPIO_PORT PORTB
#define BATT_MED_LED_PIN 9U

#define BATT_LOW_LED_GPIO GPIOB
#define BATT_LOW_LED_GPIO_PORT PORTB
#define BATT_LOW_LED_PIN 8U

#define BATT_SENSE_EN_GPIO GPIOC
#define BATT_SENSE_EN_GPIO_PORT PORTC
#define BATT_SENSE_EN_PIN 2U

#define BATT_CRG_EN_1_GPIO GPIOE
#define BATT_CRG_EN_1_GPIO_PORT PORTE
#define BATT_CRG_EN_1_PIN 22U

#define BATT_CRG_EN_2_GPIO GPIOE
#define BATT_CRG_EN_2_GPIO_PORT PORTE
#define BATT_CRG_EN_2_PIN 23U

#define BATT_CRG_POK_GPIO GPIOC
#define BATT_CRG_POK_GPIO_PORT PORTC
#define BATT_CRG_POK_PIN 3U

#define BATT_CHARGING_GPIO GPIOA
#define BATT_CHARGING_GPIO_PORT PORTA
#define BATT_CHARGING_PIN 5U

#define DEMO_ADC16_BASE ADC0
#define DEMO_ADC16_CHANNEL_GROUP 0U
#define DEMO_ADC16_USER_CHANNEL 9U /*PTB1, ADC0_SE9 */
#define DEMO_ADC16_GPIO GPIOB
#define DEMO_ADC16_GPIO_PORT PORTB
#define DEMO_ADC16_PIN 1U

typedef enum {
	CHARGE_RATE_100MA = 0,
	CHARGE_RATE_500MA = 1,
	CHARGE_RATE_724MA = 2,
	CHARGE_RATE_SUSPEND = 3
}CHARGE_RATE;

typedef enum {
	CHARGE_POWER_OK,
	CHARGE_POWER_OFF,
	CHARGE_CHARGING,
	CHARGE_CHARGE_COMPLETE
}CHARGE_STATUS;

typedef enum{
	CHARGING_NEEDED,
	CHARGING_ACTIVE,
	CHARGING_COMPLETE
}CHARGING_STATE;

static void init_leds(void);
static void init_charger(void);
static void init_charging_status(void);
CHARGE_STATUS readPOKStatus(void);
static void setChargingRate(CHARGE_RATE rate);
CHARGE_STATUS readChargingStatus(void);
static void init_charge_sense_enable(void);

static adc16_channel_config_t adc16ChannelConfigStruct;

void batteryCharger_task(void *pvParameters){
	init_leds();
	init_charger();
	init_charging_status();
	init_charge_sense_enable();
	CHARGING_STATE currentState = CHARGING_COMPLETE;
	uint32_t adcResult = 0;

	const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;

	for(;;){

		//Just for now see if the charge says it wants to charge. This will only take affect on power up.
		if(CHARGE_CHARGE_COMPLETE != readChargingStatus()){
			currentState = CHARGING_NEEDED;
		}

		if(CHARGE_POWER_OK == readPOKStatus()){ //We have bee connected to power, see if we need to charge.
			if(CHARGE_CHARGE_COMPLETE != readChargingStatus() && CHARGING_NEEDED == currentState){ //We need to charge and the charge hasn't completed.
				setChargingRate(CHARGE_RATE_724MA);
				currentState = CHARGING_ACTIVE;
			} else if (CHARGE_CHARGE_COMPLETE == readChargingStatus() && CHARGING_ACTIVE == currentState) { //We were charging and now it is complete.
				currentState = CHARGING_COMPLETE;
				setChargingRate(CHARGE_RATE_SUSPEND);
				GPIO_ClearPinsOutput(BATT_FULL_LED_GPIO, 1u << BATT_FULL_LED_PIN);
			}
		} else{
			//Turn off charger because we have been un-hooked from power
			currentState = CHARGING_COMPLETE;
			setChargingRate(CHARGE_RATE_SUSPEND);
		}

		if(true){
        /*
         When in software trigger mode, each conversion would be launched once calling the "ADC16_ChannelConfigure()"
         function, which works like writing a conversion command and executing it. For another channel's conversion,
         just to change the "channelNumber" field in channel's configuration structure, and call the
         "ADC16_ChannelConfigure() again.
        */
		GPIO_ClearPinsOutput(BATT_SENSE_EN_GPIO, 1u << BATT_SENSE_EN_PIN);

        ADC16_SetChannelConfig(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP, &adc16ChannelConfigStruct);
        vTaskDelay(10);
        while (0U == (kADC16_ChannelConversionDoneFlag &
                      ADC16_GetChannelStatusFlags(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP)))
        {
        }
        adcResult = ADC16_GetChannelConversionValue(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP);
        adcResult*=2; //scale the value because we have a voltage divider that gives us half. 5200 = 4.02v

        GPIO_SetPinsOutput(BATT_SENSE_EN_GPIO, 1u << BATT_SENSE_EN_PIN);
		}
		//if the voltage is low then start flashing the low led.
		if(adcResult < 3900){
			GPIO_TogglePinsOutput(BATT_LOW_LED_GPIO, 1u << BATT_LOW_LED_PIN);
		}
	vTaskDelay(xDelay);
	}
}


static void init_leds(void){
	/* Define the init structure for the output LED pin*/
	gpio_pin_config_t status_led_config = {
			kGPIO_DigitalOutput, 1
	};

	CLOCK_EnableClock(kCLOCK_PortB);
	PORT_SetPinMux(BATT_LOW_LED_GPIO_PORT, BATT_LOW_LED_PIN, kPORT_MuxAsGpio);
	PORT_SetPinMux(BATT_MED_LED_GPIO_PORT, BATT_MED_LED_PIN, kPORT_MuxAsGpio);
	PORT_SetPinMux(BATT_FULL_LED_GPIO_PORT, BATT_FULL_LED_PIN, kPORT_MuxAsGpio);

	/* Init output LED GPIO. */
	GPIO_PinInit(BATT_LOW_LED_GPIO, BATT_LOW_LED_PIN, &status_led_config);
	GPIO_PinInit(BATT_MED_LED_GPIO, BATT_MED_LED_PIN, &status_led_config);
	GPIO_PinInit(BATT_FULL_LED_GPIO, BATT_FULL_LED_PIN, &status_led_config);

	//Clear to Enable
	GPIO_SetPinsOutput(BATT_LOW_LED_GPIO,  1u << BATT_LOW_LED_PIN);
	GPIO_SetPinsOutput(BATT_MED_LED_GPIO,  1u << BATT_MED_LED_PIN);
	GPIO_SetPinsOutput(BATT_FULL_LED_GPIO,  1u << BATT_FULL_LED_PIN);
}


static void init_charger(void){
	/* Define the init structure for the output LED pin*/
	gpio_pin_config_t charger_led_config = {
			kGPIO_DigitalOutput, 1
	};

	CLOCK_EnableClock(kCLOCK_PortE);
	PORT_SetPinMux(BATT_CRG_EN_1_GPIO_PORT, BATT_CRG_EN_1_PIN, kPORT_MuxAsGpio);
	PORT_SetPinMux(BATT_CRG_EN_2_GPIO_PORT, BATT_CRG_EN_2_PIN, kPORT_MuxAsGpio);

	/* Init output LED GPIO. */
	GPIO_PinInit(BATT_CRG_EN_1_GPIO, BATT_CRG_EN_1_PIN, &charger_led_config);
	GPIO_PinInit(BATT_CRG_EN_2_GPIO, BATT_CRG_EN_2_PIN, &charger_led_config);

	setChargingRate(CHARGE_RATE_SUSPEND);
}

static void init_charging_status(void){
	/* Define the init structure for the LED input pin*/
	gpio_pin_config_t charging_status_led_config = {
			kGPIO_DigitalInput, 1,
	};

	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_PortA);
	PORT_SetPinMux(BATT_CRG_POK_GPIO_PORT, BATT_CRG_POK_PIN, kPORT_MuxAsGpio);
	PORT_SetPinMux(BATT_CHARGING_GPIO_PORT, BATT_CHARGING_PIN, kPORT_MuxAsGpio);

	/* Init output LED GPIO. */
	GPIO_PinInit(BATT_CRG_POK_GPIO, BATT_CRG_POK_PIN, &charging_status_led_config);
	GPIO_PinInit(BATT_CHARGING_GPIO, BATT_CHARGING_PIN, &charging_status_led_config);
}

static void init_charge_sense_enable(void){

	gpio_pin_config_t charge_sense_enable_config = {
			kGPIO_DigitalOutput, 1
	};

    adc16_config_t adc16ConfigStruct;
    /*
     * adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceVref;
     * adc16ConfigStruct.clockSource = kADC16_ClockSourceAsynchronousClock;
     * adc16ConfigStruct.enableAsynchronousClock = true;
     * adc16ConfigStruct.clockDivider = kADC16_ClockDivider8;
     * adc16ConfigStruct.resolution = kADC16_ResolutionSE12Bit;
     * adc16ConfigStruct.longSampleMode = kADC16_LongSampleDisabled;
     * adc16ConfigStruct.enableHighSpeed = false;
     * adc16ConfigStruct.enableLowPower = false;
     * adc16ConfigStruct.enableContinuousConversion = false;
     */
    ADC16_GetDefaultConfig(&adc16ConfigStruct);
    ADC16_Init(DEMO_ADC16_BASE, &adc16ConfigStruct);
    ADC16_EnableHardwareTrigger(DEMO_ADC16_BASE, false); /* Make sure the software trigger is used. */
    adc16ChannelConfigStruct.channelNumber = DEMO_ADC16_USER_CHANNEL;
    adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = false;


	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_Adc0);
	PORT_SetPinMux(BATT_SENSE_EN_GPIO_PORT, BATT_SENSE_EN_PIN, kPORT_MuxAsGpio);
	GPIO_PinInit(BATT_SENSE_EN_GPIO, BATT_SENSE_EN_PIN, &charge_sense_enable_config);
	GPIO_SetPinsOutput(BATT_SENSE_EN_GPIO, 1u << BATT_SENSE_EN_PIN);

	PORT_SetPinMux(DEMO_ADC16_GPIO_PORT, DEMO_ADC16_PIN, kPORT_PinDisabledOrAnalog);

}

CHARGE_STATUS readPOKStatus(void){
	CHARGE_STATUS status = CHARGE_POWER_OFF;
	if(!GPIO_ReadPinInput(BATT_CRG_POK_GPIO, BATT_CRG_POK_PIN)){
		status = CHARGE_POWER_OK;
	}
	return status;
}

CHARGE_STATUS readChargingStatus(void){
	CHARGE_STATUS status = CHARGE_CHARGING;
	if(GPIO_ReadPinInput(BATT_CHARGING_GPIO, BATT_CHARGING_PIN)){
		status = CHARGE_CHARGE_COMPLETE;
	}
	return status;
}

static void setChargingRate(CHARGE_RATE rate){
	switch(rate){
	case CHARGE_RATE_100MA:
		GPIO_ClearPinsOutput(BATT_CRG_EN_1_GPIO, 1u << BATT_CRG_EN_1_PIN);
		GPIO_ClearPinsOutput(BATT_CRG_EN_2_GPIO, 1u << BATT_CRG_EN_2_PIN);
		break;
	case CHARGE_RATE_500MA:
		GPIO_SetPinsOutput(BATT_CRG_EN_1_GPIO, 1u << BATT_CRG_EN_1_PIN);
		GPIO_ClearPinsOutput(BATT_CRG_EN_2_GPIO, 1u << BATT_CRG_EN_2_PIN);
		break;
	case CHARGE_RATE_724MA:
		GPIO_ClearPinsOutput(BATT_CRG_EN_1_GPIO, 1u << BATT_CRG_EN_1_PIN);
		GPIO_SetPinsOutput(BATT_CRG_EN_2_GPIO, 1u << BATT_CRG_EN_2_PIN);
		break;
	case CHARGE_RATE_SUSPEND:
	default:
		GPIO_SetPinsOutput(BATT_CRG_EN_1_GPIO, 1u << BATT_CRG_EN_1_PIN);
		GPIO_SetPinsOutput(BATT_CRG_EN_2_GPIO, 1u << BATT_CRG_EN_2_PIN);
		break;
	};
}
