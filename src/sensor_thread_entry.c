/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier:  BSD-3-Clause
*/
#include <sensor_thread.h>
#include "sm.h"
/* Sensor entry function */
/* pvParameters contains TaskHandle_t */
void sensor_thread_entry(void *pvParameters) {
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* TODO: add your own code here */
    sm_init();
    while (1) {
        sm_run();
    }
}
