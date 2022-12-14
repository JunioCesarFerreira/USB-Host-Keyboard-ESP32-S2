/*
 *
 * UsbHost driver
 * Author: Junio C Ferreira <jcf_ssp@hotmail.com>
 * 
 * Based on esp32_s2_usb_host_hid_test, by Jeff Leung <https://github.com/badjeff>
 * 
 */

#ifndef USBHostHid_hpp
#define USBHostHid_hpp

#include <cstring>
#include <queue>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_intr_alloc.h"
#include "usb/usb_host.h"

#ifndef USB_HOST_PRIORITY_TASK
#define USB_HOST_PRIORITY_TASK     2
#endif 

#ifndef USB_HOST_HID_MAX_EP_DEV
#define USB_HOST_HID_MAX_EP_DEV    4
#endif

// see https://www.usb.org/sites/default/files/hid1_11.pdf page 68.
enum HidInterfaceProtocol
{
    USB_HID_NONE,
    USB_HID_KEYBOARD,
    USB_HID_MOUSE
};

typedef void (*usb_data_transfer_cb_t)(usb_transfer_t *transfer);
typedef void (*usb_detected_cb_t)(void);
typedef void (*usb_open_cb_t)(void);
typedef void (*usb_close_cb_t)(void);

void startUsbHostTasks(
    HidInterfaceProtocol interfaceProtocol,
    usb_detected_cb_t detected_cb, 
    usb_open_cb_t open_cb, 
    usb_data_transfer_cb_t transfer_cb, 
    usb_close_cb_t close_cb
    );

#endif
