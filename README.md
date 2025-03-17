# Heart Rate Monitor with MAX30102 and ESP32

## Overview
Implementation of a **heart rate monitor** using the **MAX30102 digital sensor** and **ESP32**. The project uses an OLED display to visualize heart rate and heart rate variability (HRV) in real-time.

## Documentation
For detailed information and demonstration, please refer to **`dokumentace.pdf`** included in this repository (written in Czech).

## Features
- Measures heart rate using the MAX30102 sensor
- Displays heart rate (BPM) and HRV on an OLED screen
- Uses an **ESP32** microcontroller for processing and display control
- Visual feedback with a blinking **LED** and heart symbols on the screen
- Serial output for monitoring and debugging

## Hardware Requirements
- ESP32 Development Board
- MAX30102 Pulse Oximeter & Heart Rate Sensor
- OLED Display (SSD1306, 128x64 resolution)
- Connecting wires
- Power source

## Software Requirements
- **PlatformIO** (or **Arduino IDE**)
- Required Libraries:
  - `Wire.h` (for I2C communication)
  - `MAX30105.h` (MAX30102 sensor library)
  - `heartRate.h` (Heart rate calculation functions)
  - `Adafruit_GFX.h` (Graphics library for OLED display)
  - `Adafruit_SSD1306.h` (OLED driver library)

## Installation and Setup
1. Clone this repository:
   ```bash
   git clone https://github.com/kristian-dobes/heart-rate-monitor.git
   ```
2. Open the `src/main.cpp` file in PlatformIO or Arduino IDE.
3. Install the required libraries (listed above).
4. If using **PlatformIO**, ensure you have the correct environment settings in `platformio.ini`.
5. Connect the hardware as per the wiring configuration in `main.cpp`.
6. Compile and upload the code to the ESP32.
7. Open the Serial Monitor (115200 baud rate) to view real-time readings.

## Usage
- Place your finger over the MAX30102 sensor.
- The OLED display will show:
  - **BPM (Beats Per Minute)**
  - **HRV (Heart Rate Variability in milliseconds)**
- If no finger is detected, a message will prompt the user to place their finger on the sensor.

## File Structure
```
heart-rate-monitor/
│── src/
│   ├── main.cpp   # Main program file
│── platformio.ini   # PlatformIO configuration file
│── dokumentace.pdf   # Project documentation
│── README.md     # Project description and setup instructions
```

## License
This project is licensed under the **MIT License**.

## Author
Kristian Dobes - **xdobes22**

## Acknowledgments
- Adafruit for the SSD1306 OLED display libraries
- SparkFun for the MAX30102 sensor library