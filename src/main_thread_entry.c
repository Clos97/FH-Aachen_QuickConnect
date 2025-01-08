/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier:  BSD-3-Clause
*/
#include "hal_data.h"
#include "main_thread.h"
#include "user.h"
#include "sensor_thread.h"
#include "sm.h"
#include "common_utils.h"
// Uncomment the desired debug level
//#include "log_disabled.h"
//#include "log_error.h"
//#include "log_warning.h"
#include "log_info.h"
//#include "log_debug.h"

/******************************************************************************
 User Macros
*******************************************************************************/
#define WIDTH_64                                (64)
#define CONNECT_TIMEOUT                         (5000)
#define SUB_TOPIC_FILTER_COUNT                  ( 1 )
#define PUBLISH_MAX_NUMBER                      ( 3 )
#define QUEUE_RECEIVE_TIMEOUT					(100)	// this is the interval we also receive MQTT messages
#define PUBLISHING_INTERVAL_MS	                (30000)

/******************************************************************************
 User global variables
*******************************************************************************/
extern TaskHandle_t sensor_thread;
extern QueueHandle_t g_sensor_queue;
mqtt_onchip_da16xxx_sub_info_t subTopics[SUB_TOPIC_FILTER_COUNT];
mqtt_onchip_da16xxx_instance_ctrl_t g_rm_mqtt_onchip_da16xxx_instance;
/* Setup Access Point connection parameters */
static const WIFINetworkParams_t net_params = {
 .ucChannel                  = 0,
 .xPassword.xWPA.cPassphrase = WIFI_PWD,
 .ucSSID                     = WIFI_SSID,
 .xPassword.xWPA.ucLength = sizeof(WIFI_PWD),
 .ucSSIDLength            = sizeof(WIFI_SSID),
 .xSecurity               = eWiFiSecurityWPA2,
};
static const char *pSubTopics[SUB_TOPIC_FILTER_COUNT] = {
    USER_LED_TOPIC
};

/******************************************************************************
 User function prototype declarations
*******************************************************************************/
void clear_sensor_slots(sm_sensor_data * slots);

/******************************************************************************
 User function implementations
*******************************************************************************/
void clear_sensor_slots(sm_sensor_data * slots) {
    memset(slots, 0, sizeof(sm_sensor_data) * NUM_SENSORS);
}

void mqtt0_callback (mqtt_onchip_da16xxx_callback_args_t * p_args) {
    int led_state = 0;
    char * ptr = strstr(p_args->p_topic, USER_LED_TOPIC);
    if (ptr != NULL) {
        led_state = strtol((char *) p_args->p_data, NULL, 10);
        log_info("Received message %s",(char *) p_args->p_data);
        utils_set_LED(GREEN_LED, (uint8_t)led_state);
    }
}

