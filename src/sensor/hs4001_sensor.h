/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#ifndef HS4001_SENSOR_H_
#define HS4001_SENSOR_H_

#include <stdint.h>
#include "sm_handle.h"

void hs4001_sensor_open(sm_handle * handle, uint8_t address, uint8_t channel);
void hs4001_sensor_close(sm_handle handle);
sm_sensor_status hs4001_sensor_read(sm_handle handle, int32_t * data);
void hs4001_sensor_fsm(void);
uint8_t * hs4001_sensor_get_flag(sm_handle handle);
sm_result hs4001_sensor_set_attr(sm_handle handle, sm_sensor_attributes attr, uint32_t value);
sm_result hs4001_sensor_get_attr(sm_handle handle, sm_sensor_attributes attr, uint32_t * value);

#endif /* HS4001_SENSOR_H_ */
