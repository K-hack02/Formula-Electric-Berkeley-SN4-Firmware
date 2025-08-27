# Formula Electric Berkeley – SN4 Firmware

This repository contains the firmware for **Formula Electric at Berkeley's SN4 system**, which manages multiple embedded subsystems of the Formula SAE Electric Vehicle. Each subsystem runs on STM32 microcontrollers and communicates via CAN bus.

## Repository Structure

The codebase is organized into separate directories for each major subsystem:

- **BMS/**  
  Battery Management System firmware implementing custom BMS state machine. Handles cell voltage and temperature monitoring, balancing, charging logic, fault detection, and fault handling to ensure safe accumulator operation.


- **BMS RTOS/**  
  Real-time operating system layer for the BMS, using FreeRTOS to schedule tasks, manage inter-task communication, and ensure reliable concurrency in safety-critical operations.


- **PCU/**  
  Power Control Unit firmware responsible for interpreting accelerator pedal and brake pressure inputs to generate torque commands. Includes safety handling, high-voltage interlock management, and robust fault tolerance.


- **DASH/**  
  Driver dashboard firmware that processes driver inputs and receives CAN messages for display. Interfaces with a SquareLine UI to render vehicle status and alerts on screen.


- **LVPDB/**  
  Low Voltage Power Distribution Board firmware controlling TPS chips that distribute power to various vehicle subsystems such as coolant pumps and brake lights.


- **sensor_nodes/**  
  Firmware for distributed sensor boards that collect data (temperature, pressure, potentiometers, etc.) and package it into CAN messages. A single codebase supports both front and rear sensor nodes, using conditional logic to configure board-specific behavior.


- **data_control_unit/**  
  Data Control Unit firmware that logs all CAN messages on the bus to an SD card and transmits essential messages over Xbee for real-time telemetry.


- **DART_SN3/**  
  Distributed Air Routing Technology firmware controlling the 10 accumulator cooling fans, ensuring proper airflow and temperature regulation during operation.


- **ivMeter/**  
  Custom current/voltage metering board designed to replace the IVT. Sends high-voltage circuit voltage and current measurements over CAN for system monitoring.

## My Contributions

I (Kavin Vasudevan) contributed heavily to the following subsystems:

- **BMS** – Implemented the full BMS state machine, charging logic, and fault detection/handling. Rewrote the driver handshake logic to improve reliability.


- **BMS RTOS** – Implemented a FreeRTOS architecture for the BMS, including task scheduling, inter-task communication, and concurrency-safe data handling.


- **PCU** – Developed firmware to process accelerator pedal and brake pressure sensor inputs, with integrated fault-tolerant logic. Also rewrote the driver handshake logic.


- **DASH** – Optimized superloop operations, implemented visualization logic to display incoming CAN messages, and rewrote the driver handshake logic.


- **DCU (Partial Contribution)** – Implemented a circular buffer for storing CAN messages, enabling efficient and reliable telemetry logging.

## Key Features

- **CAN Communication** – Unified communication across all boards using standardized DBC-based message generation and parsing.


- **Fault Handling & Heartbeat Monitoring** – Implemented to detect subsystem failures and ensure safety-critical functionality.  


- **Telemetry & Data Logging** – Real-time telemetry pipeline with structured message forwarding for post-race analysis and debugging.


- **FreeRTOS Integration** – Used in the BMS to provide multi-threaded execution and reliable timing guarantees.

## Getting Started

### Prerequisites
- [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)

### Building & Flashing
1. Open the desired subsystem project folder (e.g., `PCU/`) in STM32CubeIDE.  
2. Build the project for the target STM32 board.  
3. Flash using ST-Link or equivalent programmer/debugger.

## License
This repository is maintained by Formula Electric at Berkeley and is intended for educational and research use.
