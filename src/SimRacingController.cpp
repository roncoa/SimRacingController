/**************************
   SimRacingController.cpp
   v 2.2.0
   by roncoa@gmail.com
   29/01/2025
   
   Extended version with:
   - Enhanced error handling
   - Thread safety
   - Power management
   - MCP23017 support
 **************************/

#include "SimRacingController.h"

/*
   Constructor - Initializes all variables to safe default values
*/
SimRacingController::SimRacingController(unsigned long powerSaveTimeoutMs) :
    // Thread safety
    isUpdating(false),
    
    // Power management
    isPowerSaving(false),
    lastActivityTime(0),
    powerSaveTimeout(powerSaveTimeoutMs),
    
    // Error handling
    lastError(ControllerError::NO_ERROR),
    errorCallback(nullptr),
    errorReported(false),

    // Button Matrix
    numRows(0),
    numCols(0),
    rowPins(nullptr),
    colPins(nullptr),
    lastMatrixStates(nullptr),
    matrixStates(nullptr),
    lastMatrixDebounceTime(nullptr),
    matrixDebounceDelay(50),

    // Direct GPIO
    gpioPins(nullptr),
    lastGpioStates(nullptr),
    gpioStates(nullptr),
    gpioDebounceTime(nullptr),
    numGpio(0),

    // MCP23017
    mcpConfigs(nullptr),
    numMcpDevices(0),
    lastMcpStates(nullptr),
    mcpStates(nullptr),
    mcpDebounceTime(nullptr),
    mcpInitialized(false),

    // Encoders
    numEncoders(0),
    encoders(nullptr),
    encoderDebounceTime(5),

    // Profiles
    currentProfile(0),
    numProfiles(1),

    // Callbacks
    onMatrixChange(nullptr),
    onGpioChange(nullptr),
    onEncoderChange(nullptr),
    onEncoderButtonChange(nullptr),
    onMcpChange(nullptr) {}

/*
   Destructor - Ensures proper cleanup of allocated memory
*/
SimRacingController::~SimRacingController() {
    cleanupArrays();
    delete[] encoders;
    delete[] lastGpioStates;
    delete[] gpioStates;
    delete[] gpioDebounceTime;
    delete[] mcpConfigs;
    delete[] lastMcpStates;
    delete[] mcpStates;
    delete[] mcpDebounceTime;
}

/*
   I2C Helper Methods
*/
bool SimRacingController::waitForI2C(unsigned long startTime) const {
    while (Wire.available() == 0) {
        if (millis() - startTime > I2C_TIMEOUT_MS) {
            return false;
        }
    }
    return true;
}

bool SimRacingController::checkI2CError(uint8_t error) {
    switch (error) {
        case 0:
            return true;
        case 1:
            lastError = ControllerError(ControllerError::I2C_ERROR, "Data too long");
            break;
        case 2:
            lastError = ControllerError(ControllerError::I2C_ERROR, "Address NACK");
            break;
        case 3:
            lastError = ControllerError(ControllerError::I2C_ERROR, "Data NACK");
            break;
        case 4:
            lastError = ControllerError(ControllerError::I2C_ERROR, "Other error");
            break;
    }
    if (errorCallback && !errorReported) {
        errorCallback(lastError);
        errorReported = true;
    }
    return false;
}

/*
   MCP23017 Methods
*/
bool SimRacingController::writeMcpRegister(uint8_t device, uint8_t reg, uint8_t value) {
    if (device >= numMcpDevices) {
        lastError = ControllerError(ControllerError::INVALID_CONFIG, "Invalid MCP device");
        return false;
    }

    Wire.beginTransmission(mcpConfigs[device].address);
    Wire.write(reg);
    Wire.write(value);
    return checkI2CError(Wire.endTransmission());
}

bool SimRacingController::readMcpRegister(uint8_t device, uint8_t reg, uint8_t& value) {
    if (device >= numMcpDevices) {
        lastError = ControllerError(ControllerError::INVALID_CONFIG, "Invalid MCP device");
        return false;
    }

    Wire.beginTransmission(mcpConfigs[device].address);
    Wire.write(reg);
    if (!checkI2CError(Wire.endTransmission())) return false;

    unsigned long startTime = millis();
    Wire.requestFrom(mcpConfigs[device].address, (uint8_t)1);
    if (!waitForI2C(startTime)) {
        lastError = ControllerError(ControllerError::TIMEOUT_ERROR, "I2C read timeout");
        return false;
    }

    value = Wire.read();
    return true;
}

