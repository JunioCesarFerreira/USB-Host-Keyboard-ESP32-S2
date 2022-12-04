#ifndef UsbKeyboard_h
#define UsbKeyboard_h

#include <Arduino.h>
#include <USBHost.h>

#define USB_KEYBOARD_BUFFER_LENGTH 64

#define KEYBOARD_RIGHT_ARROW 1
#define KEYBOARD_LEFT_ARROW  2
#define KEYBOARD_DOWN_ARROW  3 
#define KEYBOARD_UP_ARROW    4

enum KeyboardAction
{
	KEYBOARD_OPENING,
	KEYBOARD_OPENED,
	KEYBOARD_TRANSFER,
	KEYBOARD_CLOSE
};

typedef void (*keyboard_data_received_cb)(KeyboardAction);

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

