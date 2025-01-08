/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier:  BSD-3-Clause
*/
#include <stdio.h>
#include <stdint.h>
#include "common_utils.h"
#include "sm_handle.h"
#include "i2c.h"
#include "zmod4410_sensor.h"
#if BSP_CFG_RTOS
#include <sensor_thread.h>
#endif
// Uncomment the desired debug level
#include "log_disabled.h"
//#include "log_error.h"
//#include "log_warning.h"
//#include "log_info.h"
//#include "log_debug.h"

#define ZMOD4410_SAMPLE_INTERVAL_MS         3000
#define ZMOD4410_MEASUREMENT_TIME_MS        2000
// Number of sensor channels
#define NUM_CHANNELS 4

#define ZMOD4XXX_RESET_PIN    BSP_IO_PORT_04_PIN_11

#define ZMOD_LIB_TYPE			RM_ZMOD4410_LIB_TYPE_IAQ_2ND_GEN


typedef enum {
    STATE_WAIT_INIT,
	STATE_START_MEASUREMENT,
	STATE_WAIT_MEASUREMENT_I2C_TRANSACTION,
	STATE_CHECK_MEASUREMENT_COMPLETE,
	STATE_CHECK_MEASUREMENT_ERROR,
	STATE_READ_SENSOR,
	STATE_CHECK_READ_ERROR,
	STATE_CALCULATE_DATA,
    STATE_WAIT_SAMPLING_TIME,
    STATE_WAIT_TIME,
    STATE_WAIT_I2C_TRANSACTION
} zmod4410_state;

typedef enum
{
	CALLBACK_STATUS_WAIT,
	CALLBACK_STATUS_SUCCESS,
	CALLBACK_STATUS_REPEAT,
	CALLBACK_STATUS_DEVICE_ERROR
} callback_status;

static uint8_t data_ready[NUM_CHANNELS] = { 0 };
static int32_t iaq = 0;
static int32_t tvoc = 0;
static int32_t etoh = 0;
static int32_t eco2 = 0;
static uint8_t num_channels_open = 0;
static sm_sensor_status sensor_status[NUM_CHANNELS] = {SM_SENSOR_ERROR};
static volatile rm_zmod4xxx_iaq_2nd_data_t iaq_2nd_gen_data;
static volatile callback_status i2c_callback_status = CALLBACK_STATUS_WAIT;
static uint32_t acq_interval = ZMOD4410_SAMPLE_INTERVAL_MS;

void zmod4410_sensor_reset(void);

void zmod4xxx_comms_i2c_callback(rm_zmod4xxx_callback_args_t * p_args) {
	if ((RM_ZMOD4XXX_EVENT_DEV_ERR_POWER_ON_RESET == p_args->event) || 
        (RM_ZMOD4XXX_EVENT_DEV_ERR_ACCESS_CONFLICT == p_args->event)) {
		i2c_callback_status = CALLBACK_STATUS_DEVICE_ERROR;
	} else if (RM_ZMOD4XXX_EVENT_ERROR == p_args->event) {
		i2c_callback_status = CALLBACK_STATUS_REPEAT;
	} else {
		i2c_callback_status = CALLBACK_STATUS_SUCCESS;
	}
}

void zmod4410_sensor_reset(void) {
	/* Reset ZMOD sensor (active low).*/ 
    log_info("Performing reset");
    R_IOPORT_PinWrite(&g_ioport_ctrl, ZMOD4XXX_RESET_PIN, BSP_IO_LEVEL_HIGH);
    utils_delay_ms(10);
    R_IOPORT_PinWrite(&g_ioport_ctrl, ZMOD4XXX_RESET_PIN, BSP_IO_LEVEL_LOW);
    utils_delay_ms(10);
    R_IOPORT_PinWrite(&g_ioport_ctrl, ZMOD4XXX_RESET_PIN, BSP_IO_LEVEL_HIGH);
}

