/**************************
   SimRacingController.cpp
   v 2.0.0
   by roncoa@gmail.com
   27/01/2025
 **************************/

#include "SimRacingController.h"

/*
   Constructor - Initializes all variables to safe default values
*/
SimRacingController::SimRacingController() :
  numRows(0), numCols(0), rowPins(nullptr), colPins(nullptr),
  matrixDebounceDelay(50),
  gpioPins(nullptr), lastGpioStates(nullptr), gpioStates(nullptr),
  gpioDebounceTime(nullptr), numGpio(0),
  numEncoders(0), encoderDebounceTime(5),
  currentProfile(0), numProfiles(1),
  onMatrixChange(nullptr), onGpioChange(nullptr),
  onEncoderChange(nullptr), onEncoderButtonChange(nullptr),
  lastMatrixStates(nullptr), matrixStates(nullptr), lastMatrixDebounceTime(nullptr),
  encoders(nullptr) {}

/*
   Destructor - Ensures proper cleanup of allocated memory
*/
SimRacingController::~SimRacingController() {
  cleanupArrays();
  delete[] encoders;
  delete[] lastGpioStates;
  delete[] gpioStates;
  delete[] gpioDebounceTime;
}

/*
   Configures the button matrix
   @param rowPins Array of pin numbers for rows
   @param numRows Number of rows in the matrix
   @param colPins Array of pin numbers for columns
   @param numCols Number of columns in the matrix
*/
void SimRacingController::setMatrix(const int* rowPins, int numRows,
                                    const int* colPins, int numCols) {
  MatrixConfig config(rowPins, numRows, colPins, numCols);
  configureMatrix(config);
}

/*
   Configures direct GPIO buttons
   @param pins Array of GPIO pin numbers
   @param numPins Number of GPIO pins to configure
*/
void SimRacingController::setGpio(const int* pins, int numPins) {
  // Cleanup existing GPIO arrays
  delete[] lastGpioStates;
  delete[] gpioStates;
  delete[] gpioDebounceTime;

  // Update member variables
  const_cast<int*&>(gpioPins) = const_cast<int*>(pins);
  const_cast<int&>(numGpio) = numPins;

  // Allocate new memory
  lastGpioStates = new bool[numPins]();
  gpioStates = new bool[numPins]();
  gpioDebounceTime = new unsigned long[numPins]();
}

/*
   Configures encoders without buttons
   @param encoderPinsA Array of first pins for each encoder
   @param encoderPinsB Array of second pins for each encoder
   @param numEncoders Number of encoders to configure
*/
void SimRacingController::setEncoders(const int* encoderPinsA, const int* encoderPinsB,
                                      int numEncoders) {
  setEncoders(encoderPinsA, encoderPinsB, nullptr, numEncoders);
}

/*
   Configures encoders with optional buttons
   @param encoderPinsA Array of first pins for each encoder
   @param encoderPinsB Array of second pins for each encoder
   @param encoderBtnPins Array of button pins (can be nullptr)
   @param numEncoders Number of encoders to configure
*/
void SimRacingController::setEncoders(const int* encoderPinsA, const int* encoderPinsB,
                                      const int* encoderBtnPins, int numEncoders) {
  EncoderInitConfig config(encoderPinsA, encoderPinsB, encoderBtnPins, numEncoders);
  configureEncoders(config);
}

/*
   Internal method to configure the button matrix
*/
void SimRacingController::configureMatrix(const MatrixConfig& config) {
  // Clean up existing arrays if any
  cleanupArrays();

  // Update member variables
  const_cast<int&>(numRows) = config.numRows;
  const_cast<int&>(numCols) = config.numCols;
  const_cast<int*&>(rowPins) = const_cast<int*>(config.rowPins);
  const_cast<int*&>(colPins) = const_cast<int*>(config.colPins);

  // Initialize arrays
  initializeArrays();
}

/*
   Internal method to configure encoders
*/
void SimRacingController::configureEncoders(const EncoderInitConfig& config) {
  // Clean up existing encoders if any
  delete[] encoders;

  // Update member variables
  const_cast<int&>(numEncoders) = config.count;

  // Initialize encoders
  encoders = new EncoderConfig[config.count];
  for (int i = 0; i < config.count; i++) {
    encoders[i].pinA = config.pinsA[i];
    encoders[i].pinB = config.pinsB[i];
    encoders[i].pinBtn = config.btnPins ? config.btnPins[i] : -1;
    encoders[i].divisor = 4;
    encoders[i].valid = true;
  }
}

/*
   Sets the number of available profiles
   @param numProfiles Number of profiles to configure
*/
void SimRacingController::setProfiles(int numProfiles) {
  const_cast<int&>(this->numProfiles) = numProfiles;
}

/*
   Sets debounce times for matrix/GPIO buttons and encoders
   @param matrixDebounce Debounce time in ms for matrix and GPIO buttons
   @param encoderDebounce Debounce time in ms for encoders
*/
void SimRacingController::setDebounceTime(unsigned long matrixDebounce,
    unsigned long encoderDebounce) {
  const_cast<unsigned long&>(matrixDebounceDelay) = matrixDebounce;
  const_cast<unsigned long&>(encoderDebounceTime) = encoderDebounce;
}

/*
   Initializes matrix state arrays
*/
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

/*
   Cleans up matrix state arrays
*/
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
   Initializes all configured components
   Must be called after all set* methods and before update()
*/
void SimRacingController::begin() {
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
  }
}

/*
   Updates the state of all inputs
   Must be called in the main loop
*/
void SimRacingController::update() {
  // Update matrix buttons
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
        }
      }

      lastMatrixStates[row][col] = currentReading;
    }

    digitalWrite(rowPins[row], HIGH);
  }

  // Update GPIO buttons
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
      }
    }

    lastGpioStates[i] = currentReading;
  }

  // Update encoders
  for (int i = 0; i < numEncoders; i++) {
    updateEncoder(i);
  }
}

