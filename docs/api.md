# SimRacingController API Reference v2.2.0

## Initialization

### Constructor
```cpp
SimRacingController(unsigned long powerSaveTimeoutMs = 300000);  // Default constructor
```

### Configuration Methods
```cpp
// Matrix configuration
void setMatrix(const int* rowPins, int numRows, const int* colPins, int numCols);

// Direct GPIO configuration
void setGpio(const int* pins, int numPins);

// MCP23017 configuration
bool setMcpDevices(const McpConfig* configs, uint8_t numDevices);

// Encoder configuration
void setEncoders(const int* encoderPinsA, const int* encoderPinsB, int numEncoders);  // Without buttons
void setEncoders(const int* encoderPinsA, const int* encoderPinsB,                    // With buttons
                const int* encoderBtnPins, int numEncoders);

// Additional configuration
void setProfiles(int numProfiles);
void setDebounceTime(unsigned long matrixDebounce, unsigned long encoderDebounce);
bool setPowerSaveTimeout(unsigned long timeoutMs);
```

### Configuration Structures
```cpp
struct McpConfig {
    uint8_t address;        // I2C address (0x20-0x27)
    bool usePullups;       // Enable internal pullups
    bool useInterrupts;    // Enable interrupts
    uint8_t intPin;        // Arduino pin for interrupts (-1 if not used)
    
    McpConfig(uint8_t addr = 0x20, bool pullups = true, 
              bool ints = false, uint8_t intPin = -1);
};

struct ControllerError {
    enum ErrorCode {
        NO_ERROR = 0,
        INVALID_PIN = 1,
        PIN_CONFLICT = 2,
        INVALID_CONFIG = 3,
        ENCODER_MALFUNCTION = 4,
        MCP_ERROR = 5,
        I2C_ERROR = 6,
        TIMEOUT_ERROR = 7
    };
    
    ErrorCode code;
    const char* message;
};
```

### Parameters
- `powerSaveTimeoutMs`: Power save timeout in milliseconds (default: 300000)
- `rowPins`: Array of row pin numbers for matrix
- `numRows`: Number of rows in matrix
- `colPins`: Array of column pin numbers for matrix
- `numCols`: Number of columns in matrix
- `pins`: Array of GPIO pin numbers
- `numPins`: Number of GPIO pins
- `configs`: Array of MCP23017 configurations
- `numDevices`: Number of MCP23017 devices (max 8)
- `encoderPinsA`: Array of first pins for each encoder
- `encoderPinsB`: Array of second pins for each encoder
- `encoderBtnPins`: Array of button pins for each encoder (optional)
- `numEncoders`: Number of encoders
- `numProfiles`: Number of available profiles
- `matrixDebounce`: Debounce time for matrix/GPIO/MCP buttons in ms (default: 50)
- `encoderDebounce`: Debounce time for encoders in ms (default: 5)
- `timeoutMs`: Power save timeout in ms (5000-3600000)

### Core Methods
```cpp
bool begin();     // Initialize hardware
void update();    // Standard update (blocking)
bool tryUpdate(); // Non-blocking update
void waitForUpdate(); // Blocking update
```

### Error Handling Methods
```cpp
bool validateConfiguration();
bool validatePins();
void setErrorCallback(bool (*callback)(const ControllerError&));
ControllerError getLastError() const;
void clearError();
```

### Power Management Methods
```cpp
void sleep();
void wake();
bool isInPowerSave() const;
bool isUpdateInProgress() const;
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

// MCP23017 events
typedef void (*McpCallback)(int profile, int device, int pin, bool state);
void setMcpCallback(McpCallback callback);

// Encoder rotation
typedef void (*EncoderCallback)(int profile, int encoder, int direction);
void setEncoderCallback(EncoderCallback callback);

// Encoder buttons
typedef void (*EncoderButtonCallback)(int profile, int encoder, bool pressed);
void setEncoderButtonCallback(EncoderButtonCallback callback);

// Error events
typedef bool (*ErrorCallback)(const ControllerError& error);
void setErrorCallback(ErrorCallback callback);
```

### Callback Parameters
- `profile`: Current active profile (0 to numProfiles-1)
- `row`: Matrix row index
- `col`: Matrix column index
- `gpio`: GPIO pin index
- `device`: MCP23017 device index
- `pin`: MCP23017 pin number (0-15)
- `state`: Button state (true=pressed)
- `encoder`: Encoder index
- `direction`: 1 for clockwise, -1 for counter-clockwise
- `pressed`: true for press, false for release
- `error`: ControllerError structure with error details

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

### Matrix, GPIO and MCP23017
```cpp
bool getMatrixState(int row, int col) const;    // Get matrix button state
bool getGpioState(int gpio) const;              // Get GPIO button state
bool getMcpState(uint8_t device, uint8_t pin) const; // Get MCP pin state
```

### Encoders
```cpp
int32_t getEncoderPosition(int index) const;    // Get current position
int8_t getEncoderDirection(int index) const;    // Get last direction
uint16_t getEncoderSpeed(int index) const;      // Get rotation speed
bool isEncoderValid(int index) const;           // Check for errors
bool getEncoderButtonState(int index) const;    // Get button state
```

### System State
```cpp
int getProfile() const;           // Get current profile
bool isInPowerSave() const;      // Get power save state
bool isUpdateInProgress() const;  // Get update state
ControllerError getLastError() const; // Get last error
```

### Return Values
- `getMatrixState`: Current matrix button state
- `getGpioState`: Current GPIO button state
- `getMcpState`: Current MCP23017 pin state
- `getProfile`: Current active profile
- `getEncoderPosition`: Current encoder position
- `getEncoderDirection`: Last encoder direction (1/-1)
- `getEncoderSpeed`: Encoder rotation speed (steps/second)
- `isEncoderValid`: true if no errors detected
- `getEncoderButtonState`: Current encoder button state
- `isInPowerSave`: true if in power save mode
- `isUpdateInProgress`: true if update is in progress
- `getLastError`: Last error structure

## Memory Usage
- 1 bool per input (matrix/GPIO/MCP) for current state
- 1 bool per input for debounce state
- 1 unsigned long per input for debounce timing
- 32-bit counter per encoder
- 8-bit state variable per encoder
- Additional state variables per encoder for speed and validity
- Error state and callback management
- Power management state
- Thread safety flags
