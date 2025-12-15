# SMART PARKING SYSTEM - STM32F4 Dual Gate Control System

A robust embedded system for automated gate control using IR sensors, servo motors, and Bluetooth communication on the STM32F4 microcontroller platform.

## Overview

This project implements an intelligent dual-gate control system designed for parking lot or access control applications. The system uses infrared sensors to detect incoming and outgoing vehicles, automatically operates servo-controlled gates, and provides real-time status updates via Bluetooth communication.

## Hardware Components

### Microcontroller
- **Board**: STM32F4 Discovery Board (or compatible STM32F4xx)
- **MCU**: STM32F407VG (Cortex-M4 core)
- **Clock Speed**: 16 MHz (configurable up to 168 MHz)
- **Architecture**: ARM Cortex-M4 with FPU

### Sensors & Actuators
- **IR Sensors**: 2x Infrared obstacle detection sensors
  - Sensor 0 (PA0): Incoming vehicle detection
  - Sensor 1 (PA1): Outgoing vehicle detection
- **Servo Motor**: 1x SG90 or compatible servo (50Hz PWM)
  - Connected to PB6 (TIM4_CH1)
  - Operating range: 0° to 90°
- **ADC Sensor**: Analog sensor on PC0 (Channel 10)

### Communication & Indicators
- **Bluetooth Module**: HC-05 or HC-06 Bluetooth module
  - TX/RX via USART2 (PA2/PA3)
  - Baud rate: 9600 bps
- **Status LED**: Connected to PC13 (onboard LED)

## Pin Configuration

| Pin   | Function              | Description                    |
|-------|-----------------------|--------------------------------|
| PA0   | Input (Pull-up)       | IR Sensor 0 (Incoming gate)   |
| PA1   | Input (Pull-up)       | IR Sensor 1 (Outgoing gate)   |
| PA2   | USART2 TX (AF7)       | Bluetooth TX                   |
| PA3   | USART2 RX (AF7)       | Bluetooth RX                   |
| PB6   | TIM4_CH1 (AF2)        | Servo PWM control              |
| PC0   | ADC1_IN10             | Analog sensor input            |
| PC13  | Output                | Status LED                     |

## Features

### Core Functionality
- **Dual Gate Operation**: Independent control for incoming and outgoing traffic
- **State Machine Logic**: Intelligent gate state tracking with flags
- **Debouncing**: Stable sensor reading with 3ms debounce delay
- **Bluetooth Monitoring**: Real-time gate status notifications
- **Analog Input**: ADC support for additional sensors

### Safety Features
- Pull-up resistors on sensor inputs for reliable detection
- State change detection to prevent unnecessary servo movements
- Gate state flags to avoid duplicate operations
- Both-sensor detection for simultaneous vehicle handling

## System Operation

### Gate Control Logic

**Incoming Vehicle (Sensor 0)**:
1. Sensor detects vehicle → Gate lifts to 90°
2. Sends "Gate 1 closed" notification
3. Vehicle clears sensor → Gate drops to 0°
4. Sends "Gate 1 opened for outgoing car" notification

**Outgoing Vehicle (Sensor 1)**:
1. Sensor detects vehicle → Gate lifts to 90°
2. Sends "Gate 2 closed" notification
3. Vehicle clears sensor → Gate drops to 0°
4. Sends "Gate 2 opened for outgoing car" notification

**Simultaneous Detection**:
- Both sensors active → Gate remains at 90°
- Notification: "Both gates closed for incoming and outgoing cars"

## Build & Compile

### Development Environment
- **IDE**: Keil µVision 5 or STM32CubeIDE
- **Compiler**: ARM Compiler 5/6 (ARMCC) or GCC ARM
- **Debugger**: ST-LINK V2 or compatible

### Required Libraries
- CMSIS Core
- STM32F4xx HAL/Standard Peripheral Library
- `stm32f4xx.h` device header

### Compilation Steps

**Keil µVision**:
1. Create new project for STM32F407VG
2. Add source file and device header
3. Configure target options (16 MHz clock)
4. Build project (F7)
5. Flash to board via ST-LINK

**STM32CubeIDE**:
1. Import project or create new STM32F4 project
2. Add source file to `Src/` folder
3. Build project (Ctrl+B)
4. Flash using debug configuration

### Compiler Settings
- Optimization Level: -O0 (Debug) or -O2 (Release)
- Target: Cortex-M4 with FPU
- Floating Point: Hardware FPU enabled

## Configuration

### PWM Parameters
- **Frequency**: 50 Hz (20 ms period)
- **Pulse Width Range**: 500-2500 µs
- **Timer**: TIM4, Prescaler = 15, ARR = 19999

### USART2 Settings
- **Baud Rate**: 9600 bps
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None

### ADC Configuration
- **Resolution**: 12-bit
- **Channel**: 10 (PC0)
- **Mode**: Single conversion

## Usage

1. **Power Up**: Connect STM32F4 board to power supply
2. **Bluetooth Pairing**: Pair HC-05/HC-06 with smartphone/computer
3. **Monitor Status**: Open serial terminal (9600 baud) to view gate operations
4. **Testing**: Trigger IR sensors to verify gate movement

### Bluetooth Commands
Currently configured for status output only. Future enhancements could include:
- Manual gate control commands
- Configuration parameter updates
- System diagnostics

## Timing Specifications

- **Main Loop Delay**: 100 ms
- **Debounce Delay**: 3 ms per sensor reading
- **Servo Movement**: 20 ms settling time
- **ADC Conversion**: ~3 µs (12-bit at 16 MHz)

## Known Limitations

- Single servo controls both conceptual "gates" (expandable to dual servos)
- Bluetooth status messages are output-only (no command parsing)
- ADC value read but not currently utilized in logic
- Fixed delay functions (consider using hardware timers for production)

## Future Enhancements

- [ ] Dual servo support for independent gate control
- [ ] Bluetooth command parsing for remote control
- [ ] ADC integration for distance/proximity sensing
- [ ] RTOS implementation for concurrent task management
- [ ] Vehicle counting and statistics logging
- [ ] LCD display for local status monitoring

## License

This project is provided as-is for educational and development purposes.

## Author

Embedded Systems Project - STM32F4 Development

---

**Last Updated**: December 2025  
**Version**: 1.0  
**Board**: STM32F407VG Discovery
