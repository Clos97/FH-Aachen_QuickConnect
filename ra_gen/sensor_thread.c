/* generated thread source file - do not edit */
#include "sensor_thread.h"

#if 1
static StaticTask_t sensor_thread_memory;
#if defined(__ARMCC_VERSION)           /* AC6 compiler */
                static uint8_t sensor_thread_stack[1536] BSP_PLACE_IN_SECTION(BSP_UNINIT_SECTION_PREFIX ".stack.thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
                #else
static uint8_t sensor_thread_stack[1536] BSP_PLACE_IN_SECTION(BSP_UNINIT_SECTION_PREFIX ".stack.sensor_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
#endif
#endif
TaskHandle_t sensor_thread;
void sensor_thread_create(void);
static void sensor_thread_func(void *pvParameters);
void rtos_startup_err_callback(void *p_instance, void *p_data);
void rtos_startup_common_init(void);
/* I2C Communication Device */
#define RA_NOT_DEFINED (1)
rm_comms_i2c_instance_ctrl_t g_comms_i2c_device1_ctrl;

/* Lower level driver configuration */
const i2c_master_cfg_t g_comms_i2c_device1_lower_level_cfg = { .slave = 0x32,
		.addr_mode = I2C_MASTER_ADDR_MODE_7BIT, .p_callback =
				rm_comms_i2c_callback, };

const rm_comms_cfg_t g_comms_i2c_device1_cfg = {
		.semaphore_timeout = 0xFFFFFFFF, .p_lower_level_cfg =
				(void*) &g_comms_i2c_device1_lower_level_cfg, .p_extend =
				(void*) &g_comms_i2c_bus0_extended_cfg, .p_callback =
				rm_zmod4xxx_comms_i2c_callback,
#if defined(g_zmod4xxx_sensor0_ctrl)
    .p_context          = g_zmod4xxx_sensor0_ctrl,
#else
		.p_context = (void*) &g_zmod4xxx_sensor0_ctrl,
#endif
		};

const rm_comms_instance_t g_comms_i2c_device1 = { .p_ctrl =
		&g_comms_i2c_device1_ctrl, .p_cfg = &g_comms_i2c_device1_cfg, .p_api =
		&g_comms_on_comms_i2c, };
zmod4xxx_dev_t g_zmod4xxx_sensor0_dev;
iaq_2nd_gen_handle_t g_zmod4xxx_sensor0_lib_handle;
iaq_2nd_gen_results_t g_zmod4xxx_sensor0_lib_results;
uint8_t g_zmod4xxx_sensor0_product_data[7];
extern rm_zmod4xxx_api_t const g_zmod4xxx_on_zmod4410_iaq_2nd_gen;
extern zmod4xxx_conf g_zmod4410_iaq_2nd_gen_sensor_type[];
rm_zmod4xxx_lib_extended_cfg_t g_zmod4xxx_sensor0_extended_cfg = { .lib_type =
		RM_ZMOD4410_LIB_TYPE_IAQ_2ND_GEN, .product_id = 0x2310, .p_api =
		(void*) &g_zmod4xxx_on_zmod4410_iaq_2nd_gen, .p_data_set =
		(void*) g_zmod4410_iaq_2nd_gen_sensor_type, .p_product_data =
		g_zmod4xxx_sensor0_product_data, .p_device =
		(void*) &g_zmod4xxx_sensor0_dev, .p_handle =
		(void*) &g_zmod4xxx_sensor0_lib_handle, .p_results =
		(void*) &g_zmod4xxx_sensor0_lib_results, };
rm_zmod4xxx_instance_ctrl_t g_zmod4xxx_sensor0_ctrl;
const rm_zmod4xxx_cfg_t g_zmod4xxx_sensor0_cfg = { .p_comms_instance =
		&g_comms_i2c_device1,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
		.p_irq_instance = NULL, .p_irq_callback = NULL,
#else
    .p_irq_instance = &RA_NOT_DEFINED,
    .p_irq_callback = zmod4xxx_irq_callback,
#endif
#undef RA_NOT_DEFINED
		.p_comms_callback = zmod4xxx_comms_i2c_callback,
#if defined(NULL)
    .p_context           = NULL,
#else
		.p_context = &NULL,
#endif
		.p_extend = (void*) &g_zmod4xxx_sensor0_extended_cfg, };

const rm_zmod4xxx_instance_t g_zmod4xxx_sensor0 = { .p_ctrl =
		&g_zmod4xxx_sensor0_ctrl, .p_cfg = &g_zmod4xxx_sensor0_cfg, .p_api =
		&g_zmod4xxx_on_zmod4xxx, };
/* I2C Communication Device */
#define RA_NOT_DEFINED (1)
rm_comms_i2c_instance_ctrl_t g_comms_i2c_device0_ctrl;

/* Lower level driver configuration */
const i2c_master_cfg_t g_comms_i2c_device0_lower_level_cfg = { .slave = 0x54,
		.addr_mode = I2C_MASTER_ADDR_MODE_7BIT, .p_callback =
				rm_comms_i2c_callback, };

const rm_comms_cfg_t g_comms_i2c_device0_cfg = {
		.semaphore_timeout = 0xFFFFFFFF, .p_lower_level_cfg =
				(void*) &g_comms_i2c_device0_lower_level_cfg, .p_extend =
				(void*) &g_comms_i2c_bus0_extended_cfg, .p_callback =
				rm_hs400x_comms_i2c_callback,
#if defined(g_hs400x_sensor0_ctrl)
    .p_context          = g_hs400x_sensor0_ctrl,
#else
		.p_context = (void*) &g_hs400x_sensor0_ctrl,
#endif
		};

const rm_comms_instance_t g_comms_i2c_device0 = { .p_ctrl =
		&g_comms_i2c_device0_ctrl, .p_cfg = &g_comms_i2c_device0_cfg, .p_api =
		&g_comms_on_comms_i2c, };
rm_hs400x_instance_ctrl_t g_hs400x_sensor0_ctrl;
const rm_hs400x_cfg_t g_hs400x_sensor0_cfg = { .p_comms_instance =
		&g_comms_i2c_device0, .temperature_resolution =
		RM_HS400X_TEMPERATURE_RESOLUTION_14BIT, .humidity_resolution =
		RM_HS400X_HUMIDITY_RESOLUTION_14BIT,
#if 3 == RM_HS400X_CFG_MEASUREMENT_TYPE // Periodic Measurement
    .frequency = RM_HS400X_PERIODIC_MEASUREMENT_FREQUENCY_1HZ,
#endif
		.p_comms_callback = hs400x_comms_i2c_callback,
#if defined(NULL)
    .p_context           = NULL,
#else
		.p_context = &NULL,
#endif
		};

const rm_hs400x_instance_t g_hs400x_sensor0 = {
		.p_ctrl = &g_hs400x_sensor0_ctrl, .p_cfg = &g_hs400x_sensor0_cfg,
		.p_api = &g_hs400x_on_hs400x, };
extern uint32_t g_fsp_common_thread_count;

const rm_freertos_port_parameters_t sensor_thread_parameters = { .p_context =
		(void*) NULL, };

void sensor_thread_create(void) {
	/* Increment count so we will know the number of threads created in the RA Configuration editor. */
	g_fsp_common_thread_count++;

	/* Initialize each kernel object. */

#if 1
	sensor_thread = xTaskCreateStatic(
#else
                    BaseType_t sensor_thread_create_err = xTaskCreate(
                    #endif
			sensor_thread_func, (const char*) "Sensor", 1536 / 4, // In words, not bytes
			(void*) &sensor_thread_parameters, //pvParameters
			2,
#if 1
			(StackType_t*) &sensor_thread_stack,
			(StaticTask_t*) &sensor_thread_memory
#else
                        & sensor_thread
                        #endif
			);

#if 1
	if (NULL == sensor_thread) {
		rtos_startup_err_callback(sensor_thread, 0);
	}
#else
                    if (pdPASS != sensor_thread_create_err)
                    {
                        rtos_startup_err_callback(sensor_thread, 0);
                    }
                    #endif
}
static void sensor_thread_func(void *pvParameters) {
	/* Initialize common components */
	rtos_startup_common_init();

	/* Initialize each module instance. */

#if (1 == BSP_TZ_NONSECURE_BUILD) && (1 == 1)
                    /* When FreeRTOS is used in a non-secure TrustZone application, portALLOCATE_SECURE_CONTEXT must be called prior
                     * to calling any non-secure callable function in a thread. The parameter is unused in the FSP implementation.
                     * If no slots are available then configASSERT() will be called from vPortSVCHandler_C(). If this occurs, the
                     * application will need to either increase the value of the "Process Stack Slots" Property in the rm_tz_context
                     * module in the secure project or decrease the number of threads in the non-secure project that are allocating
                     * a secure context. Users can control which threads allocate a secure context via the Properties tab when
                     * selecting each thread. Note that the idle thread in FreeRTOS requires a secure context so the application
                     * will need at least 1 secure context even if no user threads make secure calls. */
                     portALLOCATE_SECURE_CONTEXT(0);
                    #endif

	/* Enter user code for this thread. Pass task handle. */
	sensor_thread_entry(pvParameters);
}