void zmod4410_sensor_open(sm_handle * handle, uint8_t address, uint8_t channel) {
    fsp_err_t status = FSP_SUCCESS;
#if BSP_CFG_RTOS
        /* Create a semaphore for blocking if a semaphore is not NULL */
        if (NULL != g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore)
        {
    #if BSP_CFG_RTOS == 1 // AzureOS
            tx_semaphore_create(g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_handle,
                                g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_name,
                                (ULONG)0);
    #elif BSP_CFG_RTOS == 2 // FreeRTOS
            *(g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_handle) = xSemaphoreCreateCountingStatic((UBaseType_t)1, (UBaseType_t)0, g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_memory);
    #endif
        }

        /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
        if (NULL != g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex)
        {
    #if BSP_CFG_RTOS == 1 // AzureOS
            tx_mutex_create(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_handle,
                            g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_name,
                            TX_INHERIT);
    #elif BSP_CFG_RTOS == 2 // FreeRTOS
            *(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_handle) = xSemaphoreCreateRecursiveMutexStatic(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_memory);
    #endif
        }
#endif
    if (0 == num_channels_open) {
        status = i2c_initialize();
        if (FSP_SUCCESS != status && FSP_ERR_ALREADY_OPEN != status) {
            log_error("I2C init failed");
            APP_TRAP();
        }
        // Open zmod4410 sensor instance, this must be done before calling any zmod4410 API
        status = g_zmod4xxx_sensor0.p_api->open(g_zmod4xxx_sensor0.p_ctrl, g_zmod4xxx_sensor0.p_cfg);
        if(FSP_SUCCESS != status && FSP_ERR_ALREADY_OPEN != status) {
            // Do something in case of error
            log_error("Sensor open err %d", status);
            APP_TRAP();
        }
    }
    log_info("Sensor channel %d open success",channel);
    num_channels_open++;
    handle->address = address;
    handle->channel = channel;    
}

void zmod4410_sensor_close(sm_handle handle) {
    // Handle not used here
    FSP_PARAMETER_NOT_USED(handle);
    fsp_err_t status = FSP_SUCCESS;
    if (0 == num_channels_open) {
        log_error("Sensor not open");
    } else {      
        num_channels_open--;
        if (0 == num_channels_open) {
            status = g_zmod4xxx_sensor0.p_api->close(g_zmod4xxx_sensor0.p_ctrl);
            if(FSP_SUCCESS != status) {
                // Do something in case of error
                log_error("Sensor close err %d", status);
                APP_TRAP();
            }        
        }
    }
}

sm_sensor_status zmod4410_sensor_read(sm_handle handle, int32_t * data) {
    log_debug("Sensor read channel %d", handle.channel);
    sm_sensor_status status = SM_SENSOR_ERROR;
    if (handle.channel == 0) {
        status = sensor_status[0];
        *data =  (int32_t)iaq;
        sensor_status[0] = SM_SENSOR_STALE_DATA;
    } else if (handle.channel == 1) {
        status = sensor_status[1];
        *data =  (int32_t)tvoc;
        sensor_status[1] = SM_SENSOR_STALE_DATA;
    } else if (handle.channel == 2) {
        status = sensor_status[2];
        *data =  (int32_t)etoh;
        sensor_status[2] = SM_SENSOR_STALE_DATA;
    } else if (handle.channel == 3) {
        status = sensor_status[3];
        *data =  (int32_t)eco2;
        sensor_status[3] = SM_SENSOR_STALE_DATA;
    }
    return status;
}

uint8_t * zmod4410_sensor_get_flag(sm_handle handle) {
    if (handle.channel < NUM_CHANNELS) return &data_ready[handle.channel]; else return NULL;
}

sm_result zmod4410_sensor_set_attr(sm_handle handle, sm_sensor_attributes attr, uint32_t value) {
	FSP_PARAMETER_NOT_USED(handle);	// ZMOD4410 can't set attributes per channel
    sm_result result = SM_ERROR;
    if (handle.channel < NUM_CHANNELS) {
    	switch (attr) {
			case SM_ACQUISITION_INTERVAL:
				if (value >= ZMOD4410_MEASUREMENT_TIME_MS) {
					acq_interval = value;
					result = SM_OK;
				}
				break;
			default:
				result = SM_NOT_SUPPORTED;
    	}
    }
    return result;
}

sm_result zmod4410_sensor_get_attr(sm_handle handle, sm_sensor_attributes attr, uint32_t * value) {
	FSP_PARAMETER_NOT_USED(handle);	// ZMOD4410 can't get attributes per channel
    sm_result result = SM_ERROR;
    if (handle.channel < NUM_CHANNELS) {
    	switch (attr) {
			case SM_ACQUISITION_INTERVAL:
				*value = acq_interval;
				result = SM_OK;
				break;
			default:
				result = SM_NOT_SUPPORTED;
    	}
    }
    return result;
}

