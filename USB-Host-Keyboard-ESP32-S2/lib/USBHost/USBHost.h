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
#include "esp_intr_alloc.h"
#include "usb/usb_host.h"

#ifndef USB_HOST_PRIORITY_TASK
#define USB_HOST_PRIORITY_TASK     2
#endif 

typedef void (*usb_data_transfer_cb_t)(usb_transfer_t *transfer);

void startUsbHostTasks(usb_data_transfer_cb_t callback);

#endif
