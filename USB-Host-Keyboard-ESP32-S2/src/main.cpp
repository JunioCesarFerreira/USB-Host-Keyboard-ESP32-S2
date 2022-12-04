#include <Arduino.h>
#include <USBKeyboard.h>

USBKeyboard usbKeyboard;

/// @brief Data receiving event.
void data_received()
{
    printf("data: %s\n", usbKeyboard.getBuffer());
}

void setup(void)
{
    Serial.begin(921600);
    delay(2000); 
    printf("\n\nTest usb host keyboard.\n");
    usbKeyboard.begin(data_received);
}

void loop() { }
