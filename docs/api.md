# SimRacingController API Reference v2.0.0

## Initialization

### Constructor
```cpp
SimRacingController();  // Default constructor
```

### Configuration Methods
```cpp
// Matrix configuration
void setMatrix(const int* rowPins, int numRows, const int* colPins, int numCols);

// Direct GPIO configuration
void setGpio(const int* pins, int numPins);

// Encoder configuration
void setEncoders(const int* encoderPinsA, const int* encoderPinsB, int numEncoders);  // Without buttons
void setEncoders(const int* encoderPinsA, const int* encoderPinsB,                    // With buttons
                const int* encoderBtnPins, int numEncoders);

// Additional configuration
void setProfiles(int numProfiles);
void setDebounceTime(unsigned long matrixDebounce, unsigned long encoderDebounce);
```

### Parameters
- `rowPins`: Array of row pin numbers for matrix
- `numRows`: Number of rows in matrix
- `colPins`: Array of column pin numbers for matrix
- `numCols`: Number of columns in matrix
- `pins`: Array of GPIO pin numbers
- `numPins`: Number of GPIO pins
- `encoderPinsA`: Array of encoder A pin numbers
- `encoderPinsB`: Array of encoder B pin numbers
- `encoderBtnPins`: Array of encoder button pin numbers (optional)
- `numEncoders`: Number of encoders
- `numProfiles`: Number of profiles
- `matrixDebounce`: Debounce time for matrix/GPIO buttons in ms (default: 50)
- `encoderDebounce`: Debounce time for encoders in ms (default: 5)

### Core Methods
```cpp
void begin();  // Initialize hardware
void update(); // Process inputs (call in loop)
```

## Callbacks

### Callback Types and Setters
```cpp
// Matrix button events
typedef void (*MatrixCallback)(int profile, int row, int col, bool state);
void setMatrixCallback(MatrixCallback callback);

// GPIO button events
typedef void (*GpioCallback)(int profile, int gpio, bool state);
void setGpioCallback(GpioCallback callback);

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
- `gpio`: GPIO pin index
- `encoder`: Encoder index
- `direction`: 1 for clockwise, -1 for counter-clockwise
- `state/pressed`: true for press, false for release

## Configuration Methods
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

### Matrix and GPIO
```cpp
bool getMatrixState(int row, int col) const;  // Get matrix button state
bool getGpioState(int gpio) const;            // Get GPIO button state
```

### Encoders
```cpp
int32_t getEncoderPosition(int index) const;    // Get current position
int8_t getEncoderDirection(int index) const;    // Get last direction
uint16_t getEncoderSpeed(int index) const;      // Get rotation speed
bool isEncoderValid(int index) const;           // Check for errors
bool getEncoderButtonState(int index) const;    // Get button state
```

### Profile
```cpp
int getProfile() const;  // Get current profile
```

### Return Values
- `getMatrixState`: Current matrix button state
- `getGpioState`: Current GPIO button state
- `getProfile`: Current active profile
- `getEncoderPosition`: Current encoder position
- `getEncoderDirection`: Last encoder direction (1/-1)
- `getEncoderSpeed`: Encoder rotation speed (steps/second)
- `isEncoderValid`: true if no errors detected
- `getEncoderButtonState`: Current encoder button state

## Memory Usage
- 1 bool per input (matrix/GPIO) for current state
- 1 bool per input for debounce state
- 1 unsigned long per input for debounce timing
- 32-bit counter per encoder
- 8-bit state variable per encoder
- Additional state variables per encoder for speed and validity
