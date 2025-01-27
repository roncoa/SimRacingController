# SimRacingController Library

[![GitHub release](https://img.shields.io/github/release/roncoa/SimRacingController.svg)](https://github.com/roncoa/SimRacingController/releases)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

Advanced Arduino library for creating SimRacing button boxes with matrix button, direct GPIO, and encoder support. Designed specifically for racing simulator controllers, this library offers a flexible and efficient solution for building custom control panels.

## Table of Contents
- [Features](#features)
- [Installation](#installation)
- [Hardware Requirements](#hardware-requirements)
- [Usage](#usage)
  - [Basic Setup](#basic-setup)
  - [Event Handling](#event-handling)
  - [State Reading](#state-reading)
- [Examples](#examples)
- [Documentation](#documentation)
- [Troubleshooting](#troubleshooting)
- [Version History](#version-history)
- [Support](#support)

## Features
- Button matrix management with debounce
- Direct GPIO button support
- Rotary encoder support with configurable sensitivity
- Optional encoder push buttons
- Multiple profiles support
- Event-driven architecture with callbacks
- Efficient memory usage
- Hardware-agnostic design
- Comprehensive error checking and validation
- Real-time encoder speed detection

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
- ESP32 S3 boards

### Required Components
- Push buttons for matrix
- Diodes (1N4148 or similar) for button matrix
- Direct push buttons (optional)
- Rotary encoders (optional)
- Pull-up resistors (if not using internal pull-ups)

## Usage

### Basic Setup
```cpp
#include <SimRacingController.h>

// Matrix configuration
const int MATRIX_ROWS = 3;
const int MATRIX_COLS = 5;
const int rowPins[MATRIX_ROWS] = {2, 3, 4};
const int colPins[MATRIX_COLS] = {5, 6, 7, 8, 9};

// Direct GPIO buttons
const int NUM_GPIO = 3;
const int gpioPins[NUM_GPIO] = {16, 17, 18};

// Encoders configuration
const int NUM_ENCODERS = 4;
const int encoderPinsA[NUM_ENCODERS] = {20, 18, 14, 10};
const int encoderPinsB[NUM_ENCODERS] = {21, 19, 15, 16};
const int encoderBtnPins[NUM_ENCODERS] = {22, 23, 24, 25}; // Optional

SimRacingController controller;

void setup() {
    // Configure components
    controller.setMatrix(rowPins, MATRIX_ROWS, colPins, MATRIX_COLS);
    controller.setGpio(gpioPins, NUM_GPIO);
    
    // Choose one:
    // 1. Encoders with buttons:
    controller.setEncoders(encoderPinsA, encoderPinsB, encoderBtnPins, NUM_ENCODERS);
    // 2. Encoders without buttons:
    // controller.setEncoders(encoderPinsA, encoderPinsB, NUM_ENCODERS);
    
    // Additional configuration
    controller.setProfiles(3);  // Number of profiles
    controller.setDebounceTime(50, 5);  // Matrix/GPIO=50ms, Encoder=5ms
    
    controller.begin();
}

void loop() {
    controller.update();
}
```

### Event Handling
```cpp
// Matrix button events
void onMatrixChange(int profile, int row, int col, bool pressed) {
    // Handle matrix button press/release
}

// Direct GPIO button events
void onGpioChange(int profile, int gpio, bool pressed) {
    // Handle GPIO button press/release
}

// Encoder rotation events
void onEncoderChange(int profile, int encoder, int direction) {
    // Get additional encoder info
    int32_t position = controller.getEncoderPosition(encoder);
    uint16_t speed = controller.getEncoderSpeed(encoder);
    bool isValid = controller.isEncoderValid(encoder);
}

// Encoder button events
void onEncoderButtonChange(int profile, int encoder, bool pressed) {
    // Handle encoder button press/release
}

void setup() {
    // ... configuration code ...
    controller.setMatrixCallback(onMatrixChange);
    controller.setGpioCallback(onGpioChange);
    controller.setEncoderCallback(onEncoderChange);
    controller.setEncoderButtonCallback(onEncoderButtonChange);
    controller.begin();
}
```

### State Reading
```cpp
// Matrix buttons
bool matrixState = controller.getMatrixState(row, col);

// GPIO buttons
bool gpioState = controller.getGpioState(gpio);

// Encoder information
int32_t position = controller.getEncoderPosition(encoder);
int8_t direction = controller.getEncoderDirection(encoder);
uint16_t speed = controller.getEncoderSpeed(encoder);
bool isValid = controller.isEncoderValid(encoder);
bool btnState = controller.getEncoderButtonState(encoder);
```

## Examples

### Basic Example
Basic button, GPIO, and encoder reading with serial output.
File: `examples/Basic/Basic.ino`

### Advanced Example
Complete setup showing all features including profile management and callbacks.
File: `examples/Advanced/Advanced.ino`

### ACC Button Box
Complete setup for Assetto Corsa Competizione.
File: `examples/ButtonBox_ACC/ButtonBox_ACC.ino`

Requirements:
- KeySequence library
- ACC shortcuts configuration

## Documentation

### Detailed Guides
API Reference
Hardware Wiring Guide

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

#### Profiles
- Multiple profile support
- Easy profile switching
- Separate callbacks per profile

## Troubleshooting

### Common Issues

1. Button Ghosting
   - Verify diode installation
   - Check matrix wiring

2. Encoder Issues
   - Adjust debounce time
   - Verify A/B pin connections
   - Check encoder divisor

3. Matrix/GPIO Not Responding
   - Verify pin modes
   - Check pull-up resistors
   - Confirm connections

## Version History

- 2.0.0
  - Added direct GPIO button support
  - Improved encoder handling
  - Better memory management
  - Enhanced documentation
- 1.0.1
  - Small fixes
- 1.0.0
  - Initial release

## Support
- GitHub Issues: Technical issues, bugs
- GitHub Discussions: Questions, ideas
- Email: roncoa@gmail.com

## License
MIT License - See LICENSE file

## Author
roncoa@gmail.com