bool SimRacingController::readMcpPorts(uint8_t device, uint16_t& value) {
    if (device >= numMcpDevices) {
        lastError = ControllerError(ControllerError::INVALID_CONFIG, "Invalid MCP device");
        return false;
    }

    Wire.beginTransmission(mcpConfigs[device].address);
    Wire.write(MCP23017_GPIOA);
    if (!checkI2CError(Wire.endTransmission())) return false;

    unsigned long startTime = millis();
    Wire.requestFrom(mcpConfigs[device].address, (uint8_t)2);
    if (!waitForI2C(startTime)) {
        lastError = ControllerError(ControllerError::TIMEOUT_ERROR, "I2C read timeout");
        return false;
    }

    value = Wire.read() | (Wire.read() << 8);
    return true;
}

bool SimRacingController::initializeMcp(uint8_t device) {
    if (device >= numMcpDevices) return false;

    const McpConfig& config = mcpConfigs[device];

    // Reset the MCP23017
    if (!writeMcpRegister(device, MCP23017_IOCONA, 0x00) ||
        !writeMcpRegister(device, MCP23017_IOCONB, 0x00))
        return false;

    // Set all pins as inputs
    if (!writeMcpRegister(device, MCP23017_IODIRA, 0xFF) ||
        !writeMcpRegister(device, MCP23017_IODIRB, 0xFF))
        return false;

    // Enable pullups if configured
    if (config.usePullups) {
        if (!writeMcpRegister(device, MCP23017_GPPUA, 0xFF) ||
            !writeMcpRegister(device, MCP23017_GPPUB, 0xFF))
            return false;
    }

    // Configure interrupts if enabled
    if (config.useInterrupts && config.intPin >= 0) {
        pinMode(config.intPin, INPUT_PULLUP);
        
        if (!writeMcpRegister(device, MCP23017_GPINTENA, 0xFF) ||
            !writeMcpRegister(device, MCP23017_GPINTENB, 0xFF) ||
            !writeMcpRegister(device, MCP23017_INTCONA, 0x00) ||
            !writeMcpRegister(device, MCP23017_INTCONB, 0x00))
            return false;
    }

    // Enable sequential mode and byte mode
    return writeMcpRegister(device, MCP23017_IOCONA, 0x20);
}

/*
   Configuration Methods
*/
void SimRacingController::setMatrix(const int* rowPins, int numRows,
                                    const int* colPins, int numCols) {
    MatrixConfig config(rowPins, numRows, colPins, numCols);
    configureMatrix(config);
}

void SimRacingController::configureMatrix(const MatrixConfig& config) {
    cleanupArrays();

    const_cast<int&>(numRows) = config.numRows;
    const_cast<int&>(numCols) = config.numCols;
    const_cast<int*&>(rowPins) = const_cast<int*>(config.rowPins);
    const_cast<int*&>(colPins) = const_cast<int*>(config.colPins);

    initializeArrays();
}

void SimRacingController::setGpio(const int* pins, int numPins) {
    delete[] lastGpioStates;
    delete[] gpioStates;
    delete[] gpioDebounceTime;

    const_cast<int*&>(gpioPins) = const_cast<int*>(pins);
    const_cast<int&>(numGpio) = numPins;

    lastGpioStates = new bool[numPins]();
    gpioStates = new bool[numPins]();
    gpioDebounceTime = new unsigned long[numPins]();
}

bool SimRacingController::setMcpDevices(const McpConfig* configs, uint8_t numDevices) {
    if (numDevices > MAX_MCP_DEVICES || !configs) {
        lastError = ControllerError(ControllerError::INVALID_CONFIG, "Invalid MCP config");
        return false;
    }

    delete[] mcpConfigs;
    delete[] lastMcpStates;
    delete[] mcpStates;
    delete[] mcpDebounceTime;

    numMcpDevices = numDevices;
    mcpConfigs = new McpConfig[numDevices];
    lastMcpStates = new uint16_t[numDevices]();
    mcpStates = new uint16_t[numDevices]();
    mcpDebounceTime = new unsigned long[numDevices * 16]();

    for (uint8_t i = 0; i < numDevices; i++) {
        mcpConfigs[i] = configs[i];
    }

    mcpInitialized = false;
    return true;
}

void SimRacingController::setEncoders(const int* encoderPinsA, const int* encoderPinsB,
                                      int numEncoders) {
    setEncoders(encoderPinsA, encoderPinsB, nullptr, numEncoders);
}

