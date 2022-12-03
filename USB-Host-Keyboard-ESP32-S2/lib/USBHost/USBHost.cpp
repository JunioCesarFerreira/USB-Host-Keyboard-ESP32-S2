#include "USBHost.h"

static usb_data_transfer_cb_t usb_data_transfer_cb;

// Tasks
static void usb_host_lib_daemon_task(void *arg);
static void usb_class_driver_task(void *arg);
static void usb_host_task(void* args);

/// @brief Inicializa tasks USB host.
/// @param callback Evento de recebimento de dados.
void startUsbHostTasks(usb_data_transfer_cb_t callback)
{
	usb_data_transfer_cb = callback;
	
	xTaskCreatePinnedToCore(usb_host_task,
							"usb_host_task",
							8192,
							NULL,
							USB_HOST_PRIORITY_TASK,
							NULL,
							0);
}

/// @brief Tarefa que perciste a reconexão USB após desconecção do cabo.
static void usb_host_task(void* args)
{
	while (true)
	{
		SemaphoreHandle_t signaling_sem = xSemaphoreCreateBinary();

		// Create usb host lib daemon task
		TaskHandle_t daemon_task_hdl;
		xTaskCreatePinnedToCore(usb_host_lib_daemon_task,
							"usb_host_lib_daemon_task",
							4096,
							(void *)signaling_sem,
							USB_HOST_PRIORITY_TASK-1,
							&daemon_task_hdl,
							0);

		// Create usb class driver task
		TaskHandle_t class_driver_task_hdl;
		xTaskCreatePinnedToCore(usb_class_driver_task,
							"usb_class_driver_task",
							4096,
							(void *)signaling_sem,
							USB_HOST_PRIORITY_TASK,
							&class_driver_task_hdl,
							0);

		vTaskDelay(10); 

		// Wait for the tasks to complete
		for (int i = 0; i < 2; i++) 
		{
			xSemaphoreTake(signaling_sem, portMAX_DELAY);
		}

		// Delete the tasks
		vTaskDelete(class_driver_task_hdl);
		vTaskDelete(daemon_task_hdl);

		printf("\n[usbHost]: *** loop usb task ***\n\n");

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

static void usb_host_lib_daemon_task(void *arg)
{
	SemaphoreHandle_t signaling_sem = (SemaphoreHandle_t)arg;

	printf("[usbHost]:Installing USB Host Library... ");
	usb_host_config_t host_config = {
		.skip_phy_setup = false,
		.intr_flags = ESP_INTR_FLAG_LEVEL1
	};
	ESP_ERROR_CHECK(usb_host_install(&host_config));
	printf("installed.\n");

	//Signal to the class driver task that the host library is installed
	xSemaphoreGive(signaling_sem);
	vTaskDelay(pdMS_TO_TICKS(10)); //Short delay to let client task spin up

	bool has_clients = true;
	bool has_devices = true;
	while (has_clients || has_devices ) 
	{
		uint32_t event_flags;
		ESP_ERROR_CHECK(usb_host_lib_handle_events(portMAX_DELAY, &event_flags));
		if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) 
		{
			has_clients = false;
		}
		if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) 
		{
			has_devices = false;
		}
	}
	printf("[usbHost]:No more clients and devices\n");

	vTaskDelay(pdMS_TO_TICKS(100));
	//Uninstall the USB Host Library
	ESP_ERROR_CHECK(usb_host_uninstall());
	//Wait to be deleted
	xSemaphoreGive(signaling_sem);
	vTaskSuspend(NULL);
}

static void client_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
{
	class_driver_t *driver_obj = (class_driver_t *)arg;
	switch (event_msg->event) 
	{
		case USB_HOST_CLIENT_EVENT_NEW_DEV:
			if (driver_obj->dev_addr == 0) 
			{
				driver_obj->dev_addr = event_msg->new_dev.address;
				//Open the device next
				driver_obj->actions |= ACTION_OPEN_DEV;
			}
			break;
		case USB_HOST_CLIENT_EVENT_DEV_GONE:
			if (driver_obj->dev_hdl != NULL) 
			{
				//Cancel any other actions and close the device next
				driver_obj->actions = ACTION_CLOSE_DEV;
			}
			break;
		default:
			//Should never occur
			abort();
	}
}

static void action_open_dev(class_driver_t *driver_obj)
{
	assert(driver_obj->dev_addr != 0);
	printf("[usbHost]:Opening device at address %d\n", driver_obj->dev_addr);
	ESP_ERROR_CHECK(usb_host_device_open(driver_obj->client_hdl, driver_obj->dev_addr, &driver_obj->dev_hdl));
	
	//Get the device's information next
	driver_obj->actions &= ~ACTION_OPEN_DEV;
	driver_obj->actions |= ACTION_GET_DEV_INFO;
}