void zmod4410_sensor_fsm(void) {
    static uint32_t timer, timeout;
    static zmod4410_state sensor_state = STATE_WAIT_INIT, next_state = STATE_WAIT_INIT, repeat_state = STATE_WAIT_INIT;
    static rm_zmod4xxx_raw_data_t raw_data;
    fsp_err_t status = FSP_SUCCESS;
	float temperature = 20.0F;  // This is here because RM_ZMOD4XXX_TemperatureAndHumiditySet expects floats...
	float humidity	= 50.0F;    // This is here because RM_ZMOD4XXX_TemperatureAndHumiditySet expects floats...
    rm_zmod4xxx_lib_type_t lib_type = g_zmod4xxx_sensor0_extended_cfg.lib_type;
    switch (sensor_state) {
        case STATE_WAIT_INIT:
            if (0 < num_channels_open) {
                sensor_state = STATE_START_MEASUREMENT;
            }
            break;
        case STATE_START_MEASUREMENT:
            i2c_callback_status = CALLBACK_STATUS_WAIT;
            status = RM_ZMOD4XXX_MeasurementStart(g_zmod4xxx_sensor0.p_ctrl);
			if (FSP_SUCCESS == status) {
                sensor_state = STATE_WAIT_MEASUREMENT_I2C_TRANSACTION;
            } else {
                log_error("Measurement start err %d", status);
				utils_halt_func();
			}            
            break;
        case STATE_WAIT_MEASUREMENT_I2C_TRANSACTION:
            switch (i2c_callback_status) {
                case CALLBACK_STATUS_WAIT :
                    break;
                case CALLBACK_STATUS_SUCCESS :
                    if (ZMOD_LIB_TYPE == lib_type) {
                        sensor_state = STATE_WAIT_TIME;
                        next_state = STATE_CHECK_MEASUREMENT_COMPLETE;
                        timer = utils_systime_get();
                        timeout = ZMOD4410_MEASUREMENT_TIME_MS;
                    } else {
                        sensor_state = STATE_CHECK_MEASUREMENT_COMPLETE;
                    }
                    break;
                case CALLBACK_STATUS_REPEAT :
                    sensor_state = STATE_START_MEASUREMENT;
                    break;
                default :
                    log_error("Undefined I2C callback status");
                    utils_halt_func();
                    break;                
            }
            break;
        case STATE_CHECK_MEASUREMENT_COMPLETE:
            i2c_callback_status = CALLBACK_STATUS_WAIT;
			status = RM_ZMOD4XXX_StatusCheck(g_zmod4xxx_sensor0.p_ctrl);
			if (FSP_SUCCESS == status) {
                if (ZMOD_LIB_TYPE == lib_type) {
                    next_state = STATE_CHECK_MEASUREMENT_ERROR;
                } else {
                    next_state = STATE_READ_SENSOR;
                }
                repeat_state = STATE_CHECK_MEASUREMENT_COMPLETE;
                sensor_state = STATE_WAIT_I2C_TRANSACTION;
			} else {
                log_error("Status check err %d", status);
				utils_halt_func();
			}
            break;       
        case STATE_CHECK_MEASUREMENT_ERROR:
			i2c_callback_status = CALLBACK_STATUS_WAIT;
			/* Check device error */
			status = RM_ZMOD4XXX_DeviceErrorCheck(g_zmod4xxx_sensor0.p_ctrl);
			if (FSP_SUCCESS == status) {
                next_state = STATE_READ_SENSOR;
                repeat_state = STATE_CHECK_MEASUREMENT_ERROR;
                sensor_state = STATE_WAIT_I2C_TRANSACTION;
			} else {
                log_error("Device check err %d", status);
				utils_halt_func();
			}
            break;
        case STATE_READ_SENSOR:
			i2c_callback_status = CALLBACK_STATUS_WAIT;
			status = RM_ZMOD4XXX_Read(g_zmod4xxx_sensor0.p_ctrl, &raw_data);
			if (FSP_SUCCESS == status) {
                if (ZMOD_LIB_TYPE == lib_type) {
                    next_state = STATE_CHECK_READ_ERROR;
                } else {
                    next_state = STATE_CALCULATE_DATA;
                }
                repeat_state = STATE_READ_SENSOR;
                sensor_state = STATE_WAIT_I2C_TRANSACTION;                
			} else if (FSP_ERR_SENSOR_MEASUREMENT_NOT_FINISHED == status) {
                timeout = 50;   // wait 50ms before trying to check again
                sensor_state = STATE_WAIT_TIME;
                timer = utils_systime_get();
                next_state = STATE_CHECK_MEASUREMENT_COMPLETE;                
			} else {
                log_error("Device read err %d", status);
				utils_halt_func();
			}
            break;
        case STATE_CHECK_READ_ERROR:
			i2c_callback_status = CALLBACK_STATUS_WAIT;
			status = RM_ZMOD4XXX_DeviceErrorCheck(g_zmod4xxx_sensor0.p_ctrl);
			if (FSP_SUCCESS == status) {
                next_state = STATE_CALCULATE_DATA;
                repeat_state = STATE_CHECK_READ_ERROR;
                sensor_state = STATE_WAIT_I2C_TRANSACTION;  
			} else {
                log_error("Device check err %d", status);
				utils_halt_func();
			}
            break;
        case STATE_CALCULATE_DATA:
            // Note that in this code we are not reading temperature and humidity! These are float values defined as 20 degrees C and 50%
            status = RM_ZMOD4XXX_TemperatureAndHumiditySet(g_zmod4xxx_sensor0.p_ctrl, temperature, humidity);
            if (FSP_SUCCESS != status) {
                log_error("Calculation err %d", status);
                utils_halt_func();
            }
            status = RM_ZMOD4XXX_Iaq2ndGenDataCalculate(g_zmod4xxx_sensor0.p_ctrl,&raw_data,(rm_zmod4xxx_iaq_2nd_data_t*)&iaq_2nd_gen_data);
			if (FSP_SUCCESS == status) {
				/* Gas data is valid. */
                iaq = (int32_t)(iaq_2nd_gen_data.iaq*10);
                tvoc = (int32_t)(iaq_2nd_gen_data.tvoc*100);
                etoh = (int32_t)(iaq_2nd_gen_data.etoh*100);
                eco2 = (int32_t)(iaq_2nd_gen_data.eco2*100);
                log_debug("Data available IAQ %d TVOC %d ETOH %d ECO2 %d", iaq, tvoc, etoh, eco2);
                data_ready[0] = 1;
                data_ready[1] = 1;                
                data_ready[2] = 1;
                data_ready[3] = 1;
                sensor_status[0] = SM_SENSOR_DATA_VALID;
                sensor_status[1] = SM_SENSOR_DATA_VALID;
                sensor_status[2] = SM_SENSOR_DATA_VALID;
                sensor_status[3] = SM_SENSOR_DATA_VALID;                
                sensor_state = STATE_WAIT_SAMPLING_TIME;
			} else if (FSP_ERR_SENSOR_IN_STABILIZATION == status) {
				/* Gas data is invalid. Sensor is in stabilization. */
                log_info("Sensor is in stabilization");
                sensor_status[0] = SM_SENSOR_INVALID_DATA;
                sensor_status[1] = SM_SENSOR_INVALID_DATA;
                sensor_status[2] = SM_SENSOR_INVALID_DATA;
                sensor_status[3] = SM_SENSOR_INVALID_DATA;                
                sensor_state = STATE_START_MEASUREMENT;
			} else {
                log_error("Calculation error %d", status);
				utils_halt_func();
			}
            break;
        case STATE_WAIT_SAMPLING_TIME:
            sensor_state = STATE_WAIT_TIME;
            next_state = STATE_START_MEASUREMENT;
            timer = utils_systime_get();
            timeout = acq_interval;
            break;
        case STATE_WAIT_TIME:
            if (utils_systime_get() >= (timer + timeout)) sensor_state = next_state;
            break;
        case STATE_WAIT_I2C_TRANSACTION:
			switch (i2c_callback_status)	{
                case CALLBACK_STATUS_WAIT :
                    break;
                case CALLBACK_STATUS_SUCCESS :
                    sensor_state = next_state;
                    break;
                case CALLBACK_STATUS_REPEAT :
                    log_info("Repeating I2C transaction");
                    sensor_state = repeat_state;
                    break;
                case CALLBACK_STATUS_DEVICE_ERROR :
                    log_error("I2C callback error");
                    utils_halt_func();
                    break;
                default :
                    log_error("Undefined I2C callback status");
                    utils_halt_func();
                    break;
			}
            break;       
        default:
            sensor_state = STATE_WAIT_INIT;        
    }
}