/*
   Updates the state of a single encoder
   Handles both rotation and button (if configured)
*/
void SimRacingController::updateEncoder(int index) {
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
      if (enc.lastState == 0) {
        if (currentState == 1) enc.encDir = 1;
        else if (currentState == 2) enc.encDir = -1;
        else enc.errorCount++;
      }
      else if (enc.lastState == 1) {
        if (currentState == 3) enc.encDir = 1;
        else if (currentState == 0) enc.encDir = -1;
        else enc.errorCount++;
      }
      else if (enc.lastState == 2) {
        if (currentState == 0) enc.encDir = 1;
        else if (currentState == 3) enc.encDir = -1;
        else enc.errorCount++;
      }
      else if (enc.lastState == 3) {
        if (currentState == 2) enc.encDir = 1;
        else if (currentState == 1) enc.encDir = -1;
        else enc.errorCount++;
      }

      // Process complete state transition
      if (enc.encDir != 0) {
        if ((enc.lastState == 3 && currentState == 2 && enc.encDir == 1) ||
            (enc.lastState == 3 && currentState == 1 && enc.encDir == -1)) {
          enc.position += ((enc.encDir * 4) / enc.divisor);
          enc.lastDirection = enc.encDir;
          enc.valid = (enc.errorCount == 0);

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
  }
}

/*
   Processes matrix button state changes
*/
void SimRacingController::processMatrixPress(int row, int col, bool state) {
  if (onMatrixChange) {
    onMatrixChange(currentProfile, row, col, state);
  }
}

/*
   Sets encoder resolution divisor
   @param encoderIndex Index of the encoder to configure
   @param divisor Divisor value (1-4)
*/
void SimRacingController::setEncoderDivisor(int encoderIndex, int32_t divisor) {
  if (encoderIndex >= 0 && encoderIndex < numEncoders && divisor > 0 && divisor <= 4) {
    encoders[encoderIndex].divisor = divisor;
  }
}

/*
   Sets encoder absolute position
   @param encoderIndex Index of the encoder
   @param position New absolute position
*/
void SimRacingController::setEncoderPosition(int encoderIndex, int32_t position) {
  if (encoderIndex >= 0 && encoderIndex < numEncoders) {
    encoders[encoderIndex].position = position;
  }
}

/*
   Sets current active profile
   @param profile Profile number (0 to numProfiles-1)
*/
void SimRacingController::setProfile(int profile) {
  if (profile >= 0 && profile < numProfiles) {
    currentProfile = profile;
  }
}

/*
   Gets current active profile
   @return Current profile number
*/
int SimRacingController::getProfile() const {
  return currentProfile;
}

/*
   Gets encoder's current absolute position
   @param index Index of the encoder
   @return Current position value
*/
int32_t SimRacingController::getEncoderPosition(int index) const {
  if (index >= 0 && index < numEncoders) {
    return encoders[index].position;
  }
  return 0;
}

/*
   Gets encoder's last rotation direction
   @param index Index of the encoder
   @return Last direction (-1: CCW, 0: no movement, 1: CW)
*/
int8_t SimRacingController::getEncoderDirection(int index) const {
  if (index >= 0 && index < numEncoders) {
    return encoders[index].lastDirection;
  }
  return 0;
}

/*
   Gets encoder's current rotation speed
   @param index Index of the encoder
   @return Speed in steps per second
*/
uint16_t SimRacingController::getEncoderSpeed(int index) const {
  if (index >= 0 && index < numEncoders) {
    return encoders[index].speed;
  }
  return 0;
}

/*
   Gets matrix button state
   @param row Row number
   @param col Column number
   @return true if button is pressed
*/
bool SimRacingController::getMatrixState(int row, int col) const {
  if (row >= 0 && row < numRows && col >= 0 && col < numCols) {
    return matrixStates[row][col];
  }
  return false;
}

/*
   Gets GPIO button state
   @param gpio GPIO index
   @return true if button is pressed
*/
bool SimRacingController::getGpioState(int gpio) const {
  if (gpio >= 0 && gpio < numGpio) {
    return gpioStates[gpio];
  }
  return false;
}

/*
   Checks if encoder is working correctly
   @param index Index of the encoder
   @return true if encoder has no errors
*/
bool SimRacingController::isEncoderValid(int index) const {
  if (index >= 0 && index < numEncoders) {
    return encoders[index].valid;
  }
  return false;
}

/*
   Gets encoder button state if configured
   @param index Index of the encoder
   @return true if button is pressed, false if not pressed or not configured
*/
bool SimRacingController::getEncoderButtonState(int index) const {
  if (index >= 0 && index < numEncoders && encoders[index].pinBtn >= 0) {
    return encoders[index].btnState;
  }
  return false;
}

/*
   Sets callback for matrix button events
   @param callback Function to call on state change
*/
void SimRacingController::setMatrixCallback(MatrixCallback callback) {
  onMatrixChange = callback;
}

/*
   Sets callback for GPIO button events
   @param callback Function to call on state change
*/
void SimRacingController::setGpioCallback(GpioCallback callback) {
  onGpioChange = callback;
}

/*
   Sets callback for encoder rotation events
   @param callback Function to call on rotation
*/
void SimRacingController::setEncoderCallback(EncoderCallback callback) {
  onEncoderChange = callback;
}

/*
   Sets callback for encoder button events
   @param callback Function to call on button state change
*/
void SimRacingController::setEncoderButtonCallback(EncoderButtonCallback callback) {
  onEncoderButtonChange = callback;
}
