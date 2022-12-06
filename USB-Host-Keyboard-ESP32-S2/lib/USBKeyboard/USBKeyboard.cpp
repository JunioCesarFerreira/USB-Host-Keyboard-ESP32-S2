#include "USBKeyboard.h"

//#define USB_KEYBOARD_DEBUG // to enable debug log this file.

#ifdef USB_KEYBOARD_DEBUG
#define USB_KEYBOARD_DEBUG_ARGS(string,...) (printf(string,__VA_ARGS__))
#define USB_KEYBOARD_DEBUG_INFO(string) (printf(string))
#else
#define USB_KEYBOARD_DEBUG_ARGS(string,...)
#define USB_KEYBOARD_DEBUG_INFO(string)
#endif

keyboard_data_received_cb usb_keyboard_callback;

char data_received[USB_KEYBOARD_BUFFER_LENGTH]; 
uint8_t data_received_length; 

void usb_data_trasnfer_cb(usb_transfer_t *transfer);
void usb_opening_cb();
void usb_opened_cb();
void usb_close_cb();

void USBKeyboard::begin(keyboard_data_received_cb cb)
{
	usb_keyboard_callback = cb;
	clearBuffer();
	startUsbHostTasks(
		USB_HID_KEYBOARD,
		usb_opening_cb, 
		usb_opened_cb,
		usb_data_trasnfer_cb,
		usb_close_cb
		);
}

char* USBKeyboard::getBuffer()
{
	return &data_received[0];
}

uint8_t USBKeyboard::bufferLength()
{
	return data_received_length;
}

void USBKeyboard::clearBuffer()
{
	for(int i=0;i<USB_KEYBOARD_BUFFER_LENGTH;i++)
	{
		data_received[i]=0;
	}
	data_received_length = 0;
}

/// @brief Keyboard map (based on a brazilian keyboard).
/// @param byte0 Indicate specials keys pressed.
/// @param byte2 Code to keyboard map.
/// @return char ASCII key pressed.
char keyboard_mapping(uint8_t byte0, uint8_t byte2)
{
	USB_KEYBOARD_DEBUG_ARGS("[Keyboard]:Bytes mapping: (0x%02x, 0x%02x)", byte0, byte2);
	uint8_t map_value = 0;
	if (byte0==0 && byte2>=0x4F && byte2<=0x52)
	{
		switch (byte2) // Arrows
		{
			case 0x4F: // right
				map_value = KEYBOARD_RIGHT_ARROW;
				break;
			case 0x50: // left
				map_value = KEYBOARD_LEFT_ARROW;
				break;
			case 0x51: // down
				map_value = KEYBOARD_DOWN_ARROW;
				break;
			case 0x52: // up
				map_value = KEYBOARD_UP_ARROW;
				break;
		}
	}
	else if (byte2==0x2C) // Space
	{
		map_value = ' ';
	}
	else if (byte2==0x2A) // Backspace
	{
		map_value = 8;
	}
	else if (byte2==0x28 || byte2==0x58) // Enter
	{
		map_value = '\n';
	}
	else if (byte2>=0x04 && byte2<=0x1D) // Keys A-Z
	{
		if ((byte0 & 0x22) != 0)// Shift pressed
		{
			map_value = byte2 + 61;
		}
		else if (byte0 == 0x40) // Alt Gr pressed
		{
			switch (byte2)
			{
				case 0x14: // Q
					map_value = '/';
					break; 
				case 0x1A: // W
					map_value = '?';
					break;
			}
		}
		else
		{
			map_value = byte2 + 93;
		}
	}
	else if (byte2>=0x28 && byte2<=0x38)
	{
		if ((byte0 & 0x22) != 0)// Shift pressed
		{
			char arrChr[17] = {'\n',0x1B,'\b','\t',' ','_','+','`','{','}',0,'C','^','\"','<','>',':'};
			map_value = arrChr[byte2-40];
		}
		else
		{
			char arrChr[17] = {'\n',0x1B,'\b','\t',' ','-','=',0,'[',']',0,'c','~','\'',',','.',';'};
			map_value = arrChr[byte2-40];
		}
	}
	else if (byte2==0x27) // 0 number
	{
		if ((byte0 & 0x22) != 0)// Shift pressed
		{
			map_value = ')';
		}
		else
		{
			map_value = 48;
		}
	}
	else if (byte2>=0x1E && byte2<=0x26) // Numbers 1 to 9
	{
		if ((byte0 & 0x22) != 0)// Shift pressed
		{
			char arrSpChr[9] = {'!','@','#','$','%',0,'&','*','('};
			map_value = arrSpChr[byte2-30]; 
		}
		else
		{
			map_value = byte2 + 19;
		}
	}
#pragma region Numeric Keypad
	else if (byte2 == 0x62) 
	{
		map_value = '0';
	}
	else if (byte2 == 0x63)
	{
		map_value = ',';
	}
	else if (byte2 == 0x85)
	{
		map_value = '.';
	}
	else if (byte2 == 0x57)
	{
		map_value = '+';
	}
	else if (byte2 == 0x56)
	{
		map_value = '-';
	}
	else if (byte2>=0x59 && byte2<=0x61)
	{
		map_value = byte2 - 40;
	}
#pragma endregion
	else if (byte2==0x64)
	{
		if ((byte0 & 0x22) != 0)// Shift pressed
		{
			map_value = '|';
		}
		else
		{
			map_value = '\\';
		}
	}
	USB_KEYBOARD_DEBUG_ARGS(" |-> 0x%02x (char: %c)\n", map_value, (char)map_value);
	return (char)map_value;
}


void usb_opening_cb()
{
	usb_keyboard_callback(KEYBOARD_OPENING);
}

void usb_opened_cb()
{
	usb_keyboard_callback(KEYBOARD_OPENED);
}

void usb_close_cb()
{
	usb_keyboard_callback(KEYBOARD_CLOSE);
}

void usb_data_trasnfer_cb(usb_transfer_t *transfer)
{
	if (transfer->data_buffer[2] != 0) // Check byte to decode.
	{
#ifdef USB_KEYBOARD_DEBUG
		printf("[USBKeyboard]:data received= ");
		for (int i=0; i<transfer->actual_num_bytes; i++) 
		{
			printf("0x%02x ", transfer->data_buffer[i]);
		}
		printf("\n");
#endif
		data_received[data_received_length] = keyboard_mapping(transfer->data_buffer[0], transfer->data_buffer[2]);
		data_received_length++;
		if (data_received_length == USB_KEYBOARD_BUFFER_LENGTH)
		{
			data_received_length=0;			
		}
		usb_keyboard_callback(KEYBOARD_TRANSFER);
	}
}