static void action_get_info(class_driver_t *driver_obj)
{
	assert(driver_obj->dev_hdl != NULL);
	usb_device_info_t dev_info;
	ESP_ERROR_CHECK(usb_host_device_info(driver_obj->dev_hdl, &dev_info));
	
		printf("[usbHost]:Getting device information\n");
		printf("[usbHost]:\t%s speed\n", (dev_info.speed == USB_SPEED_LOW) ? "Low" : "Full");
		printf("[usbHost]:\tbConfigurationValue %d\n", dev_info.bConfigurationValue);
	
	//Get the device descriptor next
	driver_obj->actions &= ~ACTION_GET_DEV_INFO;
	driver_obj->actions |= ACTION_GET_DEV_DESC;
}

static void action_get_dev_desc(class_driver_t *driver_obj)
{
	assert(driver_obj->dev_hdl != NULL);
	const usb_device_desc_t *dev_desc;
	ESP_ERROR_CHECK(usb_host_get_device_descriptor(driver_obj->dev_hdl, &dev_desc));
   
		printf("[usbHost]:Getting device descriptor\n");
		printf("[usbHost]:\tidVendor 0x%04x\n", dev_desc->idVendor);
		printf("[usbHost]:\tidProduct 0x%04x\n", dev_desc->idProduct);
	
	usb_print_device_descriptor(dev_desc);

	driver_obj->bMaxPacketSize0 = dev_desc->bMaxPacketSize0;

	//Get the device's config descriptor next
	driver_obj->actions &= ~ACTION_GET_DEV_DESC;
	driver_obj->actions |= ACTION_GET_CONFIG_DESC;
}

static void action_get_config_desc(class_driver_t *driver_obj)
{
	assert(driver_obj->dev_hdl != NULL);
	printf("[usbHost]:Getting config descriptor\n");
	const usb_config_desc_t *config_desc;
	ESP_ERROR_CHECK(usb_host_get_active_config_descriptor(driver_obj->dev_hdl, &config_desc));
	usb_print_config_descriptor(config_desc, NULL);

	//Get the device's string descriptors next
	driver_obj->actions &= ~ACTION_GET_CONFIG_DESC;
	driver_obj->actions |= ACTION_GET_STR_DESC;
}

static void action_get_str_desc(class_driver_t *driver_obj)
{
	assert(driver_obj->dev_hdl != NULL);
	usb_device_info_t dev_info;
	ESP_ERROR_CHECK(usb_host_device_info(driver_obj->dev_hdl, &dev_info));
	if (dev_info.str_desc_manufacturer) 
	{
		printf("[usbHost]:Getting Manufacturer string descriptor\n");
		usb_print_string_descriptor(dev_info.str_desc_manufacturer);
	}
	if (dev_info.str_desc_product) 
	{
		printf("[usbHost]:Getting Product string descriptor\n");
		usb_print_string_descriptor(dev_info.str_desc_product);
	}
	if (dev_info.str_desc_serial_num) 
	{
		printf("[usbHost]:Getting Serial Number string descriptor\n");
		usb_print_string_descriptor(dev_info.str_desc_serial_num);
	}

	//Claim the interface next
	driver_obj->actions &= ~ACTION_GET_STR_DESC;
	driver_obj->actions |= ACTION_CLAIM_INTF;
}

