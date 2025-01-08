/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier:  BSD-3-Clause
*/
#ifndef __ZMOD4410_SENSOR_H
#define __ZMOD4410_SENSOR_H
#include <stdint.h>
#include "sm_handle.h"

void zmod4410_sensor_open(sm_handle * handle, uint8_t address, uint8_t channel);
void zmod4410_sensor_close(sm_handle handle);
sm_sensor_status zmod4410_sensor_read(sm_handle handle, int32_t * data);
void zmod4410_sensor_fsm(void);
uint8_t * zmod4410_sensor_get_flag(sm_handle handle);
sm_result zmod4410_sensor_set_attr(sm_handle handle, sm_sensor_attributes attr, uint32_t value);
sm_result zmod4410_sensor_get_attr(sm_handle handle, sm_sensor_attributes attr, uint32_t * value);

#endif
