/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "r_dtc.h"
#include "r_transfer_api.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "rm_at_transport_da16xxx_uart.h"
#include "rm_at_transport_da16xxx.h"
#include "rm_wifi_da16xxx.h"
#include "r_sci_i2c.h"
#include "r_i2c_master_api.h"
#include "rm_comms_i2c.h"
#include "rm_comms_api.h"
#include "r_ioport.h"
#include "bsp_pin_cfg.h"
FSP_HEADER
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer0;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer0_ctrl;
extern const transfer_cfg_t g_transfer0_cfg;
/** UART on SCI Instance. */
extern const uart_instance_t g_uart0;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart0_ctrl;
extern const uart_cfg_t g_uart0_cfg;
extern const sci_uart_extended_cfg_t g_uart0_cfg_extend;

#ifndef rm_at_transport_da16xxx_uart_callback
void rm_at_transport_da16xxx_uart_callback(uart_callback_args_t *p_args);
#endif
extern const char *g_at_transport_da16xxx_uart_cmd_baud;
extern at_transport_da16xxx_instance_ctrl_t g_at_transport_da16xxx_ctrl;
extern at_transport_da16xxx_cfg_t g_at_transport_da16xxx_cfg;
extern const at_transport_da16xxx_instance_t g_at_transport_da16xxx;
extern const wifi_da16xxx_cfg_t g_wifi_da16xxx_cfg;
extern const i2c_master_cfg_t g_i2c0_cfg;
/* I2C on SCI Instance. */
extern const i2c_master_instance_t g_i2c0;
#ifndef rm_comms_i2c_callback
void rm_comms_i2c_callback(i2c_master_callback_args_t *p_args);
#endif

extern const sci_i2c_extended_cfg_t g_i2c0_cfg_extend;
extern sci_i2c_instance_ctrl_t g_i2c0_ctrl;
/* I2C Shared Bus */
extern rm_comms_i2c_bus_extended_cfg_t g_comms_i2c_bus0_extended_cfg;
#define IOPORT_CFG_NAME g_bsp_pin_cfg
#define IOPORT_CFG_OPEN R_IOPORT_Open
#define IOPORT_CFG_CTRL g_ioport_ctrl

/* IOPORT Instance */
extern const ioport_instance_t g_ioport;

/* IOPORT control structure. */
extern ioport_instance_ctrl_t g_ioport_ctrl;
void g_common_init(void);
FSP_FOOTER
#endif /* COMMON_DATA_H_ */
