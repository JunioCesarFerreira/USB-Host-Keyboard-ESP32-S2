// this library was based on the links repository and documentation:
// https://github.com/badjeff/esp32_s2_usb_host_hid_test
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/peripherals/usb_host.html

#ifndef USBHost_hpp
#define USBHost_hpp

#include <cstring>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "usb/usb_host.h"

#define CLIENT_NUM_EVENT_MSG       5

#define ACTION_OPEN_DEV            0x01
#define ACTION_GET_DEV_INFO        0x02
#define ACTION_GET_DEV_DESC        0x04
#define ACTION_GET_CONFIG_DESC     0x08
#define ACTION_GET_STR_DESC        0x10
#define ACTION_CLOSE_DEV           0x20
#define ACTION_EXIT                0x40
#define ACTION_CLAIM_INTF          0x0100
#define ACTION_TRANSFER_CONTROL    0x0200
#define ACTION_TRANSFER            0x0400

#define USB_HOST_PRIORITY_TASK     2

static const char *TAG_DAEMON = "DAEMON";
static const char *TAG_CLASS = "CLASS";

typedef struct 
{
	usb_host_client_handle_t client_hdl;
	uint8_t dev_addr;
	usb_device_handle_t dev_hdl;
	uint32_t actions;
	uint16_t bMaxPacketSize0;
	usb_ep_desc_t *ep_in;
	usb_ep_desc_t *ep_out;
	SemaphoreHandle_t transfer_done;
	usb_transfer_status_t transfer_status;
} class_driver_t;

typedef void (*usb_data_transfer_cb_t)(usb_transfer_t *transfer);

void startUsbHostTasks(usb_data_transfer_cb_t callback);

#endif
