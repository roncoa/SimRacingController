/**************************
   SimRacingController.h
   v 2.1.0
   by roncoa@gmail.com
   27/01/2025
 **************************/

#ifndef SIMRACING_CONTROLLER_H
#define SIMRACING_CONTROLLER_H

#include <Arduino.h>

/*
   Matrix configuration structure
   Contains all necessary parameters for button matrix initialization
*/
struct MatrixConfig {
  const int* rowPins;      // Array of row pin numbers
  const int* colPins;      // Array of column pin numbers
  int numRows;            // Number of rows in the matrix
  int numCols;            // Number of columns in the matrix

  MatrixConfig(const int* _rowPins, int _numRows, const int* _colPins, int _numCols) :
    rowPins(_rowPins), colPins(_colPins), numRows(_numRows), numCols(_numCols) {}
};

class SimRacingController {
  private:
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

    /*
       Encoder Configuration Structure
       Manages state and settings for each rotary encoder
    */
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

    /*
       Encoder Initialization Configuration
       Used during setup to configure multiple encoders
    */
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

  public:
    /*
       Constructor and Destructor
    */
    SimRacingController();
    ~SimRacingController();

    /*
       Configuration Methods
       Must be called before begin()
    */
    void setMatrix(const int* rowPins, int numRows, const int* colPins, int numCols);
    void setGpio(const int* pins, int numPins);
    void setEncoders(const int* encoderPinsA, const int* encoderPinsB, int numEncoders);
    void setEncoders(const int* encoderPinsA, const int* encoderPinsB,
                     const int* encoderBtnPins, int numEncoders);
    void setProfiles(int numProfiles);
    void setDebounceTime(unsigned long matrixDebounce, unsigned long encoderDebounce);

    /*
       Core Methods
    */
    void begin();    // Initialize all configured components
    void update();   // Must be called in loop to process all inputs

    /*
       Encoder Configuration Methods
    */
    void setEncoderDivisor(int encoderIndex, int32_t divisor);
    void setEncoderPosition(int encoderIndex, int32_t position);

    /*
       Profile Management
    */
    void setProfile(int profile);
    int getProfile() const;

    /*
       State Getters
    */
    int32_t getEncoderPosition(int index) const;
    int8_t getEncoderDirection(int index) const;
    uint16_t getEncoderSpeed(int index) const;
    bool getMatrixState(int row, int col) const;
    bool getGpioState(int gpio) const;
    bool isEncoderValid(int index) const;
    bool getEncoderButtonState(int index) const;

    /*
       Callback Types
    */
    typedef void (*MatrixCallback)(int profile, int row, int col, bool state);
    typedef void (*GpioCallback)(int profile, int gpio, bool state);
    typedef void (*EncoderCallback)(int profile, int encoder, int direction);
    typedef void (*EncoderButtonCallback)(int profile, int encoder, bool pressed);

    /*
       Callback Setters
    */
    void setMatrixCallback(MatrixCallback callback);
    void setGpioCallback(GpioCallback callback);
    void setEncoderCallback(EncoderCallback callback);
    void setEncoderButtonCallback(EncoderButtonCallback callback);

  private:
    MatrixCallback onMatrixChange;
    GpioCallback onGpioChange;
    EncoderCallback onEncoderChange;
    EncoderButtonCallback onEncoderButtonChange;
};

#endif
