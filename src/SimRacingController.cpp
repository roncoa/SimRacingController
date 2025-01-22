/**************************
 * SimRacingController.cpp
 * v 1.0.0
 * by roncoa@gmail.com
 * 22/01/2025
 **************************/

#include "SimRacingController.h"

// Costruttore con pulsanti encoder
SimRacingController::SimRacingController(
    const int* rowPins, int numRows,
    const int* colPins, int numCols,
    const int* encoderPinsA, const int* encoderPinsB, 
    const int* encoderBtnPins,
    int numEncoders,
    int numProfiles,
    unsigned long buttonDebounce,
    unsigned long encoderDebounce
) : 
    numRows(numRows),
    numCols(numCols),
    rowPins(rowPins),
    colPins(colPins),
    buttonDebounceDelay(buttonDebounce),
    numEncoders(numEncoders),
    encoderDebounceTime(encoderDebounce),
    currentProfile(0),
    numProfiles(numProfiles),
    onButtonChange(nullptr),
    onEncoderChange(nullptr),
    onEncoderButtonChange(nullptr)
{
    initializeArrays();
    
    // Inizializza encoder
    encoders = new EncoderConfig[numEncoders];
    for(int i = 0; i < numEncoders; i++) {
        encoders[i].pinA = encoderPinsA[i];
        encoders[i].pinB = encoderPinsB[i];
        encoders[i].pinBtn = encoderBtnPins ? encoderBtnPins[i] : -1;
        encoders[i].divisor = 4;
        encoders[i].valid = true;
    }
}

// Costruttore senza pulsanti encoder
SimRacingController::SimRacingController(
    const int* rowPins, int numRows,
    const int* colPins, int numCols,
    const int* encoderPinsA, const int* encoderPinsB,
    int numEncoders,
    int numProfiles,
    unsigned long buttonDebounce,
    unsigned long encoderDebounce
) : SimRacingController(rowPins, numRows, colPins, numCols, encoderPinsA, encoderPinsB, 
                       nullptr, numEncoders, numProfiles, buttonDebounce, encoderDebounce) {}

SimRacingController::~SimRacingController() {
    cleanupArrays();
    delete[] encoders;
}

void SimRacingController::initializeArrays() {
    lastButtonStates = new bool*[numRows];
    buttonStates = new bool*[numRows];
    lastDebounceTime = new unsigned long*[numRows];
    
    for(int i = 0; i < numRows; i++) {
        lastButtonStates[i] = new bool[numCols]();
        buttonStates[i] = new bool[numCols]();
        lastDebounceTime[i] = new unsigned long[numCols]();
    }
}

void SimRacingController::cleanupArrays() {
    for(int i = 0; i < numRows; i++) {
        delete[] lastButtonStates[i];
        delete[] buttonStates[i];
        delete[] lastDebounceTime[i];
    }
    delete[] lastButtonStates;
    delete[] buttonStates;
    delete[] lastDebounceTime;
}

void SimRacingController::begin() {
    // Configura pin matrice
    for(int i = 0; i < numRows; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);
    }
    for(int i = 0; i < numCols; i++) {
        pinMode(colPins[i], INPUT_PULLUP);
    }
    
    // Configura pin encoder
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
    // Aggiorna matrice pulsanti
    for(int row = 0; row < numRows; row++) {
        digitalWrite(rowPins[row], LOW);
        delayMicroseconds(10);
        
        for(int col = 0; col < numCols; col++) {
            bool currentReading = (digitalRead(colPins[col]) == LOW);
            
            if(currentReading != lastButtonStates[row][col]) {
                lastDebounceTime[row][col] = millis();
            }
            
            if((millis() - lastDebounceTime[row][col]) > buttonDebounceDelay) {
                if(currentReading != buttonStates[row][col]) {
                    buttonStates[row][col] = currentReading;
                    processButtonPress(row, col, currentReading);
                }
            }
            
            lastButtonStates[row][col] = currentReading;
        }
        
        digitalWrite(rowPins[row], HIGH);
    }
    
    // Aggiorna encoder
    for(int i = 0; i < numEncoders; i++) {
        updateEncoder(i);
    }
}

void SimRacingController::updateEncoder(int index) {
    EncoderConfig& enc = encoders[index];
    unsigned long currentTime = millis();
    
    // Gestione pulsante encoder
    if(enc.pinBtn >= 0) {
        bool currentBtnState = (digitalRead(enc.pinBtn) == LOW);
        if(currentBtnState != enc.lastBtnState) {
            enc.lastBtnTime = currentTime;
        }
        
        if((currentTime - enc.lastBtnTime) > buttonDebounceDelay) {
            if(currentBtnState != enc.btnState) {
                enc.btnState = currentBtnState;
                if(onEncoderButtonChange) {
                    onEncoderButtonChange(currentProfile, index, currentBtnState);
                }
            }
        }
        enc.lastBtnState = currentBtnState;
    }
    
    // Gestione rotazione encoder
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
        
        // Reset velocità se non ci sono cambiamenti
        if(currentTime - enc.lastChangeTime > 1000) {
            enc.speed = 0;
        }
    }
}

void SimRacingController::processButtonPress(int row, int col, bool state) {
    if(onButtonChange) {
        onButtonChange(currentProfile, row, col, state);
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

bool SimRacingController::getButtonState(int row, int col) const {
    if(row >= 0 && row < numRows && col >= 0 && col < numCols) {
        return buttonStates[row][col];
    }
    return false;
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

void SimRacingController::setButtonCallback(ButtonCallback callback) {
    onButtonChange = callback;
}

void SimRacingController::setEncoderCallback(EncoderCallback callback) {
    onEncoderChange = callback;
}

void SimRacingController::setEncoderButtonCallback(EncoderButtonCallback callback) {
    onEncoderButtonChange = callback;
}