/* Main Thread entry function */
/* pvParameters contains TaskHandle_t */
void main_thread_entry(void *pvParameters) {

    FSP_PARAMETER_NOT_USED (pvParameters);

    uint8_t timeout = 10;
    while (1) {
        sm_handle sensor_handle;
        uint8_t sensor_count = 0;
        uint16_t index = 0;
    	// check if we already initialized all the sensors
    	while (-1 != sm_get_sensor_handle(SENSOR_ANY_TYPE, &sensor_handle, &index)) {
    		sensor_count++;
        	sm_result res = sm_set_sensor_attribute(sensor_handle, SM_ACQUISITION_INTERVAL, PUBLISHING_INTERVAL_MS);
        	if (res != SM_OK) {
        		log_error("Error setting sensor attribute");
        		APP_TRAP();
        	}
    	}
    	if (sensor_count != sm_get_total_sensor_count()) {
    		vTaskDelay(1);
    		timeout--;
    		if (0 == timeout) {
        		log_error("Error waiting for sensor init, no sensors?");
        		APP_TRAP();
    		}
    	} else break;
    }

    vTaskDelay(100);

    fsp_err_t result = FSP_ERR_ABORTED;

    log_info("Starting Wi-Fi connection...");

    /* Open connection to the Wifi Module */
    if(eWiFiSuccess != WIFI_On()) {
        log_error("Wi-Fi Open failed");
        utils_halt_func();
    }

    /* Connect to the Access Point */
    if(eWiFiSuccess != WIFI_ConnectAP(&net_params)) {
        log_error("Wi-Fi Connect failed");
        utils_halt_func();
    }

    log_info("Wi-Fi connection successful!");

    mqtt_onchip_da16xxx_cfg_t g_mqtt_onchip_da16xxx_usrcfg = g_mqtt_onchip_da16xxx_cfg;
    g_mqtt_onchip_da16xxx_usrcfg.p_host_name = (char*) &EXAMPLE_MQTT_HOST;
    // ==============USER INPUT=============
	g_mqtt_onchip_da16xxx_usrcfg.mqtt_port = (uint32_t*) EXAMPLE_MQTT_PORT;
    g_mqtt_onchip_da16xxx_usrcfg.p_mqtt_user_name = (char*) &IO_USERNAME;
    g_mqtt_onchip_da16xxx_usrcfg.p_mqtt_password = (char*) &IO_PASSWORD;
	//======================================

    /* Initialize the MQTT Client connection */
    result = RM_MQTT_DA16XXX_Open(&g_rm_mqtt_onchip_da16xxx_instance, &g_mqtt_onchip_da16xxx_usrcfg);
    if(FSP_SUCCESS != result) {
        log_error("MQTT Open failed");
        utils_halt_func();
    }

    log_info("MQTT setup successful!");

    vTaskDelay(100);

    /* Subscribe to topics */
    char g_led_topic[WIDTH_64] = IO_USERNAME;
    strcat(g_led_topic, *pSubTopics);
    subTopics[0].qos = MQTT_ONCHIP_DA16XXX_QOS_0;
    subTopics[0].p_topic_filter = g_led_topic;
    subTopics[0].topic_filter_length= (uint16_t)strlen(g_led_topic);
    /* Subscribe to MQTT topics to be received */
    result = RM_MQTT_DA16XXX_Subscribe(&g_rm_mqtt_onchip_da16xxx_instance, subTopics, 1);
    if(FSP_SUCCESS != result) {
        log_error("MQTT Subscribe failed");
        utils_halt_func();
    }

    log_info("MQTT Subscribe to %s", g_led_topic);
    vTaskDelay(100);
    /* Connect to the MQTT Broker */
    uint32_t num_retry = 1;
    for(;;) {
        result = RM_MQTT_DA16XXX_Connect(&g_rm_mqtt_onchip_da16xxx_instance, CONNECT_TIMEOUT);
        if (FSP_SUCCESS == result) break;
        if (num_retry > 0) num_retry--; else {
            log_error("MQTT Connect failed error %d", result);
            utils_halt_func();
        }
    }

    vTaskResume(sensor_thread);
    sm_sensor_data sensor_data, sensor_slots[NUM_SENSORS];
    clear_sensor_slots(sensor_slots);
    uint32_t last_publishing_time = utils_systime_get();
    uint8_t max_sensors_seen = 0;
    while (1) {
        uint8_t index = 0;
        uint8_t num_sensors = 0;
        if (pdTRUE == xQueueReceive(g_sensor_queue, &sensor_data, PUBLISHING_INTERVAL_MS)) {
            // Store the received data into the right slot
            for (index = 0; index < NUM_SENSORS; index++) {
                if (0 == sensor_slots[index].handle.value || sensor_slots[index].handle.value == sensor_data.handle.value) {
                    // Found a slot
                    sensor_slots[index].handle.value = sensor_data.handle.value;
                    sensor_slots[index].data = sensor_data.data;
                    num_sensors = index + 1;
                    break;
                }
            }
        }
        if (index > max_sensors_seen) max_sensors_seen = index;
        if ((0 < num_sensors) && ((NUM_SENSORS) == num_sensors || ((index == max_sensors_seen) && (utils_systime_get() - last_publishing_time >= PUBLISHING_INTERVAL_MS)))) {
            // We have updated all sensor slots or we had a publishing timeout and are going to proceed with the sensors we have
            for (uint32_t index2 = 0; index2 <= index; index2++) {
                // Proceed publishing data from this slot
                sm_scaling scaling = {.divider=1, .multiplier=1,.offset=0};

                sm_get_sensor_scaling(sensor_slots[index2].handle, &scaling);
                sensor_slots[index2].data += scaling.offset;
                sensor_slots[index2].data = (sensor_slots[index2].data * scaling.multiplier * 100) / scaling.divider;
                char pub_message[WIDTH_64];
                char pub_topic[WIDTH_64];
                snprintf(pub_topic, WIDTH_64, IO_USERNAME"/feeds/%s",\
                        sm_get_sensor_path_by_handle(sensor_slots[index2].handle)\
                        );

                snprintf((char*)pub_message, WIDTH_64, "%ld.%02ld",\
                        sensor_slots[index2].data/100, \
                        sensor_slots[index2].data%100);
                mqtt_onchip_da16xxx_pub_info_t pubData;
                pubData.qos = MQTT_ONCHIP_DA16XXX_QOS_0,
                pubData.p_topic_name = pub_topic;
                pubData.topic_name_Length = (uint16_t)strlen(pub_topic);
                pubData.p_payload = pub_message;
                pubData.payload_length = strlen(pubData.p_payload);
                log_debug("Topic: %s data: %s",pub_topic, pub_message);
                /* Publish data to the MQTT Broker */
                if (FSP_SUCCESS != RM_MQTT_DA16XXX_Publish(&g_rm_mqtt_onchip_da16xxx_instance, &pubData)) {
                    log_error("MQTT publish failed");
                    utils_halt_func();
                }
                // The following delay is very important as RM_MQTT_DA16XXX_Publish do not send the data right away
                // and without a delay further publishes will silently fail
                vTaskDelay(100);
            }
            last_publishing_time = utils_systime_get();
            // Clear slots in preparation for another round of data
            clear_sensor_slots(sensor_slots);
        }
        // Check for received messages
        RM_MQTT_DA16XXX_Receive(&g_rm_mqtt_onchip_da16xxx_instance, &g_mqtt_onchip_da16xxx_cfg);        
    }
}
