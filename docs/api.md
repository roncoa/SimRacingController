# SimRacingController API Reference

## Constructor
```cpp
// With encoder buttons
SimRacingController(
    const int* rowPins, int numRows,
    const int* colPins, int numCols,
    const int* encoderPinsA, const int* encoderPinsB, 
    const int* encoderBtnPins,
    int numEncoders,
    int numProfiles,
    unsigned long buttonDebounce = 50,
    unsigned long encoderDebounce = 5
);

// Without encoder buttons
SimRacingController(
    const int* rowPins, int numRows,
    const int* colPins, int numCols,
    const int* encoderPinsA, const int* encoderPinsB,
    int numEncoders,
    int numProfiles,
    unsigned long buttonDebounce = 50,
    unsigned long encoderDebounce = 5
);
```

### Parameters
- `rowPins`: Array of row pin numbers
- `numRows`: Number of rows
- `colPins`: Array of column pin numbers
- `numCols`: Number of columns
- `encoderPinsA`: Array of encoder A pin numbers
- `encoderPinsB`: Array of encoder B pin numbers
- `encoderBtnPins`: Array of encoder button pin numbers (optional)
- `numEncoders`: Number of encoders
- `numProfiles`: Number of profiles
- `buttonDebounce`: Debounce time for buttons in ms (default: 50)
- `encoderDebounce`: Debounce time for encoders in ms (default: 5)

## Initialization
```cpp
void begin();  // Initialize pins and hardware
void update(); // Process inputs (call in loop)
```

## Callbacks
```cpp
// Button events
typedef void (*ButtonCallback)(int profile, int row, int col, bool state);
void setButtonCallback(ButtonCallback callback);

// Encoder rotation
typedef void (*EncoderCallback)(int profile, int encoder, int direction);
void setEncoderCallback(EncoderCallback callback);

// Encoder buttons
typedef void (*EncoderButtonCallback)(int profile, int encoder, bool pressed);
void setEncoderButtonCallback(EncoderButtonCallback callback);
```

### Callback Parameters
- `profile`: Current active profile (0 to numProfiles-1)
- `row`: Matrix row index
- `col`: Matrix column index
- `encoder`: Encoder index
- `direction`: 1 for clockwise, -1 for counter-clockwise
- `state/pressed`: true for press, false for release

## Configuration
```cpp
void setEncoderDivisor(int encoderIndex, int32_t divisor);
void setEncoderPosition(int encoderIndex, int32_t position);
void setProfile(int profile);
```

### Parameters
- `encoderIndex`: Index of encoder (0 to numEncoders-1)
- `divisor`: Encoder sensitivity (1-4, default: 4)
- `position`: Encoder position value
- `profile`: Profile number (0 to numProfiles-1)

## State Queries
```cpp
int getProfile() const;
int32_t getEncoderPosition(int index) const;
int8_t getEncoderDirection(int index) const;
uint16_t getEncoderSpeed(int index) const;
bool getButtonState(int row, int col) const;
bool isEncoderValid(int index) const;
bool getEncoderButtonState(int index) const;
```

## Return Values
- `getProfile`: Current active profile
- `getEncoderPosition`: Current encoder position
- `getEncoderDirection`: Last encoder direction (1/-1)
- `getEncoderSpeed`: Encoder rotation speed
- `getButtonState`: Current button state
- `isEncoderValid`: true if no errors detected
- `getEncoderButtonState`: Current encoder button state

## Memory Usage
- 1 bool per button for state
- 1 bool per button for debounce
- 32-bit counter per encoder
- 8-bit state variable per encoder
