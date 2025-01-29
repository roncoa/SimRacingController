/**************************
   SimRacingController.h
   v 2.1.0
   by roncoa@gmail.com
   29/01/2025
   
   Extended version with:
   - Enhanced error handling
   - Thread safety
   - Power management
   - MCP23017 support
 **************************/

#ifndef SIMRACING_CONTROLLER_H
#define SIMRACING_CONTROLLER_H

#include <Arduino.h>
#include <Wire.h>

// MCP23017 registers
#define MCP23017_IODIRA     0x00   // IO direction A
#define MCP23017_IODIRB     0x01   // IO direction B
#define MCP23017_IPOLA      0x02   // Input polarity A
#define MCP23017_IPOLB      0x03   // Input polarity B
#define MCP23017_GPINTENA   0x04   // Interrupt enable A
#define MCP23017_GPINTENB   0x05   // Interrupt enable B
#define MCP23017_DEFVALA    0x06   // Default value A
#define MCP23017_DEFVALB    0x07   // Default value B
#define MCP23017_INTCONA    0x08   // Interrupt control A
#define MCP23017_INTCONB    0x09   // Interrupt control B
#define MCP23017_IOCONA     0x0A   // IO config A
#define MCP23017_IOCONB     0x0B   // IO config B
#define MCP23017_GPPUA      0x0C   // Pullup A
#define MCP23017_GPPUB      0x0D   // Pullup B
#define MCP23017_INTFA      0x0E   // Interrupt flag A
#define MCP23017_INTFB      0x0F   // Interrupt flag B
#define MCP23017_INTCAPA    0x10   // Interrupt capture A
#define MCP23017_INTCAPB    0x11   // Interrupt capture B
#define MCP23017_GPIOA      0x12   // Port A
#define MCP23017_GPIOB      0x13   // Port B

// I2C timeouts and limits
#define I2C_TIMEOUT_MS      100    // I2C operation timeout
#define MIN_POWER_SAVE_MS   5000   // Minimum power save timeout
#define MAX_POWER_SAVE_MS   3600000 // Maximum power save timeout (1 hour)
#define MAX_ERROR_COUNT     100    // Maximum encoder error count before error

// Error reporting structure
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
    
    ControllerError(ErrorCode c = NO_ERROR, const char* msg = "") : 
        code(c), message(msg) {}
};

/*
   Matrix configuration structure
*/
struct MatrixConfig {
    const int* rowPins;
    const int* colPins;
    int numRows;
    int numCols;

    MatrixConfig(const int* _rowPins, int _numRows, const int* _colPins, int _numCols) :
        rowPins(_rowPins), colPins(_colPins), numRows(_numRows), numCols(_numCols) {}
};

/*
   MCP23017 configuration structure
*/
struct McpConfig {
    uint8_t address;
    bool usePullups;
    bool useInterrupts;
    uint8_t intPin;
    
    McpConfig(uint8_t addr = 0x20, bool pullups = true, bool ints = false, uint8_t intPin = -1) :
        address(addr), usePullups(pullups), useInterrupts(ints), intPin(intPin) {}
};

class SimRacingController {
    private:
        // Thread safety
        volatile bool isUpdating;
        
        // Power management
        bool isPowerSaving;
        unsigned long lastActivityTime;
        const unsigned long powerSaveTimeout;
        
        // Error handling
        ControllerError lastError;
        bool (*errorCallback)(const ControllerError&);
        bool errorReported;  // Flag for preventing duplicate error reports

        // Button Matrix
        const int numRows;
        const int numCols;
        const int* rowPins;
        const int* colPins;
        bool** lastMatrixStates;
        bool** matrixStates;
        unsigned long** lastMatrixDebounceTime;
        const unsigned long matrixDebounceDelay;

        // Direct GPIO Buttons
        const int* gpioPins;
        bool* lastGpioStates;
        bool* gpioStates;
        unsigned long* gpioDebounceTime;
        const int numGpio;

        // MCP23017 support
        static const uint8_t MAX_MCP_DEVICES = 8;
        McpConfig* mcpConfigs;
        uint8_t numMcpDevices;
        uint16_t* lastMcpStates;
        uint16_t* mcpStates;
        unsigned long* mcpDebounceTime;
        bool mcpInitialized;

