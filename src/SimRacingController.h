/**************************
 * SimRacingController.h
 * v 1.0.0
 * by roncoa@gmail.com
 * 22/01/2025
 **************************/

#ifndef SIMRACING_CONTROLLER_H
#define SIMRACING_CONTROLLER_H

#include <Arduino.h>

class SimRacingController {
private:
    // Matrice pulsanti
    const int numRows;
    const int numCols;
    const int* rowPins;
    const int* colPins;
    bool** lastButtonStates;
    bool** buttonStates;
    unsigned long** lastDebounceTime;
    const unsigned long buttonDebounceDelay;

    // Encoder
    struct EncoderConfig {
        int pinA;
        int pinB;
        int pinBtn;               // Pin pulsante encoder
        uint8_t lastState;
        int8_t encDir;
        int32_t position;
        unsigned long lastTime;
        unsigned long lastBtnTime;  // Debounce pulsante
        bool lastBtnState;         // Stato precedente pulsante
        bool btnState;             // Stato attuale pulsante
        int32_t divisor;
        int8_t lastDirection;
        uint32_t errorCount;
        bool valid;
        uint16_t speed;
        unsigned long lastChangeTime;

        EncoderConfig() : 
            pinA(0), pinB(0), pinBtn(-1),
            lastState(0), encDir(0),
            position(0), lastTime(0), lastBtnTime(0),
            lastBtnState(false), btnState(false),
            divisor(4), lastDirection(0), errorCount(0),
            valid(true), speed(0), lastChangeTime(0) {}
    };
    
    const int numEncoders;
    EncoderConfig* encoders;
    const unsigned long encoderDebounceTime;

    // Profili
    int currentProfile;
    const int numProfiles;

    // Private methods
    void initializeArrays();
    void cleanupArrays();
    void updateEncoder(int index);
    void processButtonPress(int row, int col, bool state);

public:
    // Costruttore con pulsanti encoder
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

    // Costruttore senza pulsanti encoder
    SimRacingController(
        const int* rowPins, int numRows,
        const int* colPins, int numCols,
        const int* encoderPinsA, const int* encoderPinsB,
        int numEncoders,
        int numProfiles,
        unsigned long buttonDebounce = 50,
        unsigned long encoderDebounce = 5
    );
    
    ~SimRacingController();

    void begin();
    void update();
    
    // Configurazione
    void setEncoderDivisor(int encoderIndex, int32_t divisor);
    void setEncoderPosition(int encoderIndex, int32_t position);
    void setProfile(int profile);
    int getProfile() const;
    
    // Getters per stato
    int32_t getEncoderPosition(int index) const;
    int8_t getEncoderDirection(int index) const;
    uint16_t getEncoderSpeed(int index) const;
    bool getButtonState(int row, int col) const;
    bool isEncoderValid(int index) const;
    bool getEncoderButtonState(int index) const;
    
    // Callback setters
    typedef void (*ButtonCallback)(int profile, int row, int col, bool state);
    typedef void (*EncoderCallback)(int profile, int encoder, int direction);
    typedef void (*EncoderButtonCallback)(int profile, int encoder, bool pressed);
    
    void setButtonCallback(ButtonCallback callback);
    void setEncoderCallback(EncoderCallback callback);
    void setEncoderButtonCallback(EncoderButtonCallback callback);

private:
    ButtonCallback onButtonChange;
    EncoderCallback onEncoderChange;
    EncoderButtonCallback onEncoderButtonChange;
};

#endif
