################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/dht.c \
../Core/Src/light_sensor.c \
../Core/Src/liquidcrystal_i2c.c \
../Core/Src/main.c \
../Core/Src/moisture_sensor.c \
../Core/Src/plant_database.c \
../Core/Src/setup.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l4xx.c \
../Core/Src/temp_rh_sensor.c \
../Core/Src/user_interaction.c \
../Core/Src/veml7700.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/dht.o \
./Core/Src/light_sensor.o \
./Core/Src/liquidcrystal_i2c.o \
./Core/Src/main.o \
./Core/Src/moisture_sensor.o \
./Core/Src/plant_database.o \
./Core/Src/setup.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l4xx.o \
./Core/Src/temp_rh_sensor.o \
./Core/Src/user_interaction.o \
./Core/Src/veml7700.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/dht.d \
./Core/Src/light_sensor.d \
./Core/Src/liquidcrystal_i2c.d \
./Core/Src/main.d \
./Core/Src/moisture_sensor.d \
./Core/Src/plant_database.d \
./Core/Src/setup.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l4xx.d \
./Core/Src/temp_rh_sensor.d \
./Core/Src/user_interaction.d \
./Core/Src/veml7700.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L452xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/dht.cyclo ./Core/Src/dht.d ./Core/Src/dht.o ./Core/Src/dht.su ./Core/Src/light_sensor.cyclo ./Core/Src/light_sensor.d ./Core/Src/light_sensor.o ./Core/Src/light_sensor.su ./Core/Src/liquidcrystal_i2c.cyclo ./Core/Src/liquidcrystal_i2c.d ./Core/Src/liquidcrystal_i2c.o ./Core/Src/liquidcrystal_i2c.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/moisture_sensor.cyclo ./Core/Src/moisture_sensor.d ./Core/Src/moisture_sensor.o ./Core/Src/moisture_sensor.su ./Core/Src/plant_database.cyclo ./Core/Src/plant_database.d ./Core/Src/plant_database.o ./Core/Src/plant_database.su ./Core/Src/setup.cyclo ./Core/Src/setup.d ./Core/Src/setup.o ./Core/Src/setup.su ./Core/Src/stm32l4xx_hal_msp.cyclo ./Core/Src/stm32l4xx_hal_msp.d ./Core/Src/stm32l4xx_hal_msp.o ./Core/Src/stm32l4xx_hal_msp.su ./Core/Src/stm32l4xx_it.cyclo ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/stm32l4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l4xx.cyclo ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/system_stm32l4xx.su ./Core/Src/temp_rh_sensor.cyclo ./Core/Src/temp_rh_sensor.d ./Core/Src/temp_rh_sensor.o ./Core/Src/temp_rh_sensor.su ./Core/Src/user_interaction.cyclo ./Core/Src/user_interaction.d ./Core/Src/user_interaction.o ./Core/Src/user_interaction.su ./Core/Src/veml7700.cyclo ./Core/Src/veml7700.d ./Core/Src/veml7700.o ./Core/Src/veml7700.su

.PHONY: clean-Core-2f-Src

