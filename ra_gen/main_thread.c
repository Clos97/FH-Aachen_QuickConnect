/* generated thread source file - do not edit */
#include "main_thread.h"

#if 1
static StaticTask_t main_thread_memory;
#if defined(__ARMCC_VERSION)           /* AC6 compiler */
                static uint8_t main_thread_stack[2048] BSP_PLACE_IN_SECTION(BSP_UNINIT_SECTION_PREFIX ".stack.thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
                #else
static uint8_t main_thread_stack[2048] BSP_PLACE_IN_SECTION(BSP_UNINIT_SECTION_PREFIX ".stack.main_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
#endif
#endif
TaskHandle_t main_thread;
void main_thread_create(void);
static void main_thread_func(void *pvParameters);
void rtos_startup_err_callback(void *p_instance, void *p_data);
void rtos_startup_common_init(void);
#include "certificate.h"
#define CERT_INCLUDE

/** MQTT Onchip da16xxx configuration */
const mqtt_onchip_da16xxx_cfg_t g_mqtt_onchip_da16xxx_cfg = {
#define RA_NOT_DEFINED (1)

		.use_mqtt_v311 = (1), .rx_timeout = (10), .tx_timeout = (10),
		.p_callback = mqtt0_callback, .clean_session = (1), .alpn_count = (0),
		.p_alpns = { NULL, NULL, NULL }, .tls_cipher_count = (0),
		.keep_alive_seconds = (60), .p_client_identifier = NULL,
		.client_identifier_length = (0), .p_host_name = NULL, .mqtt_port =
				(8883), .p_mqtt_user_name = NULL, .user_name_length = (0),
		.p_mqtt_password = NULL, .password_length = (0),
#ifdef CERT_INCLUDE
		.p_root_ca = ROOT_CA, .root_ca_size = (sizeof(ROOT_CA)),
		.p_client_cert = CLIENT_CERT, .client_cert_size = (sizeof(CLIENT_CERT)),
		.p_client_private_key = PRIVATE_KEY, .private_key_size =
				(sizeof(PRIVATE_KEY)),
#else
                        .p_root_ca =                NULL,
                        .root_ca_size =             0,
                        .p_client_cert =            NULL,
                        .client_cert_size =         0,
                        .p_client_private_key =     NULL,
                        .private_key_size =         0,
#endif
		.p_will_topic = NULL, .p_will_msg = NULL, .p_sni_name = NULL,
		.will_qos_level = (MQTT_ONCHIP_DA16XXX_QOS_0), .p_tls_cipher_suites = {
				0 }, .p_transport_instance = &g_at_transport_da16xxx, };
extern uint32_t g_fsp_common_thread_count;

const rm_freertos_port_parameters_t main_thread_parameters = { .p_context =
		(void*) NULL, };

void main_thread_create(void) {
	/* Increment count so we will know the number of threads created in the RA Configuration editor. */
	g_fsp_common_thread_count++;

	/* Initialize each kernel object. */

#if 1
	main_thread = xTaskCreateStatic(
#else
                    BaseType_t main_thread_create_err = xTaskCreate(
                    #endif
			main_thread_func, (const char*) "Main", 2048 / 4, // In words, not bytes
			(void*) &main_thread_parameters, //pvParameters
			1,
#if 1
			(StackType_t*) &main_thread_stack,
			(StaticTask_t*) &main_thread_memory
#else
                        & main_thread
                        #endif
			);

#if 1
	if (NULL == main_thread) {
		rtos_startup_err_callback(main_thread, 0);
	}
#else
                    if (pdPASS != main_thread_create_err)
                    {
                        rtos_startup_err_callback(main_thread, 0);
                    }
                    #endif
}
static void main_thread_func(void *pvParameters) {
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
	main_thread_entry(pvParameters);
}
