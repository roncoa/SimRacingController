/**************************
 * SimRacing ButtonBox ACC
 * v 2.1.0
 * by roncoa@gmail.com
 * 27/01/2025
 **************************/

#include <KeySequence.h>
#include <SimRacingController.h>
#include "Sequenze.h"

// Debug configuration
#define DEBUG true

// Hardware configuration
#ifdef ARDUINO_ARCH_ESP32
    // Matrix Configuration
    #define MATRIX_ROWS 4
    #define MATRIX_COLS 5
    const int rowPins[MATRIX_ROWS] = {4, 5, 6, 7};
    const int colPins[MATRIX_COLS] = {15, 16, 17, 18, 8};

    // Encoder Configuration
    #define NUM_ENCODERS 5
    const int encoderPinsA[NUM_ENCODERS] = {37, 9, 11, 13, 3};
    const int encoderPinsB[NUM_ENCODERS] = {36, 10, 12, 14, 46};
    //const int encoderBtnPins[NUM_ENCODERS] = {38, 35, 45, 47, 21};  // Optional
#else
    // Matrix Configuration
    #define MATRIX_ROWS 3
    #define MATRIX_COLS 5
    const int rowPins[MATRIX_ROWS] = {2, 3, 4};
    const int colPins[MATRIX_COLS] = {5, 6, 7, 8, 9};

    // Encoder Configuration
    #define NUM_ENCODERS 4
    const int encoderPinsA[NUM_ENCODERS] = {20, 18, 14, 10};
    const int encoderPinsB[NUM_ENCODERS] = {21, 19, 15, 16};
    //const int encoderBtnPins[NUM_ENCODERS] = {0, 0, 0, 0};  // Optional
#endif

// Create instances
KeySequence keys;
SimRacingController controller;

// Matrix button callback
void onMatrixChange(int profile, int row, int col, bool state) {
    if (profile == 0) { // ACC profile
        if (state) {
            // Row 1 - Basic Controls
            if (row == 0) {
                switch (col) {
                    case 0: keys.sendSequence(ACC_EngagePitLimiter); break;
                    case 1: keys.sendSequence(ACC_CycleCarLightStages); break;
                    case 2: keys.sendSequence(ACC_LeftDirectionalLight); break;
                    case 3: keys.sendSequence(ACC_RightDirectionalLight); break;
                    case 4: keys.sendSequence(ACC_CycleMultifunctionDisplay); break;
                }
            }
            // Row 2 - Car Systems
            else if (row == 1) {
                switch (col) {
                    case 0: keys.sendSequence(ACC_Starter); break;
                    case 1: keys.sendSequence(ACC_EnableRainLights); break;
                    case 2: keys.sendSequence(ACC_EnableFlashingLights); break;
                    case 3: keys.sendSequence(ACC_CycleWiper); break;
                    case 4: keys.sendSequence(ACC_Savereplay); break;
                }
            }
            // Row 3 - Additional Controls
            else if (row == 2) {
                switch (col) {
                    case 0: keys.sendSequence(ACC_IngitionSequence); break;
                    case 1: keys.sendSequence(ACC_IncreaseTCC); break;
                    case 2: keys.sendSequence(ACC_DecreaseTCC); break;
                    case 3: keys.sendSequence(AUX2); break;
                    case 4: keys.sendSequence(AUX1); break;
                }
            }
        } else {
            keys.releaseAll();
        }
    }
}

// Encoder callback
void onEncoderChange(int profile, int encoder, int direction) {
    if (profile == 0) { // ACC profile
        switch (encoder) {
            case 0:  // Traction Control
                keys.sendSequence(direction > 0 ? ACC_IncreaseTC : ACC_DecreaseTC);
                break;
            case 1:  // ABS
                keys.sendSequence(direction > 0 ? ACC_IncreaseABS : ACC_DecreaseABS);
                break;
            case 2:  // Engine Map
                keys.sendSequence(direction > 0 ? ACC_IncreaseEngineMap : ACC_DecreaseEngineMap);
                break;
            case 3:  // Brake Bias
                keys.sendSequence(direction > 0 ? ACC_IncreaseBrakeBias : ACC_DecreaseBrakeBIas);
                break;
        }
        keys.releaseAll();
    }
}

void setup() {
    // Initialize KeySequence
    keys.setDebug(DEBUG);
    keys.begin();
    keys.setAutoRelease(false);
    keys.setDefaultDelay(150);

    // Configure controller
    controller.setMatrix(rowPins, MATRIX_ROWS, colPins, MATRIX_COLS);
    controller.setEncoders(encoderPinsA, encoderPinsB, NUM_ENCODERS);
    // Or with buttons:
    //controller.setEncoders(encoderPinsA, encoderPinsB, encoderBtnPins, NUM_ENCODERS);
    
    // Set callbacks
    controller.setMatrixCallback(onMatrixChange);
    controller.setEncoderCallback(onEncoderChange);
    
    // Set encoder sensitivity
    for (int i = 0; i < NUM_ENCODERS; i++) {
        controller.setEncoderDivisor(i, 4);
    }

    // Initialize controller
    controller.begin();

    // Initial delay
    delay(1000);
}

void loop() {
    controller.update();
}