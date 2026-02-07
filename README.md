# Smart Parking System

[![Version](https://img.shields.io/badge/version-1.0-blue.svg)](https://github.com/sanhithaac/Smart-Parking-System-)
[![Platform](https://img.shields.io/badge/platform-STM32F4-green.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f4-series.html)
[![License](https://img.shields.io/badge/license-Educational-orange.svg)](LICENSE)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](CONTRIBUTING.md)

> An intelligent embedded system for automated parking gate control using STM32F4 microcontroller, featuring IR sensors, servo motors, and Bluetooth communication.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Pin Configuration](#pin-configuration)
- [System Architecture](#system-architecture)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Configuration](#configuration)
- [Usage](#usage)
- [Technical Specifications](#technical-specifications)
- [Troubleshooting](#troubleshooting)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Overview

This project implements an intelligent dual-gate control system designed for parking lot or access control applications. The system leverages infrared sensors to detect incoming and outgoing vehicles, automatically operates servo-controlled gates, and provides real-time status updates via Bluetooth communication.

### Why This Project?

- **Automated Access Control**: Eliminates manual gate operation for parking facilities
- **Real-time Monitoring**: Bluetooth connectivity enables remote status tracking
- **Scalable Design**: Foundation for advanced parking management systems
- **Educational Value**: Comprehensive example of embedded systems integration

## Features

### Core Capabilities

- ✅ **Dual Gate Operation**: Independent control for incoming and outgoing traffic
- ✅ **Intelligent State Management**: State machine logic with robust flag tracking
- ✅ **Sensor Debouncing**: Stable readings with 3ms debounce delay
- ✅ **Wireless Monitoring**: Real-time gate status via Bluetooth
- ✅ **Analog Input Support**: ADC capability for additional sensor integration

### Safety & Reliability

- 🛡️ Pull-up resistors on sensor inputs for reliable detection
- 🛡️ State change detection to prevent unnecessary servo movements
- 🛡️ Gate state flags to avoid duplicate operations
- 🛡️ Simultaneous vehicle detection handling

## Hardware Requirements

### Required Components

#### Microcontroller
- **Board**: STM32F4 Discovery Board (STM32F407VG recommended)
- **MCU**: STM32F407VG (ARM Cortex-M4 with FPU)
- **Clock Speed**: 16 MHz (scalable up to 168 MHz)

#### Sensors & Actuators
- **IR Sensors** (2x): Infrared obstacle detection modules
  - Sensor 0 (PA0): Incoming vehicle detection
  - Sensor 1 (PA1): Outgoing vehicle detection
- **Servo Motor** (1x): SG90 or compatible (50Hz PWM)
  - Connection: PB6 (TIM4_CH1)
  - Range: 0° to 90°
- **ADC Sensor** (Optional): Analog sensor on PC0

#### Communication & Indicators
- **Bluetooth Module**: HC-05 or HC-06
  - Interface: USART2 (PA2/PA3)
  - Baud Rate: 9600 bps
- **Status LED**: PC13 (onboard LED on most STM32F4 Discovery boards)

## Pin Configuration

| Pin   | Function              | Description                    | Configuration |
|-------|-----------------------|--------------------------------|---------------|
| PA0   | GPIO Input            | IR Sensor 0 (Incoming gate)   | Pull-up       |
| PA1   | GPIO Input            | IR Sensor 1 (Outgoing gate)   | Pull-up       |
| PA2   | USART2 TX             | Bluetooth TX                   | AF7           |
| PA3   | USART2 RX             | Bluetooth RX                   | AF7           |
| PB6   | TIM4_CH1              | Servo PWM control              | AF2           |
| PC0   | ADC1_IN10             | Analog sensor input            | Analog        |
| PC13  | GPIO Output           | Status LED                     | Push-pull     |

## System Architecture

### Gate Control Logic

The system operates using a state-machine approach with intelligent sensor monitoring:

**Incoming Vehicle Detection (Sensor 0 - PA0)**
```
Vehicle Detected → Gate Opens (90°) → Bluetooth: "Gate 1 closed"
      ↓
Vehicle Clears → Gate Closes (0°) → Bluetooth: "Gate 1 opened for outgoing car"
```

**Outgoing Vehicle Detection (Sensor 1 - PA1)**
```
Vehicle Detected → Gate Opens (90°) → Bluetooth: "Gate 2 closed"
      ↓
Vehicle Clears → Gate Closes (0°) → Bluetooth: "Gate 2 opened for outgoing car"
```

**Simultaneous Detection**
```
Both Sensors Active → Gate Remains Open (90°)
      ↓
Notification: "Both gates closed for incoming and outgoing cars"
```

## Getting Started

### Prerequisites

**Software Requirements**
- IDE: Keil µVision 5 or STM32CubeIDE
- Compiler: ARM Compiler 5/6 (ARMCC) or GCC ARM
- Debugger: ST-LINK V2 drivers
- Serial Terminal: PuTTY, TeraTerm, or Arduino Serial Monitor

**Hardware Requirements**
- STM32F4 Discovery board
- USB cable for programming and power
- IR sensors (2x)
- Servo motor
- HC-05/HC-06 Bluetooth module
- Jumper wires and breadboard

**Required Libraries**
- CMSIS Core
- STM32F4xx HAL/Standard Peripheral Library
- Device header: `stm32f4xx.h`

### Installation

#### Option 1: Using Keil µVision

1. **Create New Project**
   ```
   Project → New µVision Project
   Select Device: STM32F407VG
   ```

2. **Add Source Files**
   - Add `main.c` to project
   - Include device headers from CMSIS pack

3. **Configure Project**
   - Set target clock to 16 MHz
   - Enable FPU support
   - Configure ST-LINK debugger

4. **Build and Flash**
   ```
   Build: F7 or Project → Build Target
   Flash: F8 or Flash → Download
   ```

#### Option 2: Using STM32CubeIDE

1. **Import or Create Project**
   ```
   File → New → STM32 Project
   Board Selector: STM32F407VG Discovery
   ```

2. **Add Source Code**
   - Place `main.c` in `Core/Src/` directory
   - Ensure headers are accessible

3. **Build and Deploy**
   ```
   Build: Ctrl+B or Project → Build All
   Flash: Run → Debug (F11)
   ```

### Configuration

#### PWM Settings for Servo Control
```c
Frequency: 50 Hz (20 ms period)
Pulse Width: 500-2500 µs
Timer: TIM4
Prescaler: 15
Auto-Reload Register (ARR): 19999
```

#### USART2 Configuration (Bluetooth)
```c
Baud Rate: 9600 bps
Data Bits: 8
Stop Bits: 1
Parity: None
Hardware Flow Control: Disabled
```

#### ADC Configuration
```c
Resolution: 12-bit
Channel: 10 (PC0)
Mode: Single conversion
Sampling Time: 84 cycles
```

## Usage

### Quick Start Guide

1. **Hardware Setup**
   - Connect STM32F4 Discovery board to your computer via USB
   - Wire IR sensors to PA0 and PA1 with appropriate power connections
   - Connect servo motor signal wire to PB6
   - Attach Bluetooth module to PA2 (TX) and PA3 (RX)

2. **Power On System**
   - Flash the compiled binary to the board
   - Power cycle the board if necessary
   - Status LED (PC13) should indicate system is running

3. **Bluetooth Connection**
   - Enable Bluetooth on your smartphone or computer
   - Search for HC-05/HC-06 module (default name: HC-05)
   - Default PIN: 1234 or 0000
   - Pair with the device

4. **Monitor Operations**
   - Open serial terminal application
   - Configure: 9600 baud, 8N1 (8 data bits, no parity, 1 stop bit)
   - Monitor real-time gate status messages

5. **Test Functionality**
   - Trigger IR Sensor 0 (PA0) to simulate incoming vehicle
   - Observe gate opening and status message
   - Trigger IR Sensor 1 (PA1) to simulate outgoing vehicle
   - Verify independent gate operation

### Bluetooth Communication

**Current Features**
- Real-time status notifications
- Gate position updates
- Sensor state reporting

**Example Output**
```
Gate 1 closed
Gate 1 opened for outgoing car
Gate 2 closed
Both gates closed for incoming and outgoing cars
```

**Future Command Support** (Planned)
```
OPEN_GATE_1    - Manually open incoming gate
CLOSE_GATE_1   - Manually close incoming gate
STATUS         - Query current system status
CONFIG         - Update configuration parameters
```

## Technical Specifications

### Performance Metrics

| Parameter              | Value           | Unit    |
|------------------------|-----------------|---------|
| Main Loop Frequency    | 10              | Hz      |
| Sensor Debounce Time   | 3               | ms      |
| Servo Response Time    | 20              | ms      |
| ADC Conversion Time    | ~3              | µs      |
| Bluetooth Baud Rate    | 9600            | bps     |
| PWM Frequency          | 50              | Hz      |
| Operating Voltage      | 3.3 / 5.0       | V       |

### Timing Analysis
- **Main Loop**: 100 ms cycle time
- **Debouncing**: 3 ms per sensor read (6 ms total for both sensors)
- **Servo Positioning**: ~20 ms settling time
- **State Processing**: <1 ms per iteration

## Troubleshooting

### Common Issues and Solutions

#### Problem: Servo not responding
**Symptoms**: Gate doesn't move when sensors are triggered
**Solutions**:
- Verify PB6 connection to servo signal wire
- Check servo power supply (separate 5V recommended)
- Confirm Timer4 PWM initialization
- Test servo with known working pulse widths (500-2500 µs)

#### Problem: Bluetooth connection fails
**Symptoms**: Cannot pair or connect to HC-05/HC-06
**Solutions**:
- Check module power indicator LED
- Verify USART2 TX/RX connections (PA2/PA3)
- Try default pairing codes: 1234, 0000, or 1111
- Reset Bluetooth module by power cycling

#### Problem: Sensors not detecting vehicles
**Symptoms**: No gate movement on object detection
**Solutions**:
- Verify pull-up resistors on PA0 and PA1
- Check sensor power supply (typically 3.3V or 5V)
- Test sensor output with multimeter
- Adjust sensor sensitivity potentiometer (if available)
- Ensure proper sensor-to-object distance

#### Problem: Erratic gate behavior
**Symptoms**: Gate opens/closes unexpectedly
**Solutions**:
- Increase debounce delay if environment is noisy
- Check for electromagnetic interference near sensors
- Verify stable power supply to all components
- Review state machine logic for race conditions

#### Problem: No Bluetooth messages received
**Symptoms**: Terminal shows no output
**Solutions**:
- Confirm baud rate: 9600 bps, 8N1
- Check USART2 initialization code
- Verify Bluetooth module TX is connected to PA3 (RX pin)
- Test with different serial terminal software

### Debugging Tips

1. **Use Status LED**: PC13 LED can be toggled to verify code execution
2. **Check Clock Configuration**: Ensure 16 MHz system clock is configured
3. **Monitor ADC Values**: Use ADC readings to verify analog input functionality
4. **Serial Debug Output**: Add debug messages to trace execution flow
5. **Logic Analyzer**: Use for timing analysis of PWM and USART signals

## Roadmap

### Version 2.0 (Planned)

- [ ] **Dual Servo Support**: Independent servo motors for each gate
- [ ] **Bidirectional Bluetooth**: Command parsing for remote control
- [ ] **ADC Integration**: Distance/proximity sensing for improved accuracy
- [ ] **Vehicle Counting**: Track entry/exit statistics
- [ ] **LCD Display**: Local status monitoring without Bluetooth
- [ ] **SD Card Logging**: Historical data storage and analysis

### Version 3.0 (Future)

- [ ] **RTOS Integration**: FreeRTOS for concurrent task management
- [ ] **Wi-Fi Connectivity**: ESP32 integration for IoT capabilities
- [ ] **Mobile App**: Dedicated smartphone application
- [ ] **Cloud Integration**: Real-time data analytics and remote monitoring
- [ ] **RFID Support**: Access control with RFID tags
- [ ] **License Plate Recognition**: Camera integration with OCR

## Contributing

We welcome contributions from the community! Here's how you can help:

### How to Contribute

1. **Fork the repository**
2. **Create a feature branch** (`git checkout -b feature/AmazingFeature`)
3. **Commit your changes** (`git commit -m 'Add some AmazingFeature'`)
4. **Push to the branch** (`git push origin feature/AmazingFeature`)
5. **Open a Pull Request**

### Contribution Guidelines

- Follow existing code style and conventions
- Add comments for complex logic
- Test thoroughly on actual hardware
- Update documentation for any new features
- Include example usage in PR description

### Areas for Contribution

- 🐛 Bug fixes and issue resolution
- 📝 Documentation improvements
- ✨ New features and enhancements
- 🧪 Test case development
- 🎨 Code optimization and refactoring

## License

This project is provided as-is for **educational and development purposes**.

```
MIT License - Feel free to use, modify, and distribute
See LICENSE file for full details
```

## Contact

### Project Maintainer

**Embedded Systems Development Team**

- 📧 Email: [Create an issue](https://github.com/sanhithaac/Smart-Parking-System-/issues) for questions
- 💬 Discussions: [GitHub Discussions](https://github.com/sanhithaac/Smart-Parking-System-/discussions)
- 🐛 Bug Reports: [Issue Tracker](https://github.com/sanhithaac/Smart-Parking-System-/issues)

### Acknowledgments

- STMicroelectronics for STM32F4 platform and documentation
- ARM for Cortex-M4 processor architecture
- Open-source community for embedded systems resources

---

<div align="center">

**Made with ❤️ for Embedded Systems Education**

![STM32F4](https://img.shields.io/badge/STM32-F4%20Series-blue)
![Embedded C](https://img.shields.io/badge/Language-Embedded%20C-green)
![IoT](https://img.shields.io/badge/Category-IoT%20%2F%20Smart%20Systems-orange)

**[⬆ Back to Top](#smart-parking-system)**

*Last Updated: February 2026 | Version 1.0 | STM32F407VG Discovery*

</div>
