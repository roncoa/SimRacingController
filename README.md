# SimRacingController Library

Advanced Arduino library for creating SimRacing button boxes with matrix button and encoder support. Designed specifically for racing simulator controllers, this library offers a flexible and efficient solution for building custom control panels.

## Features
- Button matrix management with debounce
- Rotary encoder support with configurable sensitivity
- Optional encoder push buttons
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

### Required Components
- Push buttons for matrix
- Diodes (1N4148 or similar) for button matrix
- Rotary encoders (optional)
- Pull-up resistors (if not using internal pull-ups)
- LED and resistor for status (optional)
- Push button for profile switching (optional)

## Basic Usage

### Simple Example
```cpp
#include <SimRacingController.h>

// Matrix configuration
const int ROW_PINS[] = {2, 3, 4};
const int COL_PINS[] = {5, 6, 7, 8, 9};

// Encoders configuration
const int ENC_PINS_A[] = {20, 18, 14, 10};
const int ENC_PINS_B[] = {21, 19, 15, 16};

// Without encoder buttons
SimRacingController controller(
    ROW_PINS, 3,             // Rows
    COL_PINS, 5,            // Columns
    ENC_PINS_A, ENC_PINS_B, // Encoders
    4,                      // Number of encoders
    1,                      // Number of profiles
    50,                     // Buttons debounce
    5                       // Encoders debounce
);

void setup() {
    controller.begin();
}

void loop() {
    controller.update();
}
```

### With Encoder Buttons
```cpp
const int ENC_BTN_PINS[] = {22, 23, 24, 25};

SimRacingController controller(
    ROW_PINS, 3,
    COL_PINS, 5,
    ENC_PINS_A, ENC_PINS_B,
    ENC_BTN_PINS,           // Encoder buttons
    4,                      // Number of encoders
    1                       // Number of profiles
);
```

### Event Handling
```cpp
void onButtonEvent(int profile, int row, int col, bool pressed) {
    // Handle button press/release
}

void onEncoderEvent(int profile, int encoder, int direction) {
    // Handle encoder rotation
}

void onEncoderButtonEvent(int profile, int encoder, bool pressed) {
    // Handle encoder button press/release
}

void setup() {
    controller.setButtonCallback(onButtonEvent);
    controller.setEncoderCallback(onEncoderEvent);
    controller.setEncoderButtonCallback(onEncoderButtonEvent);
    controller.begin();
}
```

## Examples
The library includes two examples:

### Basic
Simple button and encoder reading with serial output.
- File: `examples/Basic/Basic.ino`

### ButtonBox_ACC (Assetto Corsa Competizione)
Complete setup for ACC with:
- Button mappings for common functions
- Encoder settings for TC, ABS, etc.

Requirements:
- KeySequence library (https://github.com/roncoa/KeySequence)
- ACC shorcut: `Sequenze.h`

## Documentation

### Detailed Guides
- [API Reference](docs/api.md)
- [Hardware Wiring Guide](docs/wiring.md)
- [Examples Documentation](docs/examples.md)

### Key Concepts

#### Button Matrix
- Efficient scanning algorithm
- Built-in debounce
- No ghosting with proper diode configuration

#### Encoders
- Configurable sensitivity (1-4x)
- Speed detection
- Error checking
- Optional push button support

#### Profiles
- Multiple profile support
- Easy profile switching
- LED status indication

## Troubleshooting

### Common Issues
1. Button Ghosting
   - Verify diode installation
   - Check matrix wiring

2. Encoder Issues
   - Adjust debounce time
   - Verify A/B pin connections
   - Check encoder divisor setting

3. Matrix Not Responding
   - Verify pin modes
   - Check pull-up resistors
   - Confirm row/column connections

## Contributing
1. Fork repository
2. Create feature branch
3. Submit pull request

## Support
- GitHub Issues: Technical issues, bugs
- GitHub Discussions: Questions, ideas
- Email: roncoa@gmail.com

## License
MIT License - See LICENSE file

## Version History
- 1.0.0 (2025-01-22)
  - Initial release
  - Basic matrix support
  - Encoder functionality
  - Profile management

## Author
roncoa@gmail.com
