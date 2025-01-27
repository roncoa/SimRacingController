/**************************
 * SimRacingController.cpp
 * v 2.1.0
 * by roncoa@gmail.com
 * 27/01/2025
 **************************/

#include "SimRacingController.h"

SimRacingController::SimRacingController() : 
    numRows(0), numCols(0), rowPins(nullptr), colPins(nullptr),
    matrixDebounceDelay(50),
    gpioPins(nullptr), lastGpioStates(nullptr), gpioStates(nullptr),
    gpioDebounceTime(nullptr), numGpio(0),
    numExpanders(0), expanderType(EXPANDER_NONE),
    expanderAddresses(nullptr), expanderStates(nullptr),
    lastExpanderStates(nullptr), expanderDebounceTime(nullptr),
    wireInitialized(false),
    #ifdef SUPPORT_CUSTOM_I2C_PINS
    sdaPin(-1), sclPin(-1),
    #endif
    numEncoders(0), encoderDebounceTime(5),
    currentProfile(0), numProfiles(1),
    onMatrixChange(nullptr), onGpioChange(nullptr),
    onExpanderChange(nullptr), onEncoderChange(nullptr), 
    onEncoderButtonChange(nullptr),
    lastMatrixStates(nullptr), matrixStates(nullptr), 
    lastMatrixDebounceTime(nullptr),
    encoders(nullptr) {}

SimRacingController::~SimRacingController() {
    cleanupArrays();
    delete[] encoders;
    delete[] lastGpioStates;
    delete[] gpioStates;
    delete[] gpioDebounceTime;
    delete[] expanderAddresses;
    delete[] expanderStates;
    delete[] lastExpanderStates;
    delete[] expanderDebounceTime;
}

