# USB Host Keyboard ESP32-S2

🌍 *[Português](README.md) ∙ [English](README_en.md)*

## Overview
This project implements a USB keyboard host using the ESP32-S2 microcontroller. It is designed to efficiently interact with USB keyboards and process their inputs.

## Features
- **USB Keyboard Host Functionality**: Enables direct connection and interaction with USB keyboards.
- **Compatible with ESP32-S2**: Specifically designed for the ESP32-S2 microcontroller, leveraging its capabilities.
- **Modular Design**: Separate components for easy customization and expansion.

## Getting Started

### Prerequisites
- **ESP32-S2** microcontroller
- USB Keyboard

### Installation
1. Install **PlatformIO IDE** or integrate it with **Visual Studio Code**.
2. Clone this repository to your local machine:
   ```bash
   git clone https://github.com/JunioCesarFerreira/USB-Host-Keyboard-ESP32-S2
   ```
3. Open the project in PlatformIO or your preferred ESP32 development environment.

### Usage
1. Connect your USB keyboard to the ESP32-S2 microcontroller.
2. Power on the ESP32-S2.
3. The device will automatically start processing keyboard inputs.

## Project Structur
- **`lib`**: Libraries used in the project.
  - **RgbPixel**: RGB pixel control.
  - **USBHostHid**: USB HID Host handling.
  - **USBKeyboard**: USB keyboard functionalities.
- **`platformio.ini`**: PlatformIO configuration file.
- **`src`**: Main source code of the project.

## Libraries
- **RgbPixel**: Manages RGB pixel control.
- **USBHostHid**: Manages USB HID Host functionalities.
- **USBKeyboard**: Implements interaction with USB keyboards.

## Contributing
Contributions are welcome! Please follow standard coding practices and provide detailed descriptions of your changes. To contribute, please:
1. Fork this repository.
2. Create a new branch for your changes:
   ```bash
   git checkout -b my-new-feature
   ```
3. Commit your changes:
   ```bash
   git commit -m "Add new feature"
   ```
4. Push to the branch:
   ```bash
   git push origin my-new-feature
   ```
5. Open a Pull Request for review.

## License
This project is licensed under the [MIT License](LICENSE).