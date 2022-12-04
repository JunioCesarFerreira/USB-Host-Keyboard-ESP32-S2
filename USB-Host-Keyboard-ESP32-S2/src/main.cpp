#include <Arduino.h>
#include <USBKeyboard.h>
#include <RgbPixel.hpp>

USBKeyboard usbKeyboard;
RgbPixelClass rgbPixel;

/// @brief Data receiving event.
void data_received(KeyboardAction action)
{
    switch(action)
    {
        case KEYBOARD_OPENING: // Connecting
            rgbPixel.set(RGB_YELLON);
            break;

        case KEYBOARD_OPENED: // Connected
            rgbPixel.set(RGB_GREEN);
            break;

        case KEYBOARD_CLOSE: // Disconnect
            rgbPixel.set(RGB_RED);
            break;

        default: // Data received
            if (rgbPixel.get() != RGB_BLUE)
            {
                rgbPixel.set(RGB_BLUE);
            }
            else
            {
                rgbPixel.set(RGB_CYAN);
            }
            printf("data: %s\n", usbKeyboard.getBuffer());
            break;
    }
}

void setup(void)
{
    Serial.begin(921600);
    delay(2000); 
    printf("\n\nTest usb host keyboard.\n");
    usbKeyboard.begin(data_received);
    rgbPixel.begin();
    rgbPixel.set(RGB_RED);
}

void loop() { }