void SimRacingController::setMatrix(const int* rowPins, int numRows, 
                                  const int* colPins, int numCols) {
    MatrixConfig config(rowPins, numRows, colPins, numCols);
    configureMatrix(config);
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

void SimRacingController::setExpander(int sda, int scl, ExpanderType type, int num) {
    #ifdef SUPPORT_CUSTOM_I2C_PINS
    sdaPin = sda;
    sclPin = scl;
    #endif
    
    expanderType = type;
    const_cast<int&>(numExpanders) = num;

    delete[] expanderAddresses;
    delete[] expanderStates;
    delete[] lastExpanderStates;
    delete[] expanderDebounceTime;

    expanderAddresses = new uint8_t[num];
    expanderStates = new uint8_t[num];
    lastExpanderStates = new uint8_t[num];
    expanderDebounceTime = new unsigned long[num];

    // Initialize base addresses for each expander
    for(int i = 0; i < num; i++) {
        if(type == EXPANDER_PCF8574) {
            expanderAddresses[i] = 0x20 + i;  // PCF8574 base address
        } else if(type == EXPANDER_PCF8574A) {
            expanderAddresses[i] = 0x38 + i;  // PCF8574A base address
        } else if(type == EXPANDER_MCP23017) {
            expanderAddresses[i] = 0x20 + i;  // MCP23017 base address
        }
        expanderStates[i] = 0xFF;
        lastExpanderStates[i] = 0xFF;
        expanderDebounceTime[i] = 0;
    }
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

void SimRacingController::configureMatrix(const MatrixConfig& config) {
    cleanupArrays();
    
    const_cast<int&>(numRows) = config.numRows;
    const_cast<int&>(numCols) = config.numCols;
    const_cast<int*&>(rowPins) = const_cast<int*>(config.rowPins);
    const_cast<int*&>(colPins) = const_cast<int*>(config.colPins);
    
    initializeArrays();
}

void SimRacingController::configureEncoders(const EncoderInitConfig& config) {
    delete[] encoders;
    
    const_cast<int&>(numEncoders) = config.count;
    
    encoders = new EncoderConfig[config.count];
    for(int i = 0; i < config.count; i++) {
        encoders[i].pinA = config.pinsA[i];
        encoders[i].pinB = config.pinsB[i];
        encoders[i].pinBtn = config.btnPins ? config.btnPins[i] : -1;
        encoders[i].divisor = 4;
        encoders[i].valid = true;
    }
}

void SimRacingController::initializeExpander(uint8_t address) {
    if(expanderType == EXPANDER_MCP23017) {
        Wire.beginTransmission(address);
        Wire.write(0x00); // IODIRA register
        Wire.write(0xFF); // Set all pins as inputs
        Wire.write(0xFF); // Set all pins as inputs
        Wire.endTransmission();
        
        Wire.beginTransmission(address);
        Wire.write(0x0C); // GPPUA register
        Wire.write(0xFF); // Enable pull-up on all pins
        Wire.write(0xFF); // Enable pull-up on all pins
        Wire.endTransmission();
    }
    else if(expanderType == EXPANDER_PCF8574 || expanderType == EXPANDER_PCF8574A) {
        Wire.beginTransmission(address);
        Wire.write(0xFF);  // Set all pins as inputs with pull-up
        Wire.endTransmission();
    }
}

uint8_t SimRacingController::readExpander(uint8_t address) {
    uint8_t value = 0xFF;
    
    if(expanderType == EXPANDER_MCP23017) {
        Wire.beginTransmission(address);
        Wire.write(0x12);  // GPIOA register
        Wire.endTransmission();
        
        Wire.requestFrom(address, (uint8_t)2);
        if(Wire.available()) {
            value = Wire.read();  // Read GPIOA
            Wire.read();         // Read GPIOB (not used)
        }
    }
    else if(expanderType == EXPANDER_PCF8574 || expanderType == EXPANDER_PCF8574A) {
        Wire.requestFrom(address, (uint8_t)1);
        if(Wire.available()) {
            value = Wire.read();
        }
    }
    
    return value;
}

void SimRacingController::initializeArrays() {
    lastMatrixStates = new bool*[numRows];
    matrixStates = new bool*[numRows];
    lastMatrixDebounceTime = new unsigned long*[numRows];
    
    for(int i = 0; i < numRows; i++) {
        lastMatrixStates[i] = new bool[numCols]();
        matrixStates[i] = new bool[numCols]();
        lastMatrixDebounceTime[i] = new unsigned long[numCols]();
    }
}

void SimRacingController::cleanupArrays() {
    if (lastMatrixStates) {
        for(int i = 0; i < numRows; i++) {
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

void SimRacingController::setProfiles(int numProfiles) {
    const_cast<int&>(this->numProfiles) = numProfiles;
}

void SimRacingController::setDebounceTime(unsigned long matrixDebounce, 
                                        unsigned long encoderDebounce) {
    const_cast<unsigned long&>(matrixDebounceDelay) = matrixDebounce;
    const_cast<unsigned long&>(encoderDebounceTime) = encoderDebounce;
}

void SimRacingController::begin() {
    // Initialize matrix pins
    for(int i = 0; i < numRows; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);
    }
    for(int i = 0; i < numCols; i++) {
        pinMode(colPins[i], INPUT_PULLUP);
    }
    
    // Initialize GPIO pins
    for(int i = 0; i < numGpio; i++) {
        pinMode(gpioPins[i], INPUT_PULLUP);
    }
    
    // Initialize I2C if expanders are configured
    if(expanderType != EXPANDER_NONE) {
        #ifdef SUPPORT_CUSTOM_I2C_PINS
            if(sdaPin >= 0 && sclPin >= 0) {
                Wire.begin(sdaPin, sclPin);
            } else {
                Wire.begin();
            }
        #else
            Wire.begin();
        #endif
        wireInitialized = true;
        
        // Initialize all expanders
        for(int i = 0; i < numExpanders; i++) {
            initializeExpander(expanderAddresses[i]);
        }
    }
    
    // Initialize encoder pins
    for(int i = 0; i < numEncoders; i++) {
        pinMode(encoders[i].pinA, INPUT_PULLUP);
        pinMode(encoders[i].pinB, INPUT_PULLUP);
        if(encoders[i].pinBtn >= 0) {
            pinMode(encoders[i].pinBtn, INPUT_PULLUP);
        }
        encoders[i].lastState = (digitalRead(encoders[i].pinA) << 1) | digitalRead(encoders[i].pinB);
    }
}

void SimRacingController::update() {
    // Update matrix buttons
    for(int row = 0; row < numRows; row++) {
        digitalWrite(rowPins[row], LOW);
        delayMicroseconds(10);
        
        for(int col = 0; col < numCols; col++) {
            bool currentReading = (digitalRead(colPins[col]) == LOW);
            
            if(currentReading != lastMatrixStates[row][col]) {
                lastMatrixDebounceTime[row][col] = millis();
            }
            
            if((millis() - lastMatrixDebounceTime[row][col]) > matrixDebounceDelay) {
                if(currentReading != matrixStates[row][col]) {
                    matrixStates[row][col] = currentReading;
                    processMatrixPress(row, col, currentReading);
                }
            }
            
            lastMatrixStates[row][col] = currentReading;
        }
        
        digitalWrite(rowPins[row], HIGH);
    }
    
    // Update GPIO buttons
    for(int i = 0; i < numGpio; i++) {
        bool currentReading = (digitalRead(gpioPins[i]) == LOW);
        
        if(currentReading != lastGpioStates[i]) {
            gpioDebounceTime[i] = millis();
        }
        
        if((millis() - gpioDebounceTime[i]) > matrixDebounceDelay) {
            if(currentReading != gpioStates[i]) {
                gpioStates[i] = currentReading;
                if(onGpioChange) {
                    onGpioChange(currentProfile, i, currentReading);
                }
            }
        }
        
        lastGpioStates[i] = currentReading;
    }

    // Update I2C expanders
    if(wireInitialized && expanderType != EXPANDER_NONE) {
        for(int i = 0; i < numExpanders; i++) {
            uint8_t currentState = readExpander(expanderAddresses[i]);
            
            if(currentState != lastExpanderStates[i]) {
                expanderDebounceTime[i] = millis();
            }
            
            if((millis() - expanderDebounceTime[i]) > matrixDebounceDelay) {
                if(currentState != expanderStates[i]) {
                    expanderStates[i] = currentState;
                    if(onExpanderChange) {
                        onExpanderChange(currentProfile, i, currentState);
                    }
                }
            }
            
            lastExpanderStates[i] = currentState;
        }
    }
    
    // Update encoders
    for(int i = 0; i < numEncoders; i++) {
        updateEncoder(i);
    }
}

void SimRacingController::updateEncoder(int index) {
    EncoderConfig& enc = encoders[index];
    unsigned long currentTime = millis();
    
    // Handle encoder button if configured
    if(enc.pinBtn >= 0) {
        bool currentBtnState = (digitalRead(enc.pinBtn) == LOW);
        if(currentBtnState != enc.lastBtnState) {
            enc.lastBtnTime = currentTime;
        }
        
        if((currentTime - enc.lastBtnTime) > matrixDebounceDelay) {
            if(currentBtnState != enc.btnState) {
                enc.btnState = currentBtnState;
                if(onEncoderButtonChange) {
                    onEncoderButtonChange(currentProfile, index, currentBtnState);
                }
            }
        }
        enc.lastBtnState = currentBtnState;
    }
    
    // Handle encoder rotation
    if(currentTime - enc.lastTime >= encoderDebounceTime) {
        uint8_t currentState = (digitalRead(enc.pinA) << 1) | digitalRead(enc.pinB);
        
        if(currentState != enc.lastState) {
            enc.lastTime = currentTime;
            
            if(enc.lastChangeTime > 0) {
                unsigned long timeDiff = currentTime - enc.lastChangeTime;
                if(timeDiff > 0) {
                    enc.speed = 1000 / timeDiff;
                }
            }
            enc.lastChangeTime = currentTime;
            
            if(enc.lastState == 0) {
                if(currentState == 1) enc.encDir = 1;
                else if(currentState == 2) enc.encDir = -1;
                else enc.errorCount++;
            }
            else if(enc.lastState == 1) {
                if(currentState == 3) enc.encDir = 1;
                else if(currentState == 0) enc.encDir = -1;
                else enc.errorCount++;
            }
            else if(enc.lastState == 2) {
                if(currentState == 0) enc.encDir = 1;
                else if(currentState == 3) enc.encDir = -1;
                else enc.errorCount++;
            }
            else if(enc.lastState == 3) {
                if(currentState == 2) enc.encDir = 1;
                else if(currentState == 1) enc.encDir = -1;
                else enc.errorCount++;
            }
            
            if(enc.encDir != 0) {
                if((enc.lastState == 3 && currentState == 2 && enc.encDir == 1) ||
                   (enc.lastState == 3 && currentState == 1 && enc.encDir == -1)) {
                    enc.position += ((enc.encDir * 4) / enc.divisor);
                    enc.lastDirection = enc.encDir;
                    enc.valid = (enc.errorCount == 0);
                    
                    if(onEncoderChange) {
                        onEncoderChange(currentProfile, index, enc.encDir);
                    }
                    
                    enc.encDir = 0;
                }
            }
            
            enc.lastState = currentState;
        }
        
        // Reset speed if no changes
        if(currentTime - enc.lastChangeTime > 1000) {
            enc.speed = 0;
        }
    }
}

void SimRacingController::processMatrixPress(int row, int col, bool state) {
    if(onMatrixChange) {
        onMatrixChange(currentProfile, row, col, state);
    }
}

void SimRacingController::setEncoderDivisor(int encoderIndex, int32_t divisor) {
    if(encoderIndex >= 0 && encoderIndex < numEncoders && divisor > 0 && divisor <= 4) {
        encoders[encoderIndex].divisor = divisor;
    }
}

void SimRacingController::setEncoderPosition(int encoderIndex, int32_t position) {
    if(encoderIndex >= 0 && encoderIndex < numEncoders) {
        encoders[encoderIndex].position = position;
    }
}

void SimRacingController::setProfile(int profile) {
    if(profile >= 0 && profile < numProfiles) {
        currentProfile = profile;
    }
}

int SimRacingController::getProfile() const {
    return currentProfile;
}

bool SimRacingController::getMatrixState(int row, int col) const {
    if(row >= 0 && row < numRows && col >= 0 && col < numCols) {
        return matrixStates[row][col];
    }
    return false;
}

bool SimRacingController::getGpioState(int gpio) const {
    if(gpio >= 0 && gpio < numGpio) {
        return gpioStates[gpio];
    }
    return false;
}

uint8_t SimRacingController::getExpanderState(int expander) const {
    if(expander >= 0 && expander < numExpanders) {
        return expanderStates[expander];
    }
    return 0xFF;
}

int32_t SimRacingController::getEncoderPosition(int index) const {
    if(index >= 0 && index < numEncoders) {
        return encoders[index].position;
    }
    return 0;
}

int8_t SimRacingController::getEncoderDirection(int index) const {
    if(index >= 0 && index < numEncoders) {
        return encoders[index].lastDirection;
    }
    return 0;
}

uint16_t SimRacingController::getEncoderSpeed(int index) const {
    if(index >= 0 && index < numEncoders) {
        return encoders[index].speed;
    }
    return 0;
}

bool SimRacingController::isEncoderValid(int index) const {
    if(index >= 0 && index < numEncoders) {
        return encoders[index].valid;
    }
    return false;
}

bool SimRacingController::getEncoderButtonState(int index) const {
    if(index >= 0 && index < numEncoders && encoders[index].pinBtn >= 0) {
        return encoders[index].btnState;
    }
    return false;
}

void SimRacingController::setMatrixCallback(MatrixCallback callback) {
    onMatrixChange = callback;
}

void SimRacingController::setGpioCallback(GpioCallback callback) {
    onGpioChange = callback;
}

void SimRacingController::setExpanderCallback(ExpanderCallback callback) {
    onExpanderChange = callback;
}

void SimRacingController::setEncoderCallback(EncoderCallback callback) {
    onEncoderChange = callback;
}

void SimRacingController::setEncoderButtonCallback(EncoderButtonCallback callback) {
    onEncoderButtonChange = callback;
}