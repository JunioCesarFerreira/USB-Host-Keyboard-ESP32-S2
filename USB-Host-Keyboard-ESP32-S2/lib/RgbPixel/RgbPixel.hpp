/*
 *
 * RgbPixel controller
 * Author: Junio C Ferreira <jcf_ssp@hotmail.com>
 * 
 * Based on https://www.instructables.com/ESP32-S2-Saola-Making-the-RGB-Work/
 * 
 */
#ifndef RgbPixel_h

#include <Adafruit_NeoPixel.h>

// On the ESP32S2 SAOLA GPIO is the NeoPixel.
#define PIN        18 

//Single NeoPixel
Adafruit_NeoPixel pixels(1, PIN, NEO_GRB + NEO_KHZ800);

enum RgbBasicColor
{
    RGB_OFF,
    RGB_RED,
    RGB_GREEN,
    RGB_BLUE,
    RGB_YELLON,
    RGB_CYAN,
    RGB_MAGENTA
};

class RgbPixelClass
{
    private:
        RgbBasicColor actual;

    public:
        void begin()
        {
            actual = RGB_OFF;
            pixels.setBrightness(10); // Default brightness
            pixels.begin(); // INITIALIZE NeoPixel (REQUIRED)
        }

        RgbBasicColor get()
        {
            return actual;
        }

        void set(RgbBasicColor color)
        {
            actual = color;
            uint32_t value_color = 0;
            //Set the new color on the pixel.
            switch (color)
            {
                case RGB_RED:
                    value_color = 0x00FF0000;
                    break;
                case RGB_GREEN:
                    value_color = 0x0000FF00;
                    break;
                case RGB_BLUE:
                    value_color = 0x000000FF;
                    break;
                case RGB_YELLON:
                    value_color = 0x00FFFF00;
                    break;
                case RGB_CYAN:
                    value_color = 0x0000FFFF;
                    break;
                case RGB_MAGENTA:
                    value_color = 0x00FF00FF;
                    break;            
                default:
                    value_color = 0x00000000;
                    break;
            }
            pixels.setPixelColor(0, value_color);

            // Send the updated pixel colors to the hardware.
            pixels.show();   
        }
        void brightness(uint8_t value)
        {
            pixels.setBrightness(value);
        }
};

#endif // !RgbPixel_h