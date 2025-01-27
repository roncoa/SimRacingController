# SimRacingController Library

[![GitHub release](https://img.shields.io/github/release/roncoa/SimRacingController.svg)](https://github.com/roncoa/SimRacingController/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

Advanced Arduino library for creating SimRacing button boxes with matrix button, direct GPIO, and encoder support. Designed specifically for racing simulator controllers, this library offers a flexible and efficient solution for building custom control panels.

## Features
- Button matrix management with debounce
- Direct GPIO button support
- Rotary encoder support with configurable sensitivity
- Optional encoder push buttons
- I2C expander support (MCP23017, PCF8574, PCF8574A)
- Multiple profiles support
- Event-driven architecture with callbacks
- Efficient memory usage
- Hardware-agnostic design

## Installation

### Using Arduino Library Manager (Recommended)
1. Open Arduino IDE
2. Go to Tools > Manage Libraries...
3. Search for "SimRacingController"
4. Click Install

### Manual Installation
1. Download latest release as ZIP
2. In Arduino IDE: Sketch > Include Library > Add .ZIP Library
3. Select downloaded ZIP file
4. Restart Arduino IDE

## Hardware Requirements

### Supported Boards
- Arduino Leonardo
- Arduino Pro Micro
- Any ATmega32U4 based board
- ESP32 boards (with custom I2C pins support)

### Required Components
- Push buttons for matrix
- Diodes (1N4148 or similar) for button matrix
- Rotary encoders (optional)
- Pull-up resistors (if not using internal pull-ups)
- I2C expanders (optional): MCP23017, PCF8574, or PCF8574A

## Basic Usage

```cpp
#include <SimRacingController.h>

// Matrix configuration
const int MATRIX_ROWS = 3;
const int MATRIX_COLS = 3;
const int rowPins[MATRIX_ROWS] = {2, 3, 4};
const int colPins[MATRIX_COLS] = {5, 6, 7};

// Encoder configuration
const int NUM_ENCODERS = 2;
const int encoderPinsA[NUM_ENCODERS] = {10, 12};
const int encoderPinsB[NUM_ENCODERS] = {11, 13};

// Create controller
SimRacingController controller;

void setup() {
    // Configure components
    controller.setMatrix(rowPins, MATRIX_ROWS, colPins, MATRIX_COLS);
    controller.setEncoders(encoderPinsA, encoderPinsB, NUM_ENCODERS);
    
    // Initialize controller
    controller.begin();
}

void loop() {
    controller.update();
}
```

## Examples

### Basic
Simple button and encoder reading with serial output.
- File: `examples/Basic/Basic.ino`

### Advanced
Complete setup showing all features including:
- Matrix and GPIO button handling
- Encoder configuration and reading
- I2C expander support
- Profile management
- File: `examples/Advanced/Advanced.ino`

### ButtonBox_ACC
Complete setup for Assetto Corsa Competizione with:
- Button mappings for common functions
- Encoder settings for TC, ABS, etc.
- Multiple profiles support
- File: `examples/ButtonBox_ACC/ButtonBox_ACC.ino`

Requirements:
- KeySequence library
- ACC shortcuts configuration (`Sequenze.h`)

## Documentation

### Detailed Guides
- [API Reference](https://github.com/roncoa/SimRacingController/blob/main/docs/api.md)
- [Hardware Wiring Guide](https://github.com/roncoa/SimRacingController/blob/main/docs/wiring.md)

### Key Concepts

#### Button Matrix
- Efficient scanning algorithm
- Built-in debounce
- No ghosting with proper diode configuration
- Independent state tracking

#### Direct GPIO Buttons
- Simple direct button connection
- Same debounce as matrix
- Ideal for single buttons

#### Encoders
- Configurable sensitivity (1-4x)
- Real-time speed detection
- Error checking
- Optional push button support
- Position tracking

#### I2C Expanders
- Support for multiple expander types
- Automatic address handling
- Built-in debounce
- Easy configuration

#### Profiles
- Multiple profile support
- Easy profile switching
- Separate callbacks per profile

## Troubleshooting

### Common Issues
1. Matrix Ghosting
   - Verify diode installation
   - Check matrix wiring
   - Ensure proper pullup configuration

2. Encoder Issues
   - Adjust debounce time
   - Verify A/B pin connections
   - Check encoder divisor
   - Monitor encoder validity

3. I2C Expander Issues
   - Verify I2C addresses
   - Check power and pullups
   - Enable internal pullups

## Contributing
1. Fork repository
2. Create feature branch
3. Submit pull request

## Support
- GitHub Issues: Technical issues, bugs
- GitHub Discussions: Questions, ideas
- Email: roncoa@gmail.com

## License
MIT License - See [LICENSE](LICENSE) file

## Version History

### 2.1.0 (27/01/2025)
- Added I2C expander support (MCP23017, PCF8574, PCF8574A)
- Improved encoder handling
- Added ESP32 custom I2C pins support
- Enhanced memory management
- English documentation

### 1.0.1
- Small fixes

### 1.0.0
- Initial release
- Basic matrix support
- Encoder functionality
- Profile management

## Author
roncoa@gmail.com