void SimRacingController::setEncoders(const int* encoderPinsA, const int* encoderPinsB,
                                      const int* encoderBtnPins, int numEncoders) {
    EncoderInitConfig config(encoderPinsA, encoderPinsB, encoderBtnPins, numEncoders);
    configureEncoders(config);
}

void SimRacingController::configureEncoders(const EncoderInitConfig& config) {
    delete[] encoders;

    const_cast<int&>(numEncoders) = config.count;

    encoders = new EncoderConfig[config.count];
    for (int i = 0; i < config.count; i++) {
        encoders[i].pinA = config.pinsA[i];
        encoders[i].pinB = config.pinsB[i];
        encoders[i].pinBtn = config.btnPins ? config.btnPins[i] : -1;
        encoders[i].divisor = 4;
        encoders[i].valid = true;
    }
}

void SimRacingController::initializeArrays() {
    lastMatrixStates = new bool*[numRows];
    matrixStates = new bool*[numRows];
    lastMatrixDebounceTime = new unsigned long*[numRows];

    for (int i = 0; i < numRows; i++) {
        lastMatrixStates[i] = new bool[numCols]();
        matrixStates[i] = new bool[numCols]();
        lastMatrixDebounceTime[i] = new unsigned long[numCols]();
    }
}

void SimRacingController::cleanupArrays() {
    if (lastMatrixStates) {
        for (int i = 0; i < numRows; i++) {
            delete[] lastMatrixStates[i];
            delete[] matrixStates[i];
            delete[] lastMatrixDebounceTime[i];
        }
        delete[] lastMatrixStates;
        delete[] matrixStates;
        delete[] lastMatrixDebounceTime;

        lastMatrixStates = nullptr;
        matrixStates = nullptr;
        lastMatrixDebounceTime = nullptr;
    }
}

/*
   Configuration Validation
*/
bool SimRacingController::validatePins() {
    // Matrix pins validation
    for (int i = 0; i < numRows; i++) {
        if (rowPins[i] < 0 || rowPins[i] >= NUM_DIGITAL_PINS) {
            lastError = ControllerError(ControllerError::INVALID_PIN, "Invalid matrix row pin");
            return false;
        }
    }
    
    for (int i = 0; i < numCols; i++) {
        if (colPins[i] < 0 || colPins[i] >= NUM_DIGITAL_PINS) {
            lastError = ControllerError(ControllerError::INVALID_PIN, "Invalid matrix column pin");
            return false;
        }
    }

    // GPIO pins validation
    for (int i = 0; i < numGpio; i++) {
        if (gpioPins[i] < 0 || gpioPins[i] >= NUM_DIGITAL_PINS) {
            lastError = ControllerError(ControllerError::INVALID_PIN, "Invalid GPIO pin");
            return false;
        }
    }

    // Encoder pins validation
    for (int i = 0; i < numEncoders; i++) {
        if (encoders[i].pinA < 0 || encoders[i].pinA >= NUM_DIGITAL_PINS ||
            encoders[i].pinB < 0 || encoders[i].pinB >= NUM_DIGITAL_PINS) {
            lastError = ControllerError(ControllerError::INVALID_PIN, "Invalid encoder pin");
            return false;
        }
        
        if (encoders[i].pinBtn >= 0 && encoders[i].pinBtn >= NUM_DIGITAL_PINS) {
            lastError = ControllerError(ControllerError::INVALID_PIN, "Invalid encoder button pin");
            return false;
        }
    }

    return true;
}

bool SimRacingController::validateConfiguration() {
    if (numRows > 0 && numCols > 0) {
        if (!rowPins || !colPins) {
            lastError = ControllerError(ControllerError::INVALID_CONFIG, "Matrix pins not configured");
            return false;
        }
    }
    
    if (numGpio > 0 && !gpioPins) {
        lastError = ControllerError(ControllerError::INVALID_CONFIG, "GPIO pins not configured");
        return false;
    }
    
    if (numEncoders > 0 && !encoders) {
        lastError = ControllerError(ControllerError::INVALID_CONFIG, "Encoders not configured");
        return false;
    }
    
    if (numMcpDevices > 0 && !mcpConfigs) {
        lastError = ControllerError(ControllerError::INVALID_CONFIG, "MCP devices not configured");
        return false;
    }
    
    return validatePins();
}

