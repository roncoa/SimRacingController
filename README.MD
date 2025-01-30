# SimRacingController Library

[![GitHub release](https://img.shields.io/github/release/roncoa/SimRacingController.svg)](https://github.com/roncoa/SimRacingController/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

Advanced Arduino library for creating SimRacing button boxes with matrix button, direct GPIO, encoders and MCP23017 I2C expander support. Designed specifically for racing simulator controllers, this library offers a robust and efficient solution for building custom control panels.

## Features
- Button matrix management with configurable debounce
- Direct GPIO button support with debounce
- Rotary encoder support with:
  - Configurable sensitivity (1-4x)
  - Real-time speed detection
  - Error checking and recovery
  - Optional push button support
  - Absolute position tracking
- MCP23017 I2C expander support:
  - Up to 8 devices (128 additional inputs)
  - Configurable internal pullups
  - Optional interrupt support
  - Built-in debounce
- Multiple profiles support
- Event-driven architecture with callbacks
- Power saving mode with configurable timeout
- Thread-safe operations
- Enhanced error handling and reporting
- Efficient memory management
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
- Arduino Uno/Nano (ATmega328P)
- Arduino Mega
- Arduino Leonardo
- Arduino Pro Micro
- ESP8266 based boards
- ESP32 based boards

### Required Components
- Push buttons for matrix/GPIO
- Diodes (1N4148 or similar) for button matrix
- Rotary encoders (optional)
- Pull-up resistors (10kΩ) if not using internal pull-ups
- MCP23017 I2C expanders (optional)

## Basic Usage

```cpp
#include <SimRacingController.h>

// Matrix configuration
const int MATRIX_ROWS = 3;
const int MATRIX_COLS = 3;
const int rowPins[MATRIX_ROWS] = {2, 3, 4};
const int colPins[MATRIX_COLS] = {5, 6, 7};

// GPIO configuration
const int NUM_GPIO = 2;
const int gpioPins[NUM_GPIO] = {8, 9};

// Encoder configuration
const int NUM_ENCODERS = 2;
const int encoderPinsA[NUM_ENCODERS] = {10, 12};
const int encoderPinsB[NUM_ENCODERS] = {11, 13};
const int encoderBtnPins[NUM_ENCODERS] = {14, 15};  // Optional

// MCP23017 configuration
const uint8_t NUM_MCP = 2;
McpConfig mcpConfigs[NUM_MCP] = {
    McpConfig(0x20, true, false),  // Address 0x20, pullups on, no interrupt
    McpConfig(0x21, true, true, 16) // Address 0x21, pullups on, interrupt on pin 16
};

// Create controller
SimRacingController controller;

// Callback functions
void onMatrixChange(int profile, int row, int col, bool state) {
    Serial.printf("Matrix [%d,%d] = %d (Profile %d)\n", row, col, state, profile);
}

void onGpioChange(int profile, int gpio, bool state) {
    Serial.printf("GPIO %d = %d (Profile %d)\n", gpio, state, profile);
}

void onEncoderChange(int profile, int encoder, int direction) {
    Serial.printf("Encoder %d: %s (Profile %d)\n", 
        encoder, direction > 0 ? "CW" : "CCW", profile);
}

void onMcpChange(int profile, int device, int pin, bool state) {
    Serial.printf("MCP %d Pin %d = %d (Profile %d)\n", 
        device, pin, state, profile);
}

void setup() {
    Serial.begin(115200);
    
    // Configure inputs
    controller.setMatrix(rowPins, MATRIX_ROWS, colPins, MATRIX_COLS);
    controller.setGpio(gpioPins, NUM_GPIO);
    controller.setEncoders(encoderPinsA, encoderPinsB, encoderBtnPins, NUM_ENCODERS);
    controller.setMcpDevices(mcpConfigs, NUM_MCP);
    
    // Set callbacks
    controller.setMatrixCallback(onMatrixChange);
    controller.setGpioCallback(onGpioChange);
    controller.setEncoderCallback(onEncoderChange);
    controller.setMcpCallback(onMcpChange);
    
    // Initialize controller
    if (!controller.begin()) {
        Serial.println("Error: " + String(controller.getLastError().message));
        while(1);  // Stop if initialization fails
    }
}

void loop() {
    controller.update();
}
```

## Examples

### Basic
Simple button, encoder and MCP23017 reading with serial output.
- File: `examples/Basic/Basic.ino`

### Advanced
Complete setup showing all features including:
- Matrix and GPIO button handling
- Encoder configuration and reading
- MCP23017 expander support
- Power management
- Error handling
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

### Key Features

#### Button Matrix
- Efficient scanning algorithm
- Configurable debounce
- No ghosting with proper diode configuration
- Independent state tracking
- Active-low logic

#### Direct GPIO
- Simple direct button connection
- Same debounce as matrix
- Active-low logic with pullups
- Ideal for single buttons

#### Encoders
- Configurable sensitivity (1-4x)
- Real-time speed detection
- Error checking and reporting
- Optional push button support
- Absolute position tracking
- Efficient state machine implementation

#### MCP23017 Support
- Up to 8 devices (128 inputs)
- Configurable internal pullups
- Optional interrupt support
- Individual pin debounce
- Error detection and recovery
- Active-low logic
- Efficient I2C communication

#### Power Management
- Automatic sleep mode
- Configurable timeout
- Wake on activity
- Low power consumption
- Pin state preservation

#### Error Handling
- Comprehensive error reporting
- Error callbacks
- Validation checks
- Recovery mechanisms
- Detailed error messages

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

### 2.1.0
- Added enhanced MCP23017 support
- Improved error handling system
- Added power management
- Added thread safety
- Enhanced encoder reliability
- Improved debouncing
- Added communication timeouts
- Enhanced memory management

### 2.0.0
- Added direct GPIO button support
- Improved encoder handling
- Better memory management
- Enhanced documentation

### 1.0.1
- Small fixes

### 1.0.0
- Initial release
- Basic matrix support
- Encoder functionality
- Profile management

## Author
roncoa@gmail.com
