#ifndef _TASK_H_
#define _TASK_H_


/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void heartbeat_task(void *pvParameters);
void batteryCharger_task(void *pvParameters);
void magnetometer_task(void *pvParameters);
//void alt_press_temp_task(void *pvParameters);
//void inertial_task(void *pvParameters);

#endif
