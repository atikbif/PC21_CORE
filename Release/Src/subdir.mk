################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/ai_calculate.c \
../Src/ai_module.c \
../Src/ain.c \
../Src/can.c \
../Src/can_protocol.c \
../Src/can_task.c \
../Src/can_tx_stack.c \
../Src/crc.c \
../Src/din.c \
../Src/dma.c \
../Src/do_module.c \
../Src/dout.c \
../Src/eeprom.c \
../Src/elements.c \
../Src/ethernetif.c \
../Src/ethip.c \
../Src/freertos.c \
../Src/gpio.c \
../Src/heartbeat.c \
../Src/intern_regs.c \
../Src/iwdg.c \
../Src/lcd.c \
../Src/ld_prog.c \
../Src/leds.c \
../Src/lib_elements.c \
../Src/lwip.c \
../Src/main.c \
../Src/modbus_master.c \
../Src/modules.c \
../Src/pid.c \
../Src/rs485.c \
../Src/rs485_protocol.c \
../Src/rs_module.c \
../Src/rtc.c \
../Src/scada.c \
../Src/spi.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_hal_timebase_tim.c \
../Src/stm32f4xx_it.c \
../Src/sys.c \
../Src/syscalls.c \
../Src/system_stm32f4xx.c \
../Src/system_vars.c \
../Src/tcpserver.c \
../Src/udp_server.c \
../Src/update_plc_data.c \
../Src/usart.c 

OBJS += \
./Src/ai_calculate.o \
./Src/ai_module.o \
./Src/ain.o \
./Src/can.o \
./Src/can_protocol.o \
./Src/can_task.o \
./Src/can_tx_stack.o \
./Src/crc.o \
./Src/din.o \
./Src/dma.o \
./Src/do_module.o \
./Src/dout.o \
./Src/eeprom.o \
./Src/elements.o \
./Src/ethernetif.o \
./Src/ethip.o \
./Src/freertos.o \
./Src/gpio.o \
./Src/heartbeat.o \
./Src/intern_regs.o \
./Src/iwdg.o \
./Src/lcd.o \
./Src/ld_prog.o \
./Src/leds.o \
./Src/lib_elements.o \
./Src/lwip.o \
./Src/main.o \
./Src/modbus_master.o \
./Src/modules.o \
./Src/pid.o \
./Src/rs485.o \
./Src/rs485_protocol.o \
./Src/rs_module.o \
./Src/rtc.o \
./Src/scada.o \
./Src/spi.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_hal_timebase_tim.o \
./Src/stm32f4xx_it.o \
./Src/sys.o \
./Src/syscalls.o \
./Src/system_stm32f4xx.o \
./Src/system_vars.o \
./Src/tcpserver.o \
./Src/udp_server.o \
./Src/update_plc_data.o \
./Src/usart.o 

C_DEPS += \
./Src/ai_calculate.d \
./Src/ai_module.d \
./Src/ain.d \
./Src/can.d \
./Src/can_protocol.d \
./Src/can_task.d \
./Src/can_tx_stack.d \
./Src/crc.d \
./Src/din.d \
./Src/dma.d \
./Src/do_module.d \
./Src/dout.d \
./Src/eeprom.d \
./Src/elements.d \
./Src/ethernetif.d \
./Src/ethip.d \
./Src/freertos.d \
./Src/gpio.d \
./Src/heartbeat.d \
./Src/intern_regs.d \
./Src/iwdg.d \
./Src/lcd.d \
./Src/ld_prog.d \
./Src/leds.d \
./Src/lib_elements.d \
./Src/lwip.d \
./Src/main.d \
./Src/modbus_master.d \
./Src/modules.d \
./Src/pid.d \
./Src/rs485.d \
./Src/rs485_protocol.d \
./Src/rs_module.d \
./Src/rtc.d \
./Src/scada.d \
./Src/spi.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_hal_timebase_tim.d \
./Src/stm32f4xx_it.d \
./Src/sys.d \
./Src/syscalls.d \
./Src/system_stm32f4xx.d \
./Src/system_vars.d \
./Src/tcpserver.d \
./Src/udp_server.d \
./Src/update_plc_data.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc -c "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32F427xx -c -I../Inc -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

