/**************************
 * SimRacingController.h
 * v 2.1.0
 * by roncoa@gmail.com
 * 27/01/2025
 **************************/

#ifndef SIMRACING_CONTROLLER_H
#define SIMRACING_CONTROLLER_H

#include <Arduino.h>
#include <Wire.h>

// Platform-specific features
#ifdef ARDUINO_ARCH_ESP32
    #define SUPPORT_CUSTOM_I2C_PINS
#endif

/**
 * Matrix configuration structure
 * Contains all necessary parameters for button matrix initialization
 */
struct MatrixConfig {
    const int* rowPins;      // Array of row pin numbers
    const int* colPins;      // Array of column pin numbers
    int numRows;            // Number of rows in the matrix
    int numCols;            // Number of columns in the matrix
    
    MatrixConfig(const int* _rowPins, int _numRows, const int* _colPins, int _numCols) :
        rowPins(_rowPins), colPins(_colPins), numRows(_numRows), numCols(_numCols) {}
};

/**
 * I2C Expander types
 * Supported expander models
 */
enum ExpanderType {
    EXPANDER_NONE = 0,
    EXPANDER_MCP23017,    // 16-bit I/O expander, base address 0x20
    EXPANDER_PCF8574,     // 8-bit I/O expander, base address 0x20
    EXPANDER_PCF8574A     // 8-bit I/O expander, base address 0x38
};

class SimRacingController {
private:
    // Matrix buttons
    const int numRows;
    const int numCols;
    const int* rowPins;
    const int* colPins;
    bool** lastMatrixStates;
    bool** matrixStates;
    unsigned long** lastMatrixDebounceTime;
    const unsigned long matrixDebounceDelay;

    // Direct GPIO
    const int* gpioPins;
    bool* lastGpioStates;
    bool* gpioStates;
    unsigned long* gpioDebounceTime;
    const int numGpio;

    // I2C Expander
    const int numExpanders;
    ExpanderType expanderType;
    uint8_t* expanderAddresses;
    uint8_t* expanderStates;
    uint8_t* lastExpanderStates;
    unsigned long* expanderDebounceTime;
    bool wireInitialized;
    #ifdef SUPPORT_CUSTOM_I2C_PINS
    int sdaPin;
    int sclPin;
    #endif

    // Encoder configuration structure
    struct EncoderConfig {
        int pinA;                  // First encoder pin
        int pinB;                  // Second encoder pin
        int pinBtn;               // Encoder button pin (-1 if not used)
        uint8_t lastState;        // Previous encoder state
        int8_t encDir;           // Current rotation direction
        int32_t position;        // Current position
        unsigned long lastTime;   // Last update time
        unsigned long lastBtnTime; // Last button state change time
        bool lastBtnState;        // Previous button state
        bool btnState;           // Current button state
        int32_t divisor;         // Position increment divisor (1-4)
        int8_t lastDirection;    // Last recorded direction
        uint32_t errorCount;     // Error counter for validity check
        bool valid;              // Encoder validity flag
        uint16_t speed;          // Rotation speed
        unsigned long lastChangeTime; // Last position change time

        EncoderConfig() : 
            pinA(0), pinB(0), pinBtn(-1),
            lastState(0), encDir(0),
            position(0), lastTime(0), lastBtnTime(0),
            lastBtnState(false), btnState(false),
            divisor(4), lastDirection(0), errorCount(0),
            valid(true), speed(0), lastChangeTime(0) {}
    };

    struct EncoderInitConfig {
        const int* pinsA;        // Array of first pins
        const int* pinsB;        // Array of second pins
        const int* btnPins;      // Array of button pins (can be nullptr)
        int count;              // Number of encoders

        EncoderInitConfig(const int* _pinsA, const int* _pinsB, const int* _btnPins, int _count) :
            pinsA(_pinsA), pinsB(_pinsB), btnPins(_btnPins), count(_count) {}
    };
    
    const int numEncoders;
    EncoderConfig* encoders;
    const unsigned long encoderDebounceTime;

    // Profiles
    int currentProfile;
    const int numProfiles;

    // Private methods
    void initializeArrays();
    void cleanupArrays();
    void updateEncoder(int index);
    void processMatrixPress(int row, int col, bool state);
    void configureMatrix(const MatrixConfig& config);
    void configureEncoders(const EncoderInitConfig& config);
    void initializeExpander(uint8_t address);
    void updateExpanders();
    uint8_t readExpander(uint8_t address);

public:
    // Constructor and Destructor
    SimRacingController();
    ~SimRacingController();

    // Configuration Methods
    void setMatrix(const int* rowPins, int numRows, const int* colPins, int numCols);
    void setGpio(const int* pins, int numPins);
    void setExpander(int sda, int scl, ExpanderType type, int numExpanders);
    void setEncoders(const int* encoderPinsA, const int* encoderPinsB, int numEncoders);
    void setEncoders(const int* encoderPinsA, const int* encoderPinsB, 
                    const int* encoderBtnPins, int numEncoders);
    void setProfiles(int numProfiles);
    void setDebounceTime(unsigned long matrixDebounce, unsigned long encoderDebounce);

    // Core Methods
    void begin();    // Initialize hardware
    void update();   // Process all inputs

    // Encoder Configuration
    void setEncoderDivisor(int encoderIndex, int32_t divisor);
    void setEncoderPosition(int encoderIndex, int32_t position);
    
    // Profile Management
    void setProfile(int profile);
    int getProfile() const;
    
    // State Queries
    bool getMatrixState(int row, int col) const;
    bool getGpioState(int gpio) const;
    uint8_t getExpanderState(int expander) const;
    int32_t getEncoderPosition(int index) const;
    int8_t getEncoderDirection(int index) const;
    uint16_t getEncoderSpeed(int index) const;
    bool isEncoderValid(int index) const;
    bool getEncoderButtonState(int index) const;
    
    // Callback Types
    typedef void (*MatrixCallback)(int profile, int row, int col, bool state);
    typedef void (*GpioCallback)(int profile, int gpio, bool state);
    typedef void (*ExpanderCallback)(int profile, int expander, uint8_t state);
    typedef void (*EncoderCallback)(int profile, int encoder, int direction);
    typedef void (*EncoderButtonCallback)(int profile, int encoder, bool pressed);
    
    // Callback Setters
    void setMatrixCallback(MatrixCallback callback);
    void setGpioCallback(GpioCallback callback);
    void setExpanderCallback(ExpanderCallback callback);
    void setEncoderCallback(EncoderCallback callback);
    void setEncoderButtonCallback(EncoderButtonCallback callback);

private:
    // Callback Members
    MatrixCallback onMatrixChange;
    GpioCallback onGpioChange;
    ExpanderCallback onExpanderChange;
    EncoderCallback onEncoderChange;
    EncoderButtonCallback onEncoderButtonChange;
};

#endif