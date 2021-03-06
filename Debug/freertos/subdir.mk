################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freertos/croutine.c \
../freertos/event_groups.c \
../freertos/fsl_tickless_lptmr.c \
../freertos/fsl_tickless_systick.c \
../freertos/heap_4.c \
../freertos/list.c \
../freertos/port.c \
../freertos/queue.c \
../freertos/tasks.c \
../freertos/timers.c 

OBJS += \
./freertos/croutine.o \
./freertos/event_groups.o \
./freertos/fsl_tickless_lptmr.o \
./freertos/fsl_tickless_systick.o \
./freertos/heap_4.o \
./freertos/list.o \
./freertos/port.o \
./freertos/queue.o \
./freertos/tasks.o \
./freertos/timers.o 

C_DEPS += \
./freertos/croutine.d \
./freertos/event_groups.d \
./freertos/fsl_tickless_lptmr.d \
./freertos/fsl_tickless_systick.d \
./freertos/heap_4.d \
./freertos/list.d \
./freertos/port.d \
./freertos/queue.d \
./freertos/tasks.d \
./freertos/timers.d 


# Each subdirectory must supply rules for building sources it contributes
freertos/%.o: ../freertos/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DDEBUG -DFSL_RTOS_FREE_RTOS -DFRDM_KL25Z -DFREEDOM -DSDK_DEBUGCONSOLE=0 -DSDK_DEBUGCONSOLE_UART -D__MCUXPRESSO -D__USE_CMSIS -DSDK_OS_FREE_RTOS -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -D__REDLIB__ -I"C:\Users\isaac.rose\Documents\MCUXpressoIDE_10.0.0_344\workspace\frdmkl25z_rtos_examples_freertos_hello\source" -I"C:\Users\isaac.rose\Documents\MCUXpressoIDE_10.0.0_344\workspace\frdmkl25z_rtos_examples_freertos_hello" -I"C:\Users\isaac.rose\Documents\MCUXpressoIDE_10.0.0_344\workspace\frdmkl25z_rtos_examples_freertos_hello\drivers" -I"C:\Users\isaac.rose\Documents\MCUXpressoIDE_10.0.0_344\workspace\frdmkl25z_rtos_examples_freertos_hello\startup" -I"C:\Users\isaac.rose\Documents\MCUXpressoIDE_10.0.0_344\workspace\frdmkl25z_rtos_examples_freertos_hello\board" -I"C:\Users\isaac.rose\Documents\MCUXpressoIDE_10.0.0_344\workspace\frdmkl25z_rtos_examples_freertos_hello\CMSIS" -I"C:\Users\isaac.rose\Documents\MCUXpressoIDE_10.0.0_344\workspace\frdmkl25z_rtos_examples_freertos_hello\utilities" -I"C:\Users\isaac.rose\Documents\MCUXpressoIDE_10.0.0_344\workspace\frdmkl25z_rtos_examples_freertos_hello\freertos" -O0 -fno-common -g -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