        // Encoder Configuration Structure
        struct EncoderConfig {
            int pinA;
            int pinB;
            int pinBtn;
            uint8_t lastState;
            int8_t encDir;
            int32_t position;
            unsigned long lastTime;
            unsigned long lastBtnTime;
            bool lastBtnState;
            bool btnState;
            int32_t divisor;
            int8_t lastDirection;
            uint32_t errorCount;
            bool valid;
            uint16_t speed;
            unsigned long lastChangeTime;
            bool errorReported;

            EncoderConfig() :
                pinA(0), pinB(0), pinBtn(-1),
                lastState(0), encDir(0),
                position(0), lastTime(0), lastBtnTime(0),
                lastBtnState(false), btnState(false),
                divisor(4), lastDirection(0), errorCount(0),
                valid(true), speed(0), lastChangeTime(0),
                errorReported(false) {}
        };

        // Encoder Initialization Configuration
        struct EncoderInitConfig {
            const int* pinsA;
            const int* pinsB;
            const int* btnPins;
            int count;

            EncoderInitConfig(const int* _pinsA, const int* _pinsB, const int* _btnPins, int _count) :
                pinsA(_pinsA), pinsB(_pinsB), btnPins(_btnPins), count(_count) {}
        };

        // Encoder members
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
        
        // MCP private methods
        bool initializeMcp(uint8_t device);
        void updateMcp(uint8_t device);
        void processMcpChange(uint8_t device, int pin, bool state);
        bool writeMcpRegister(uint8_t device, uint8_t reg, uint8_t value);
        bool readMcpRegister(uint8_t device, uint8_t reg, uint8_t& value);
        bool readMcpPorts(uint8_t device, uint16_t& value);

        // I2C helper methods
        bool waitForI2C(unsigned long startTime) const;
        bool checkI2CError(uint8_t error);

    public:
        SimRacingController(unsigned long powerSaveTimeoutMs = 300000);
        ~SimRacingController();

        // Configuration Methods
        void setMatrix(const int* rowPins, int numRows, const int* colPins, int numCols);
        void setGpio(const int* pins, int numPins);
        void setEncoders(const int* encoderPinsA, const int* encoderPinsB, int numEncoders);
        void setEncoders(const int* encoderPinsA, const int* encoderPinsB,
                        const int* encoderBtnPins, int numEncoders);
        bool setMcpDevices(const McpConfig* configs, uint8_t numDevices);
        void setProfiles(int numProfiles);
        void setDebounceTime(unsigned long matrixDebounce, unsigned long encoderDebounce);

        // Enhanced Configuration Methods
        bool begin();
        bool validateConfiguration();
        bool validatePins();
        void setErrorCallback(bool (*callback)(const ControllerError&));
        ControllerError getLastError() const;
        void clearError();

        // Core Methods
        void update();
        bool tryUpdate();
        void waitForUpdate();

        // Power Management Methods
        bool setPowerSaveTimeout(unsigned long timeoutMs);
        void sleep();
        void wake();
        bool isInPowerSave() const;
        bool isUpdateInProgress() const;

        // Encoder Configuration
        void setEncoderDivisor(int encoderIndex, int32_t divisor);
        void setEncoderPosition(int encoderIndex, int32_t position);

        // Profile Management
        void setProfile(int profile);
        int getProfile() const;

        // State Getters
        int32_t getEncoderPosition(int index) const;
        int8_t getEncoderDirection(int index) const;
        uint16_t getEncoderSpeed(int index) const;
        bool getMatrixState(int row, int col) const;
        bool getGpioState(int gpio) const;
        bool getMcpState(uint8_t device, uint8_t pin) const;
        bool isEncoderValid(int index) const;
        bool getEncoderButtonState(int index) const;

        // Callback Types
        typedef void (*MatrixCallback)(int profile, int row, int col, bool state);
        typedef void (*GpioCallback)(int profile, int gpio, bool state);
        typedef void (*EncoderCallback)(int profile, int encoder, int direction);
        typedef void (*EncoderButtonCallback)(int profile, int encoder, bool pressed);
        typedef void (*McpCallback)(int profile, int device, int pin, bool state);

        // Callback Setters
        void setMatrixCallback(MatrixCallback callback);
        void setGpioCallback(GpioCallback callback);
        void setEncoderCallback(EncoderCallback callback);
        void setEncoderButtonCallback(EncoderButtonCallback callback);
        void setMcpCallback(McpCallback callback);

    private:
        // Callback members
        MatrixCallback onMatrixChange;
        GpioCallback onGpioChange;
        EncoderCallback onEncoderChange;
        EncoderButtonCallback onEncoderButtonChange;
        McpCallback onMcpChange;
};

#endif