/*
   Initialization
*/
bool SimRacingController::begin() {
    clearError();
    
    if (!validateConfiguration()) {
        return false;
    }

    // Initialize I2C if MCP devices are configured
    if (numMcpDevices > 0) {
        Wire.begin();
        Wire.setClock(400000);  // Set I2C clock to 400kHz

        // Initialize each MCP device
        for (uint8_t i = 0; i < numMcpDevices; i++) {
            if (!initializeMcp(i)) {
                lastError = ControllerError(ControllerError::MCP_ERROR, "Failed to initialize MCP");
                return false;
            }
        }
        mcpInitialized = true;
    }

    // Configure matrix pins
    for (int i = 0; i < numRows; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);
    }
    for (int i = 0; i < numCols; i++) {
        pinMode(colPins[i], INPUT_PULLUP);
    }

    // Configure GPIO pins
    for (int i = 0; i < numGpio; i++) {
        pinMode(gpioPins[i], INPUT_PULLUP);
    }

    // Configure encoder pins
    for (int i = 0; i < numEncoders; i++) {
        pinMode(encoders[i].pinA, INPUT_PULLUP);
        pinMode(encoders[i].pinB, INPUT_PULLUP);
        if (encoders[i].pinBtn >= 0) {
            pinMode(encoders[i].pinBtn, INPUT_PULLUP);
        }
        encoders[i].lastState = (digitalRead(encoders[i].pinA) << 1) | digitalRead(encoders[i].pinB);
        encoders[i].errorReported = false;
    }

    lastActivityTime = millis();
    return true;
}

/*
   Update Methods
*/
void SimRacingController::update() {
    if (!tryUpdate()) {
        waitForUpdate();
    }
}

bool SimRacingController::tryUpdate() {
    if (isUpdating) {
        return false;
    }
    
    isUpdating = true;
    
    // Check for power save mode
    if (!isPowerSaving && millis() - lastActivityTime > powerSaveTimeout) {
        sleep();
    }
    
    if (!isPowerSaving) {
        bool activityDetected = false;

        // Update MCP devices
        if (mcpInitialized) {
            for (uint8_t i = 0; i < numMcpDevices; i++) {
                updateMcp(i);
            }
        }

        // Update matrix
        for (int row = 0; row < numRows; row++) {
            digitalWrite(rowPins[row], LOW);
            delayMicroseconds(10);

            for (int col = 0; col < numCols; col++) {
                bool currentReading = (digitalRead(colPins[col]) == LOW);

                if (currentReading != lastMatrixStates[row][col]) {
                    lastMatrixDebounceTime[row][col] = millis();
                }

                if ((millis() - lastMatrixDebounceTime[row][col]) > matrixDebounceDelay) {
                    if (currentReading != matrixStates[row][col]) {
                        matrixStates[row][col] = currentReading;
                        processMatrixPress(row, col, currentReading);
                        activityDetected = true;
                    }
                }

                lastMatrixStates[row][col] = currentReading;
            }

            digitalWrite(rowPins[row], HIGH);
        }

        // Update GPIO
        for (int i = 0; i < numGpio; i++) {
            bool currentReading = (digitalRead(gpioPins[i]) == LOW);

            if (currentReading != lastGpioStates[i]) {
                gpioDebounceTime[i] = millis();
            }

            if ((millis() - gpioDebounceTime[i]) > matrixDebounceDelay) {
                if (currentReading != gpioStates[i]) {
                    gpioStates[i] = currentReading;
                    if (onGpioChange) {
                        onGpioChange(currentProfile, i, currentReading);
                    }
                    activityDetected = true;
                }
            }

            lastGpioStates[i] = currentReading;
        }

        // Update encoders
        for (int i = 0; i < numEncoders; i++) {
            updateEncoder(i);
        }

        if (activityDetected) {
            lastActivityTime = millis();
        }
    }
    
    isUpdating = false;
    return true;
}

void SimRacingController::waitForUpdate() {
    while (!tryUpdate()) {
        delay(1);
    }
}

/*
   MCP update
*/
void SimRacingController::updateMcp(uint8_t device) {
    if (device >= numMcpDevices) return;

    uint16_t currentReading;
    if (!readMcpPorts(device, currentReading)) {
        return;
    }

    for (int pin = 0; pin < 16; pin++) {
        bool pinState = !(currentReading & (1 << pin));
        bool lastState = ((lastMcpStates[device] >> pin) & 1);
        
        if (pinState != lastState) {
            mcpDebounceTime[device * 16 + pin] = millis();
            lastMcpStates[device] = (lastMcpStates[device] & ~(1 << pin)) | (pinState << pin);
        }

        if ((millis() - mcpDebounceTime[device * 16 + pin]) > matrixDebounceDelay) {
            if (pinState != ((mcpStates[device] >> pin) & 1)) {
                mcpStates[device] = (mcpStates[device] & ~(1 << pin)) | (pinState << pin);
                processMcpChange(device, pin, pinState);
            }
        }
    }
}

