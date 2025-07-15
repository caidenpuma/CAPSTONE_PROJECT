# CAPSTONE PROJECT

## Overview

This project implements a fully embedded soil monitoring system designed to simplify plant care through real-time environmental sensing and on-device feedback. SoilSense collects and displays data on moisture, temperature, humidity, and light intensity to help plant owners maintain optimal growing conditions. Unlike commercial solutions that rely on mobile app connectivity, SoilSense delivers actionable insights directly via an onboard LCD interface, offering a self-contained, user-friendly experience.

---

## Features

- Real-time sensing of soil moisture, ambient temperature, humidity, and light intensity
- Onboard LCD interface with plant-specific alert logic
- Configurable plant profiles for adaptive threshold monitoring
- LED indicator system for status and fault conditions
- Modular firmware architecture using STM32CubeIDE
- Designed for indoor plants up to 24" in diameter and depth
- Expandable architecture to support automated watering

---

## System Architecture

### Hardware Overview

| Component | Part Number | Function |
|----------|-------------|----------|
| Microcontroller | STM32L452RC | ARM Cortex-M4 MCU with low-power capabilities |
| Moisture Sensor | Coliao Capacitive | Soil water content detection |
| Temperature & Humidity Sensor | HDC2010YPAR | Indoor climate sensing |
| Light Sensor | VEML7700 | Ambient light measurement |
| Display | LCD1602 | Local interface for sensor feedback |
| Power Supply | 2x AA Batteries | 3+ months of operation under typical use |

### Functional Block Diagram

```
[Moisture Sensor]       [Light Sensor]        [Temp/Humidity Sensor]
       |                      |                          |
       ---------------------------------------------------
                              |
                     [STM32L452RC MCU]
                              |
     -----------------------------------------------------
     |                    |                      |       |
[LED Indicators]   [LCD1602 Display]     [Push Buttons]   [Battery Pack]
```

---

## Firmware Features

- Sensor drivers implemented using STM32 HAL libraries
- Low-power modes enabled for extended battery life
- Debounced GPIO control for 5-button interface
- LCD feedback includes moisture alerts, temperature warnings, and sunlight recommendations
- Modular codebase organized by sensor/peripheral interface

---

## Display Interface and Controls

| Button | Setup Mode Function | Operation Mode Function |
|--------|----------------------|--------------------------|
| 1 | Select / Confirm | Open plant selection menu |
| 2 | Calibrate | Clear alerts |
| 3 | Toggle Display | Screen on/off |
| 4 | Back | Return to previous menu |
| 5 | Next | Cycle through sensor readouts or alerts |

---

## PCB and Fabrication

- Designed using KiCad
- Two-layer layout optimized for enclosure integration
- Gerber files and final schematic included in repository
- Moisture sensors located at staggered depths on dual probe legs
- Fully waterproofed casing concept with IP67 design targets

---

## Performance Summary

| Parameter | Value |
|-----------|-------|
| Battery Life | >3 months on 2 AA batteries |
| Update Interval | 10 seconds (default) |
| Minimum Pot Size | 4" diameter and depth |
| Display Visibility | Readable from >3 feet |
| Moisture Sensor Range | 0%–100% volumetric water content (relative) |

---

## Compliance and Design Standards

- IP67 environmental sealing for sensor and electronics protection
- RoHS-compliant components to ensure material safety
- Designed to meet UL guidelines for consumer electronic safety
- Uses non-toxic, corrosion-resistant materials for plant compatibility

---

## Risk Mitigation

| Risk | Strategy |
|------|----------|
| Sensor inaccuracy | Early calibration, plant-specific offsets |
| Power constraints | Low-power sleep modes and efficient polling loop |
| Moisture ingress | Enclosure sealing and waterproof connectors |
| Voltage mismatch | Onboard LDOs for 3.3V and 5V regulation |

---

## Getting Started

### Prerequisites

- STM32CubeIDE for firmware development
- ST-Link programmer/debugger
- Access to KiCad or Gerber viewer for PCB inspection
- 2 AA batteries and indoor plant pot for deployment

### Setup Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/caidenpuma/CAPSTONE_PROJECT.git
   ```

2. Open project file in STM32CubeIDE.

3. Connect the STM32L452RC development board via ST-Link.

4. Build and flash the firmware.

5. Assemble the PCB or use the breadboard prototype to connect sensors and LCD.

6. Insert batteries and deploy on a plant for testing.

---

## Demo and Verification

- Breadboard prototyping completed with working sensor readings
- External STM32F4 board used for software development and timing validation
- Final integrated PCB successfully demonstrated with real-time sensor output
- LCD readouts and LED indicators verified against environmental stimuli

---

## Future Enhancements

- Integration of a closed-loop self-watering system
- Expansion of plant type database with dynamic parameter mapping
- Optional wireless interface for remote notifications
- Enhanced mechanical enclosure for ruggedization