static void action_claim_interface(class_driver_t *driver_obj)
{
	assert(driver_obj->dev_hdl != NULL);
	printf("[usbHost]:Getting config descriptor\n");
	const usb_config_desc_t *config_desc;
	ESP_ERROR_CHECK(usb_host_get_active_config_descriptor(driver_obj->dev_hdl, &config_desc));

	bool hidIntfClaimed = false;
	int offset = 0;

	for (size_t n = 0; n < config_desc->bNumInterfaces; n++)
	{
		const usb_intf_desc_t *intf = usb_parse_interface_descriptor(config_desc, n, 0, &offset);
		printf("[usbHost]:Parsed intf->bInterfaceNumber: 0x%02x \n", intf->bInterfaceNumber);
		
		if (intf->bInterfaceClass == 0x03) // HID - https://www.usb.org/defined-class-codes
		{
			printf("[usbHost]:Detected HID intf->bInterfaceClass: 0x%02x \n", intf->bInterfaceClass);

			const usb_ep_desc_t *ep_in = nullptr;
			const usb_ep_desc_t *ep_out = nullptr;
			const usb_ep_desc_t *ep = nullptr;
			for (size_t i = 0; i < intf->bNumEndpoints; i++) 
			{
				int _offset = 0;
				ep = usb_parse_endpoint_descriptor_by_index(intf, i, config_desc->wTotalLength, &_offset);
				printf("[usbHost]:\t > Detected EP num: %d/%d, len: %d, ", i + 1, intf->bNumEndpoints, config_desc->wTotalLength);
				if (ep) 
				{
					printf("[usbHost]:address: 0x%02x, mps: %d, dir: %s\n", ep->bEndpointAddress, ep->wMaxPacketSize, (ep->bEndpointAddress & 0x80) ? "IN" : "OUT");
					if (ep->bmAttributes != USB_TRANSFER_TYPE_INTR) 
					{
						// only support INTERRUPT > IN Report in action_transfer() for now
						continue;
					}
					if (ep->bEndpointAddress & 0x80) 
					{
						ep_in = ep;
						if (ep->wMaxPacketSize == 8) // this needs to be improved.
						{
							driver_obj->ep_in = (usb_ep_desc_t *)ep_in;
						}
					} 
					else 
					{
						ep_out = ep;
						driver_obj->ep_out = (usb_ep_desc_t *)ep_out;
					}
				}
				else 
				{
					printf("[usbHost]:error to parse endpoint by index; EP num: %d/%d, len: %d", i + 1, intf->bNumEndpoints, config_desc->wTotalLength);
				}
			}
			esp_err_t err = usb_host_interface_claim(driver_obj->client_hdl, driver_obj->dev_hdl, n, 0);
			if (err) 
			{
				printf("[usbHost]:error interface claim status: %d", err);
			} 
			else 
			{
				printf("[usbHost]:Claimed HID intf->bInterfaceNumber: 0x%02x \n", intf->bInterfaceNumber);
				hidIntfClaimed = true;
			}
		}
	}

	//Get the HID's descriptors next
	driver_obj->actions &= ~ACTION_CLAIM_INTF;
	if (hidIntfClaimed)
	{
		driver_obj->actions |= ACTION_TRANSFER_CONTROL;
	}
}

static void transfer_cb(usb_transfer_t *transfer)
{
	class_driver_t *driver_obj = (class_driver_t *)transfer->context;
	driver_obj->transfer_status = transfer->status;
	xSemaphoreGive(driver_obj->transfer_done);
}

static esp_err_t wait_for_transfer_done(usb_transfer_t *transfer)
{
	class_driver_t *driver_obj = (class_driver_t *)transfer->context;
	BaseType_t received = xSemaphoreTake(driver_obj->transfer_done, pdMS_TO_TICKS(transfer->timeout_ms));
	if (received != pdTRUE) 
	{
		xSemaphoreGive(driver_obj->transfer_done);
		return ESP_ERR_TIMEOUT;
	}
	xSemaphoreGive(driver_obj->transfer_done);
	return (driver_obj->transfer_status == USB_TRANSFER_STATUS_COMPLETED) ? ESP_OK : ESP_FAIL;
}

