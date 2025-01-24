/*************************
  SimRacing ButtonBox ACC
  v 2.0.0
  by roncoa@gmail.com
  22/01/2025
**************************/

#include <KeySequence.h>
#include <SimRacingController.h>
#include "Sequenze.h"

#define DEBUG true

// Configurazione Matrice Pulsanti
#ifdef ARDUINO_ARCH_ESP32
#define RIGHE 4
#define COLONNE 5
const int PIN_RIGHE[RIGHE] = { 4, 5, 6, 7};
const int PIN_COLONNE[COLONNE] = { 15, 16, 17, 18, 8};
#define NUM_ENCODERS 5
const int ENCODER_PINS_A[NUM_ENCODERS] = {37, 9, 11, 13, 3};
const int ENCODER_PINS_B[NUM_ENCODERS] = {36, 10, 12, 14, 46};
//const int ENCODER_BTN_PINS[NUM_ENCODERS] = {38, 35, 45, 47, 21};
#else
#define RIGHE 3
#define COLONNE 5
const int PIN_RIGHE[RIGHE] = {2, 3, 4};
const int PIN_COLONNE[COLONNE] = {5, 6, 7, 8, 9};
#define NUM_ENCODERS 4
const int ENCODER_PINS_A[NUM_ENCODERS] = {20, 18, 14, 10};
const int ENCODER_PINS_B[NUM_ENCODERS] = {21, 19, 15, 16};
//const int ENCODER_BTN_PINS[NUM_ENCODERS] = {0, 0, 0, 0};
#endif

// Istanze
KeySequence keys;
SimRacingController controller(
  PIN_RIGHE, RIGHE,
  PIN_COLONNE, COLONNE,
  ENCODER_PINS_A, ENCODER_PINS_B,
  // ENCODER_BTN_PINS,
  NUM_ENCODERS,
  1,                  // NUM_PROFILI
  50,                 // DEBOUNCE_BOTTONI
  5                   // DEBOUNCE_ENCODER
);

// Callback per i pulsanti
void onButtonEvent(int profile, int row, int col, bool pressed) {
  if (pressed) {
    if (profile == 0) { // ACC profile
      // Riga 1
      if (row == 0) {
        switch (col) {
          case 0: keys.sendSequence(ACC_EngagePitLimiter); break;
          case 1: keys.sendSequence(ACC_CycleCarLightStages); break;
          case 2: keys.sendSequence(ACC_LeftDirectionalLight); break;
          case 3: keys.sendSequence(ACC_RightDirectionalLight); break;
          case 4: keys.sendSequence(ACC_CycleMultifunctionDisplay); break;
        }
      }
      // Riga 2
      else if (row == 1) {
        switch (col) {
          case 0: keys.sendSequence(ACC_Starter); break;
          case 1: keys.sendSequence(ACC_EnableRainLights); break;
          case 2: keys.sendSequence(ACC_EnableFlashingLights); break;
          case 3: keys.sendSequence(ACC_CycleWiper); break;
          case 4: keys.sendSequence(ACC_Savereplay); break;
        }
      }
      // Riga 3
      else if (row == 2) {
        switch (col) {
          case 0: keys.sendSequence(ACC_IngitionSequence); break;
          case 1: keys.sendSequence(ACC_IncreaseTCC); break;
          case 2: keys.sendSequence(ACC_DecreaseTCC); break;
          case 3: keys.sendSequence(AUX2); break;
          case 4: keys.sendSequence(AUX1); break;
        }
      }
    }
  } else {
    keys.releaseAll();
  }
}

// Callback per gli encoder
void onEncoderEvent(int profile, int encoder, int direction) {
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
  // Inizializza KeySequence
  keys.setDebug(DEBUG);
  keys.begin();
  keys.setAutoRelease(false);
  keys.setDefaultDelay(150);

  // Registra callbacks
  controller.setButtonCallback(onButtonEvent);
  controller.setEncoderCallback(onEncoderEvent);

  // Inizializza il controller
  controller.begin();

  // Imposta il divisore per gli encoder
  for (int i = 0; i < NUM_ENCODERS; i++) {
    controller.setEncoderDivisor(i, 4);
  }

  delay(1000);  // Attendi prima di iniziare
}

void loop() {
  controller.update();
}
