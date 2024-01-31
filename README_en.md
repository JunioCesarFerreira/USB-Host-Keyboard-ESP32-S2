# USB-Host-Keyboard-ESP32-S2

🌍 *[Português](README.md) ∙ [English](README_en.md)*

## Overview
This project implements a USB host keyboard using an ESP32-S2 microcontroller. It is designed to interface with USB keyboards and process their inputs.

## Features
- USB Keyboard host functionality
- Compatible with ESP32-S2 microcontrollers
- Modular design with separate components for easy customization

## Getting Started

### Prerequisites
- ESP32-S2 microcontroller
- USB keyboard

### Installation
1. Install PlatformIO IDE or integrate it with Visual Studio Code.
2. Clone the repository to your local machine.
3. Open the project using PlatformIO or your preferred ESP32 development environment.

### Usage
- Connect your USB keyboard to the ESP32-S2 microcontroller.
- Power up the ESP32-S2.
- The device will start processing keyboard inputs.

## Structure
- `.gitignore`: Git ignore file.
- `.pio`: Contains PlatformIO build outputs and configurations.
- `.vscode`: VSCode configuration files for the project.
- `include`: Contains include files for the project.
- `lib`: Libraries used in the project.
  - `RgbPixel`: Library for controlling RGB pixels.
  - `USBHostHid`: USB Host HID handling library.
  - `USBKeyboard`: Library for handling USB keyboard functionalities.
- `platformio.ini`: PlatformIO configuration file.
- `src`: Contains the main source code for the project.
- `test`: Contains test scripts for the project.

## Libraries
- RgbPixel: Handles RGB pixel control.
- USBHostHid: Manages USB Host HID functionalities.
- USBKeyboard: Implements USB keyboard interfacing.

## Contributing
Contributions to this project are welcome. Please follow standard coding practices and provide detailed descriptions of your contributions.

## License
This project is licensed under [LICENSE](LICENSE). See the [LICENSE](LICENSE) file for more details.
