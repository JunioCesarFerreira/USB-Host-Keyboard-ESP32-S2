#ifndef UsbBarCode_h
#define UsbBarCode_h

#include <Arduino.h>
#include <USBHost.h>

#define USB_BUFFER_LENGTH 64

typedef void (*keyboard_data_received_cb)(void);

class USBKeyboard
{
	public:
		/// @brief start usb tasks.
		/// @param callback callback when data received.
		void begin(keyboard_data_received_cb callback);

		/// @brief get input buffer.
		/// @return pointer to input buffer.
		char* getBuffer();

		/// @brief get input buffer length.
		/// @return input buffer length.
		uint8_t bufferLength();

		/// @brief clear input buffer.
		void clearBuffer();
};

#endif

