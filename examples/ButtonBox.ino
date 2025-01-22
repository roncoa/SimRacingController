/******************************
  Arduino SimRacing ButtonBox
  v 2.0.0
  by roncoa@gmail.com
  22/01/2025
*******************************/

#include <KeySequence.h>
#include "SimRacingController.h"
#include "Sequenze.h"

#define DEBUG false

// Configurazione Matrice Pulsanti
#define RIGHE 3
#define COLONNE 5
const int PIN_RIGHE[RIGHE] = {2, 3, 4};          // GPIO RIGHE
const int PIN_COLONNE[COLONNE] = {5, 6, 7, 8, 9}; // GPIO COLONNE

// Configurazione Encoder
#define NUM_ENCODERS 4
const int ENCODER_PINS_A[NUM_ENCODERS] = {20, 18, 14, 10};  // Pin A degli encoder
const int ENCODER_PINS_B[NUM_ENCODERS] = {21, 19, 15, 16};  // Pin B degli encoder

// LED di Stato
#define USA_LED_STATO true
#define PIN_LED_STATO 17
#define INV_LED_STATO true
#define TEMPO_LAMPEGGIO 250
#define TEMPO_PAUSA 1000

// Pin Funzione
#define USA_PIN_FUNZIONE true
#define PIN_FUNZIONE 19
#define TEMPO_CAMBIO_PROFILO 500

// Antirimbalzo
#define DEBOUNCE_BOTTONI 50
#define DEBOUNCE_ENCODER 5

// Profili
#define NUM_PROFILI 1
#define PROFILO_DEFAULT 0    // Profilo iniziale (0 = primo profilo)

// Variabili per gestione LED
unsigned long ultimoTempoLed = 0;
bool ledAcceso = false;
int numeroLampeggi = 0;
bool inPausa = false;

// Variabili per gestione PIN_FUNZIONE
bool ultimoStatoFunzione = HIGH;
unsigned long tempoInzioPressione = 0;
bool profiloCambiato = false;

// Istanze
KeySequence keys;
SimRacingController controller(
    PIN_RIGHE, RIGHE,
    PIN_COLONNE, COLONNE,
    ENCODER_PINS_A, ENCODER_PINS_B, NUM_ENCODERS,
    NUM_PROFILI,
    DEBOUNCE_BOTTONI,
    DEBOUNCE_ENCODER
);

// Funzioni per profili
void impostaProfilo(int profilo) {
    if(profilo >= 0 && profilo < NUM_PROFILI) {
        controller.setProfile(profilo);
        numeroLampeggi = profilo + 1;  // Aggiorna i lampeggi del LED
        inPausa = false;               // Resetta stato LED
    }
}

int ottieniProfilo() {
    return controller.getProfile();
}

// Gestione LED di stato
void aggiornaLedStato() {
    if (!USA_LED_STATO) return;
    
    unsigned long tempoCorrente = millis();
    
    if (numeroLampeggi <= 0) {
        numeroLampeggi = ottieniProfilo() + 1;
    }
    
    if (inPausa) {
        if (tempoCorrente - ultimoTempoLed >= TEMPO_PAUSA) {
            inPausa = false;
            numeroLampeggi = ottieniProfilo() + 1;
            ultimoTempoLed = tempoCorrente;
            ledAcceso = false;
            digitalWrite(PIN_LED_STATO, LOW ^ INV_LED_STATO);
        }
        return;
    }
    
    if (tempoCorrente - ultimoTempoLed >= TEMPO_LAMPEGGIO) {
        ultimoTempoLed = tempoCorrente;
        if (ledAcceso) {
            digitalWrite(PIN_LED_STATO, LOW ^ INV_LED_STATO);
            ledAcceso = false;
            numeroLampeggi--;
            if (numeroLampeggi <= 0) {
                inPausa = true;
            }
        } else {
            digitalWrite(PIN_LED_STATO, HIGH ^ INV_LED_STATO);
            ledAcceso = true;
        }
    }
}

// Gestione pulsante funzione
void aggiornaPulsanteFunzione() {
    if (!USA_PIN_FUNZIONE) return;
    
    bool statoBottone = digitalRead(PIN_FUNZIONE);
    if (statoBottone == LOW && ultimoStatoFunzione == HIGH) {
        tempoInzioPressione = millis();
        profiloCambiato = false;
    } else if (statoBottone == LOW) {
        unsigned long durataPressione = millis() - tempoInzioPressione;
        if (durataPressione >= TEMPO_CAMBIO_PROFILO && !profiloCambiato) {
            int nuovoProfilo = (ottieniProfilo() + 1) % NUM_PROFILI;
            impostaProfilo(nuovoProfilo);
            profiloCambiato = true;
        }
    }
    ultimoStatoFunzione = statoBottone;
}

// Callback per i pulsanti
void onButtonEvent(int profile, int row, int col, bool pressed) {
    if(pressed) {
        if(profile == 0) {  // ACC profile
            // Riga 1
            if(row == 0) {
                switch(col) {
                    case 0: keys.sendSequence(ACC_EngagePitLimiter); break;
                    case 1: keys.sendSequence(ACC_CycleCarLightStages); break;
                    case 2: keys.sendSequence(ACC_LeftDirectionalLight); break;
                    case 3: keys.sendSequence(ACC_RightDirectionalLight); break;
                    case 4: keys.sendSequence(ACC_CycleMultifunctionDisplay); break;
                }
            }
            // Riga 2
            else if(row == 1) {
                switch(col) {
                    case 0: keys.sendSequence(ACC_Starter); break;
                    case 1: keys.sendSequence(ACC_EnableRainLights); break;
                    case 2: keys.sendSequence(ACC_EnableFlashingLights); break;
                    case 3: keys.sendSequence(ACC_CycleWiper); break;
                    case 4: keys.sendSequence(ACC_Savereplay); break;
                }
            }
            // Riga 3
            else if(row == 2) {
                switch(col) {
                    case 0: keys.sendSequence(ACC_IngitionSequence); break;
                    case 1: keys.sendSequence(ACC_IncreaseTCC); break;
                    case 2: keys.sendSequence(ACC_DecreaseTCC); break;
                    case 3: keys.sendSequence(ACC_CycleRacelogic); break;
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
    if(profile == 0) {  // ACC profile
        switch(encoder) {
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
    if(DEBUG) {
        Serial.begin(115200);
        while(!Serial) { ; }
    }
    
    // Inizializza KeySequence
    keys.setDebug(DEBUG);
    keys.begin();
    keys.setAutoRelease(false);
    keys.setDefaultDelay(150);
    
    // Configura LED di stato
    if(USA_LED_STATO) {
        pinMode(PIN_LED_STATO, OUTPUT);
        digitalWrite(PIN_LED_STATO, LOW ^ INV_LED_STATO);
    }
    
    // Configura pin funzione
    if(USA_PIN_FUNZIONE) {
        pinMode(PIN_FUNZIONE, INPUT_PULLUP);
    }
    
    // Registra le callback
    controller.setButtonCallback(onButtonEvent);
    controller.setEncoderCallback(onEncoderEvent);
    
    // Inizializza il controller
    controller.begin();
    
    // Imposta il divisore per gli encoder
    for(int i = 0; i < NUM_ENCODERS; i++) {
        controller.setEncoderDivisor(i, 4);
    }
    
    // Imposta il profilo di default
    impostaProfilo(PROFILO_DEFAULT);
    
    delay(1000);  // Attendi prima di iniziare
}

void loop() {
    controller.update();
    aggiornaLedStato();
    aggiornaPulsanteFunzione();
}