static void action_transfer_control(class_driver_t *driver_obj)
{
	assert(driver_obj->dev_hdl != NULL);
	static uint16_t mps = driver_obj->bMaxPacketSize0;
	static uint16_t tps = usb_round_up_to_mps(1024, mps);
	static usb_transfer_t *transfer;
	if (!transfer) 
	{
		usb_host_transfer_alloc(tps, 0, &transfer);
	}
	static int32_t lastSendTime = 0;
	if (xTaskGetTickCount() - lastSendTime > 8)
	{
		usb_setup_packet_t stp;

		memcpy(transfer->data_buffer, &stp, USB_SETUP_PACKET_SIZE);
		
			printf("[usbHost]:transfer->data_buffer:");
			for(int i=0; i < 8; i++) printf(" %02X", transfer->data_buffer[i]);
			printf("\n");
			printf("[usbHost]:transfer->data_buffer_size: %d\n", transfer->data_buffer_size);
			printf("[usbHost]:transfer->num_bytes: %d\n", transfer->num_bytes);
		
		transfer->bEndpointAddress = 0x00;
		transfer->device_handle = driver_obj->dev_hdl;
		transfer->callback = transfer_cb;
		transfer->context = (void *)driver_obj;
		transfer->timeout_ms = 5000;

		BaseType_t received = xSemaphoreTake(driver_obj->transfer_done, 100);
		if (received == pdTRUE) 
		{
			usb_host_transfer_submit_control(driver_obj->client_hdl, transfer);
			usb_host_client_handle_events(driver_obj->client_hdl, 100); // for raising transfer->callback
			wait_for_transfer_done(transfer);
			if (transfer->status != USB_TRANSFER_STATUS_COMPLETED) 
			{
				printf("[usbHost]:Transfer control failed - Status %d \n", transfer->status);
			}
		}

		driver_obj->actions &= ~ACTION_TRANSFER_CONTROL;
		if (transfer->status == USB_TRANSFER_STATUS_COMPLETED) 
		{
			driver_obj->actions |= ACTION_TRANSFER;

				printf("[usbHost]:usb_host_transfer_submit_control - completed \n");
				//>>>>> for HID Report Descriptor
				// Explanation: https://electronics.stackexchange.com/questions/68141/
				// USB Descriptor and Request Parser: https://eleccelerator.com/usbdescreqparser/#
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/usbcon/usb-control-transfer
				//<<<<<				
				printf("[usbHost]:\nstatus %d, actual number of bytes transferred %d\n", transfer->status, transfer->actual_num_bytes);
				for(int i=0; i < transfer->actual_num_bytes; i++) 
				{
					if (i == 8) 
					{
						printf("[usbHost]:>>> Goto https://eleccelerator.com/usbdescreqparser/ \n");
						printf("[usbHost]:>>> Copy & paste below HEX and parser as... USB HID Report Descriptor\n\n");
					}
					printf("%02X ", transfer->data_buffer[i]);
				}
				printf("\n\n");
				uint8_t *const data = (uint8_t *const)(transfer->data_buffer + USB_SETUP_PACKET_SIZE);
				size_t len = transfer->actual_num_bytes - 8;
				printf("[usbHost]:HID Report Descriptor\n");
				printf("[usbHost]:> size: %ld bytes\n", len);
				
		}

		vTaskDelay(1); //Short delay to let client task spin up
		lastSendTime = xTaskGetTickCount();
	}
}

static void action_transfer(class_driver_t *driver_obj)
{
	assert(driver_obj->dev_hdl != NULL);
	static uint16_t mps = driver_obj->ep_in->wMaxPacketSize;
	static usb_transfer_t *transfer;
	if (!transfer) 
	{
		usb_host_transfer_alloc(mps, 0, &transfer);
	}
	
	static int32_t lastSendTime = 0;
	if (xTaskGetTickCount() - lastSendTime > 16)
	{
		transfer->num_bytes = mps;
		memset(transfer->data_buffer, 0x00, mps);

		transfer->bEndpointAddress = driver_obj->ep_in->bEndpointAddress;

		transfer->device_handle = driver_obj->dev_hdl;
		transfer->callback = transfer_cb;
		transfer->context = (void *)driver_obj;

		transfer->timeout_ms = 30;

		BaseType_t received = xSemaphoreTake(driver_obj->transfer_done, pdMS_TO_TICKS(10));
		esp_err_t received_status;
		if (received == pdTRUE) 
		{
			usb_host_transfer_submit(transfer);
			usb_host_client_handle_events(driver_obj->client_hdl, 100); // for raising transfer->callback
			received_status = wait_for_transfer_done(transfer);
			if (transfer->status != USB_TRANSFER_STATUS_COMPLETED) 
			{
				printf("[usbHost]:Transfer failed - Status %d \n", transfer->status);
				vTaskDelay(10);
				lastSendTime = xTaskGetTickCount();
			}
		}

		if (transfer->status == USB_TRANSFER_STATUS_COMPLETED) 
		{
			if (transfer->actual_num_bytes > 0) 
			{
				usb_data_transfer_cb(transfer);
			}
		}

		if (received_status == ESP_OK)
		{
			vTaskDelay(1); //Short delay to let client task spin up
			lastSendTime = xTaskGetTickCount();
		}
	}
}

