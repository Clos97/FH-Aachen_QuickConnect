################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/rm_mqtt_onchip_da16xxx/rm_mqtt_onchip_da16xxx.c 

C_DEPS += \
./ra/fsp/src/rm_mqtt_onchip_da16xxx/rm_mqtt_onchip_da16xxx.d 

OBJS += \
./ra/fsp/src/rm_mqtt_onchip_da16xxx/rm_mqtt_onchip_da16xxx.o 

SREC += \
bgk_ra6e2_freertos.srec 

MAP += \
bgk_ra6e2_freertos.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/rm_mqtt_onchip_da16xxx/%.o: ../ra/fsp/src/rm_mqtt_onchip_da16xxx/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM33 -D_RA_ORDINAL=1 -I"C:/Users/a5153359/Downloads/Aachen_2025/src" -I"." -I"C:/Users/a5153359/Downloads/Aachen_2025/ra/fsp/inc" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra/fsp/inc/api" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra/fsp/inc/instances" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra/fsp/src/rm_freertos_port" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra/aws/FreeRTOS/FreeRTOS/Source/include" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra_gen" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra_cfg/fsp_cfg/bsp" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra_cfg/fsp_cfg" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra_cfg/aws" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra/fsp/src/rm_wifi_da16xxx" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra/fsp/src/rm_at_transport_da16xxx_uart" -I"C:/Users/a5153359/Downloads/Aachen_2025/ra/fsp/src/rm_mqtt_onchip_da16xxx" -I"C:/Users/a5153359/Downloads/Aachen_2025/src/qc-middleware" -I"C:/Users/a5153359/Downloads/Aachen_2025/src/qc-middleware/common_utils" -I"C:/Users/a5153359/Downloads/Aachen_2025/src/qc-middleware/SEGGER_RTT" -I"C:/Users/a5153359/Downloads/Aachen_2025/src/qc-middleware/sensor_manager" -std=c99 -Wno-stringop-overflow -Wno-format-truncation --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