void SimRacingController::processMcpChange(uint8_t device, int pin, bool state) {
    if (onMcpChange) {
        onMcpChange(currentProfile, device, pin, state);
    }
}

/*
   Power Management
*/
bool SimRacingController::setPowerSaveTimeout(unsigned long timeoutMs) {
    if (timeoutMs < MIN_POWER_SAVE_MS || timeoutMs > MAX_POWER_SAVE_MS) {
        lastError = ControllerError(ControllerError::INVALID_CONFIG, "Invalid power save timeout");
        return false;
    }
    const_cast<unsigned long&>(powerSaveTimeout) = timeoutMs;
    return true;
}

void SimRacingController::sleep() {
    isPowerSaving = true;
    
    // Set all output pins to INPUT to save power
    for (int i = 0; i < numRows; i++) {
        pinMode(rowPins[i], INPUT);
    }
}

void SimRacingController::wake() {
    isPowerSaving = false;
    lastActivityTime = millis();
    
    // Restore pin modes
    for (int i = 0; i < numRows; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);
    }
}

void SimRacingController::clearError() {
    lastError = ControllerError();
    errorReported = false;
}

/*
   Encoder Methods
*/
void SimRacingController::updateEncoder(int index) {
    if (index < 0 || index >= numEncoders) return;

    EncoderConfig& enc = encoders[index];
    unsigned long currentTime = millis();

    // Handle encoder button if configured
    if (enc.pinBtn >= 0) {
        bool currentBtnState = (digitalRead(enc.pinBtn) == LOW);
        if (currentBtnState != enc.lastBtnState) {
            enc.lastBtnTime = currentTime;
        }

        if ((currentTime - enc.lastBtnTime) > matrixDebounceDelay) {
            if (currentBtnState != enc.btnState) {
                enc.btnState = currentBtnState;
                if (onEncoderButtonChange) {
                    onEncoderButtonChange(currentProfile, index, currentBtnState);
                }
            }
        }
        enc.lastBtnState = currentBtnState;
    }

    // Handle encoder rotation
    if (currentTime - enc.lastTime >= encoderDebounceTime) {
        uint8_t currentState = (digitalRead(enc.pinA) << 1) | digitalRead(enc.pinB);

        if (currentState != enc.lastState) {
            enc.lastTime = currentTime;

            // Calculate rotation speed
            if (enc.lastChangeTime > 0) {
                unsigned long timeDiff = currentTime - enc.lastChangeTime;
                if (timeDiff > 0) {
                    enc.speed = 1000 / timeDiff;
                }
            }
            enc.lastChangeTime = currentTime;

            // Determine rotation direction using state transition
            bool validTransition = true;
            if (enc.lastState == 0) {
                if (currentState == 1) enc.encDir = 1;
                else if (currentState == 2) enc.encDir = -1;
                else validTransition = false;
            }
            else if (enc.lastState == 1) {
                if (currentState == 3) enc.encDir = 1;
                else if (currentState == 0) enc.encDir = -1;
                else validTransition = false;
            }
            else if (enc.lastState == 2) {
                if (currentState == 0) enc.encDir = 1;
                else if (currentState == 3) enc.encDir = -1;
                else validTransition = false;
            }
            else if (enc.lastState == 3) {
                if (currentState == 2) enc.encDir = 1;
                else if (currentState == 1) enc.encDir = -1;
                else validTransition = false;
            }

            if (!validTransition) {
                enc.errorCount++;
            }

            // Process complete state transition
            if (enc.encDir != 0) {
                if ((enc.lastState == 3 && currentState == 2 && enc.encDir == 1) ||
                    (enc.lastState == 3 && currentState == 1 && enc.encDir == -1)) {
                    enc.position += ((enc.encDir * 4) / enc.divisor);
                    enc.lastDirection = enc.encDir;
                    enc.valid = (enc.errorCount < MAX_ERROR_COUNT);

                    if (onEncoderChange) {
                        onEncoderChange(currentProfile, index, enc.encDir);
                    }

                    enc.encDir = 0;
                }
            }

            enc.lastState = currentState;
        }

        // Reset speed if no changes
        if (currentTime - enc.lastChangeTime > 1000) {
            enc.speed = 0;
        }

        // Check for encoder malfunction
        if (enc.errorCount >= MAX_ERROR_COUNT && !enc.errorReported) {
            lastError = ControllerError(ControllerError::ENCODER_MALFUNCTION, 
                "Excessive encoder errors detected");
            if (errorCallback) {
                errorCallback(lastError);
                enc.errorReported = true;
            }
        }
    }
}

