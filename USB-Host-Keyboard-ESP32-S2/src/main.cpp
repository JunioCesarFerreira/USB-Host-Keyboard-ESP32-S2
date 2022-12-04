#include <Arduino.h>
#include <USBKeyboard.h>
#include <RgbPixel.hpp>

USBKeyboard usbKeyboard;
RgbPixelClass RgbPixel;

/// @brief Data receiving event.
void data_received(KeyboardAction action)
{
    switch(action)
    {
        case KEYBOARD_OPENING: // Connecting
            RgbPixel.set(RGB_YELLON);
            break;

        case KEYBOARD_OPENED: // Connected
            RgbPixel.set(RGB_GREEN);
            break;

        case KEYBOARD_CLOSE: // Disconnect
            RgbPixel.set(RGB_RED);
            break;

        default: // Data received
            RgbPixel.set(RGB_BLUE);
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
    RgbPixel.begin();
    RgbPixel.set(RGB_RED);
}

void loop() { }
