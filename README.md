# SimRacing ButtonBox Library

Arduino library for SimRacing button boxes with matrix buttons and encoders support.

## Features
- Button matrix support
- Rotary encoders with optional buttons
- Multiple profiles
- Hardware debouncing
- Event callbacks
- Configurable encoder sensitivity

## Installation
1. Download as ZIP
2. Arduino IDE -> Sketch -> Include Library -> Add .ZIP Library
3. Select downloaded ZIP

## Hardware Requirements
- Arduino Leonardo/Pro Micro (ATmega32U4)
- Push buttons for matrix
- Rotary encoders (optional)
- Pull-up resistors

## Basic Usage
```cpp
#include "SimRacingController.h"

// Matrix configuration
const int ROW_PINS[] = {2, 3, 4};
const int COL_PINS[] = {5, 6, 7, 8, 9};

// Encoders configuration
const int ENC_PINS_A[] = {20, 18, 14, 10};
const int ENC_PINS_B[] = {21, 19, 15, 16};

// Initialize without encoder buttons
SimRacingController controller(
    ROW_PINS, 3,             // Rows
    COL_PINS, 5,            // Columns
    ENC_PINS_A, ENC_PINS_B, // Encoders
    4,                      // Number of encoders
    1                       // Number of profiles
);

// With encoder buttons
const int ENC_BTN_PINS[] = {22, 23, 24, 25};
SimRacingController controller(
    ROW_PINS, 3,
    COL_PINS, 5,
    ENC_PINS_A, ENC_PINS_B,
    ENC_BTN_PINS,           // Encoder buttons
    4,
    1
);
```

## Examples
- `examples/Basic`: Simple button and encoder reading
- `examples/ACC`: Complete Assetto Corsa Competizione setup
- `examples/AMS2`: Automobilista 2 configuration

See [docs/wiring.md](docs/wiring.md) for hardware setup and [docs/api.md](docs/api.md) for API details.

## License
MIT License - see LICENSE file

## Author
roncoa@gmail.com