static void aciton_close_dev(class_driver_t *driver_obj)
{
	const usb_config_desc_t *config_desc;
	ESP_ERROR_CHECK(usb_host_get_active_config_descriptor(driver_obj->dev_hdl, &config_desc));
	
	int offset = 0;
	for (size_t n = 0; n < config_desc->bNumInterfaces; n++)
	{
		const usb_intf_desc_t *intf = usb_parse_interface_descriptor(config_desc, n, 0, &offset);
		printf("[usbHost]:Releasing intf->bInterfaceNumber: 0x%02x \n", intf->bInterfaceNumber);
		if (intf->bInterfaceClass == 0x03) // HID - https://www.usb.org/defined-class-codes
		{
			printf("[usbHost]:Releasing HID intf->bInterfaceClass: 0x%02x \n", intf->bInterfaceClass);
			const usb_ep_desc_t *ep_in = nullptr;
			const usb_ep_desc_t *ep_out = nullptr;
			const usb_ep_desc_t *ep = nullptr;
			for (size_t i = 0; i < intf->bNumEndpoints; i++) 
			{
				int _offset = 0;
				ep = usb_parse_endpoint_descriptor_by_index(intf, i, config_desc->wTotalLength, &_offset);
				if (ep) 
				{
					if (ep->bEndpointAddress & 0x80) 
					{
						ep_in = ep;
					} 
					else 
					{
						ep_out = ep;
					}
					
					printf("[usbHost]:\t > Halting EP num: %d/%d, len: %d, ", i + 1, intf->bNumEndpoints, config_desc->wTotalLength);
					printf("address: 0x%02x, EP max size: %d, dir: %s\n", ep->bEndpointAddress, ep->wMaxPacketSize, (ep->bEndpointAddress & 0x80) ? "IN" : "OUT");
					
					ESP_ERROR_CHECK(usb_host_endpoint_halt(driver_obj->dev_hdl, ep->bEndpointAddress));
					ESP_ERROR_CHECK(usb_host_endpoint_flush(driver_obj->dev_hdl, ep->bEndpointAddress));
				}
			}
			ESP_ERROR_CHECK(usb_host_interface_release(driver_obj->client_hdl, driver_obj->dev_hdl, n));
		}
	}

	ESP_ERROR_CHECK(usb_host_device_close(driver_obj->client_hdl, driver_obj->dev_hdl));
	driver_obj->dev_hdl = NULL;
	driver_obj->dev_addr = 0;
	//We need to exit the event handler loop
	driver_obj->actions &= ~ACTION_CLOSE_DEV;
	driver_obj->actions &= ~ACTION_TRANSFER;
	driver_obj->actions |= ACTION_EXIT;
}

/// @brief Driver de protocolo USB HID.
/// @param arg Semáforo USB.
static void usb_class_driver_task(void *arg)
{
	SemaphoreHandle_t signaling_sem = (SemaphoreHandle_t)arg;
	class_driver_t driver_obj = {0};

	// Wait until daemon task has installed USB Host Library
	xSemaphoreTake(signaling_sem, portMAX_DELAY);

	printf("[usbHost]:Registering Client");
	usb_host_client_config_t client_config = 
	{
		.is_synchronous = false, // Synchronous clients currently not supported. Set this to false
		.max_num_event_msg = CLIENT_NUM_EVENT_MSG,
		.async = {
			.client_event_callback = client_event_cb,
			.callback_arg = (void *)&driver_obj,
		},
	};
	ESP_ERROR_CHECK(usb_host_client_register(&client_config, &driver_obj.client_hdl));

	driver_obj.transfer_done = xSemaphoreCreateCounting( 1, 1 );

	while (true) 
	{
		if (driver_obj.actions == 0) 
		{
			usb_host_client_handle_events(driver_obj.client_hdl, portMAX_DELAY);
		} 
		else 
		{
			if (driver_obj.actions & ACTION_OPEN_DEV) 
			{
				action_open_dev(&driver_obj);
			}
			if (driver_obj.actions & ACTION_GET_DEV_INFO) 
			{
				action_get_info(&driver_obj);
			}
			if (driver_obj.actions & ACTION_GET_DEV_DESC) 
			{
				action_get_dev_desc(&driver_obj);
			}
			if (driver_obj.actions & ACTION_GET_CONFIG_DESC) 
			{
				action_get_config_desc(&driver_obj);
			}
			if (driver_obj.actions & ACTION_GET_STR_DESC) 
			{
				action_get_str_desc(&driver_obj);
			}
			if (driver_obj.actions & ACTION_CLAIM_INTF) 
			{
				action_claim_interface(&driver_obj);
			}
			if (driver_obj.actions & ACTION_TRANSFER_CONTROL) 
			{
				action_transfer_control(&driver_obj);
			}
			if (driver_obj.actions & ACTION_TRANSFER) 
			{
				action_transfer(&driver_obj);
			}
			if (driver_obj.actions & ACTION_CLOSE_DEV) 
			{
				aciton_close_dev(&driver_obj);
			}
			if (driver_obj.actions & ACTION_EXIT) 
			{
				break;
			}
		}
	}

	vSemaphoreDelete(driver_obj.transfer_done);

	printf("[usbHost]:Deregistering Client");
	ESP_ERROR_CHECK(usb_host_client_deregister(driver_obj.client_hdl));

	// Wait to be deleted
	xSemaphoreGive(signaling_sem);
	vTaskSuspend(NULL);
}