/*
   Callbacks
*/
void SimRacingController::processMatrixPress(int row, int col, bool state) {
    if (onMatrixChange) {
        onMatrixChange(currentProfile, row, col, state);
    }
}

void SimRacingController::setMatrixCallback(MatrixCallback callback) {
    onMatrixChange = callback;
}

void SimRacingController::setGpioCallback(GpioCallback callback) {
    onGpioChange = callback;
}

void SimRacingController::setEncoderCallback(EncoderCallback callback) {
    onEncoderChange = callback;
}

void SimRacingController::setEncoderButtonCallback(EncoderButtonCallback callback) {
    onEncoderButtonChange = callback;
}

void SimRacingController::setMcpCallback(McpCallback callback) {
    onMcpChange = callback;
}

void SimRacingController::setErrorCallback(bool (*callback)(const ControllerError&)) {
    errorCallback = callback;
}

/*
   Configuration Methods
*/
void SimRacingController::setProfiles(int numProfiles) {
    const_cast<int&>(this->numProfiles) = numProfiles;
}

void SimRacingController::setDebounceTime(unsigned long matrixDebounce,
    unsigned long encoderDebounce) {
    const_cast<unsigned long&>(matrixDebounceDelay) = matrixDebounce;
    const_cast<unsigned long&>(encoderDebounceTime) = encoderDebounce;
}

void SimRacingController::setProfile(int profile) {
    if (profile >= 0 && profile < numProfiles) {
        currentProfile = profile;
    }
}

/*
   State Getters
*/
bool SimRacingController::isInPowerSave() const {
    return isPowerSaving;
}

bool SimRacingController::isUpdateInProgress() const {
    return isUpdating;
}

int SimRacingController::getProfile() const {
    return currentProfile;
}

bool SimRacingController::getMatrixState(int row, int col) const {
    if (row >= 0 && row < numRows && col >= 0 && col < numCols) {
        return matrixStates[row][col];
    }
    return false;
}

bool SimRacingController::getGpioState(int gpio) const {
    if (gpio >= 0 && gpio < numGpio) {
        return gpioStates[gpio];
    }
    return false;
}

bool SimRacingController::getMcpState(uint8_t device, uint8_t pin) const {
    if (device >= numMcpDevices || pin >= 16) return false;
    return (mcpStates[device] & (1 << pin)) != 0;
}

int32_t SimRacingController::getEncoderPosition(int index) const {
    if (index >= 0 && index < numEncoders) {
        return encoders[index].position;
    }
    return 0;
}

int8_t SimRacingController::getEncoderDirection(int index) const {
    if (index >= 0 && index < numEncoders) {
        return encoders[index].lastDirection;
    }
    return 0;
}

uint16_t SimRacingController::getEncoderSpeed(int index) const {
    if (index >= 0 && index < numEncoders) {
        return encoders[index].speed;
    }
    return 0;
}

bool SimRacingController::isEncoderValid(int index) const {
    if (index >= 0 && index < numEncoders) {
        return encoders[index].valid;
    }
    return false;
}

bool SimRacingController::getEncoderButtonState(int index) const {
    if (index >= 0 && index < numEncoders && encoders[index].pinBtn >= 0) {
        return encoders[index].btnState;
    }
    return false;
}

ControllerError SimRacingController::getLastError() const {
    return lastError;
}

/*
   Encoder Configuration
*/
void SimRacingController::setEncoderDivisor(int encoderIndex, int32_t divisor) {
    if (encoderIndex >= 0 && encoderIndex < numEncoders && divisor > 0 && divisor <= 4) {
        encoders[encoderIndex].divisor = divisor;
    }
}

void SimRacingController::setEncoderPosition(int encoderIndex, int32_t position) {
    if (encoderIndex >= 0 && encoderIndex < numEncoders) {
        encoders[encoderIndex].position = position;
    }
}
