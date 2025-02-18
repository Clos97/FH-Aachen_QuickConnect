/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier:  BSD-3-Clause
*/
 /*
  This file defines sensor types, drivers and instances to be used with Renesas Sensor Manager
*/

#ifndef DEFINE_SENSOR_TYPE
#define DEFINE_SENSOR_TYPE(...)
#endif
#ifndef DEFINE_SENSOR_DRIVER
#define DEFINE_SENSOR_DRIVER(...)
#endif
#ifndef DEFINE_SENSOR_INSTANCE
#define DEFINE_SENSOR_INSTANCE(...)
#endif

/******************************************************************************************
 *
 * Define all sensor types below
 * Format:
 * DEFINE_SENSOR_TYPE(type, unit, path)
 * type - the symbol to be used with this sensor type (ie.: TEMPERATURE, LIGHT, etc)
 * unit - the standard unit to be used for displaying sensor data
 * path - the path (topic) to be used when publishing data of this type
 *
 *****************************************************************************************/

DEFINE_SENSOR_TYPE(TEMPERATURE, C, temperature)

DEFINE_SENSOR_TYPE(HUMIDITY, %, humidity)

DEFINE_SENSOR_TYPE(INDOOR_AIR_QUALITY, , indoor-aqi)

DEFINE_SENSOR_TYPE(TVOC, mg/m3, tvoc)

DEFINE_SENSOR_TYPE(ETOH, ppm, EtOH)

DEFINE_SENSOR_TYPE(ESTIMATE_CO2, ppm, eCO2)


/******************************************************************************************
 *
 * Define all sensor drivers below
 * Format:
 * DEFINE_SENSOR_DRIVER(sensor_name)
 * sensor_name - this must be a unique name prefixing all thin-layer API functions
 * I.e: DEFINE_SENSOR_DRIVER(my_sensor) instructs SM to expect my_sensor_open(),
 * my_sensor_close() and my_sensor_read() functions in the respective thin-layer
 *
 *****************************************************************************************/

DEFINE_SENSOR_DRIVER(hs4001_sensor)
DEFINE_SENSOR_DRIVER(zmod4410_sensor)


/******************************************************************************************
 *
 * Define all sensor instances below
 * Format:
 * DEFINE_SENSOR_INSTANCE(type,addr,ch,drv,mult,div,offset,interv)
 * type   - one of the sensor types defined above
 * addr   - the unique address of this sensor instance (0 if not used)
 * ch     - the sensor channel used by this instance (SM_CH0 if not used)
 * drv    - the driver to be used by this instance
 * mult   - a signed 32-bit multiplier to be used for scaling the readings of this sensor
 * div    - a signed 32-bit divider to be used for scaling the readings of this sensor
 * offset - a signed 32-bit offset to be used for scaling the readings of this sensor
 * interv - the interval between readings of this sensor instance (in milliseconds)
 *
 *****************************************************************************************/

DEFINE_SENSOR_INSTANCE(TEMPERATURE, 0, SM_CH0, hs4001_sensor, 1, 100, 0, 0)

DEFINE_SENSOR_INSTANCE(HUMIDITY, 0, SM_CH1, hs4001_sensor, 1, 100, 0, 0)

DEFINE_SENSOR_INSTANCE(INDOOR_AIR_QUALITY, 0, SM_CH0, zmod4410_sensor, 1, 10, 0, 0)

DEFINE_SENSOR_INSTANCE(TVOC, 0, SM_CH1, zmod4410_sensor, 1, 100, 0, 0)

DEFINE_SENSOR_INSTANCE(ETOH, 0, SM_CH2, zmod4410_sensor, 1, 100, 0, 0)

DEFINE_SENSOR_INSTANCE(ESTIMATE_CO2, 0, SM_CH3, zmod4410_sensor, 1, 100, 0, 0)


#undef DEFINE_SENSOR_INSTANCE
#undef DEFINE_SENSOR_DRIVER
#undef DEFINE_SENSOR_TYPE
