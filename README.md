# SimRacing Controller Library

Una libreria Arduino completa per la gestione di controller personalizzati per simulazione di guida, con supporto avanzato per matrici di pulsanti ed encoder. Progettata specificamente per buttonbox e pannelli di controllo per simulatori di guida.

## Indice
1. [Caratteristiche](#caratteristiche)
2. [Hardware Supportato](#hardware-supportato)
3. [Installazione](#installazione)
4. [Schema di Collegamento](#schema-di-collegamento)
5. [Utilizzo della Libreria](#utilizzo-della-libreria)
6. [API di Riferimento](#api-di-riferimento)
7. [Esempi](#esempi)
8. [Risoluzione Problemi](#risoluzione-problemi)
9. [Contribuire](#contribuire)
10. [Licenza](#licenza)

## Caratteristiche

### Gestione Matrice di Pulsanti
- Supporto per matrici di pulsanti di qualsiasi dimensione
- Debounce hardware e software integrato
- Rilevamento preciso di pressione e rilascio
- Gestione ottimizzata della scansione della matrice
- Pull-up configurabili (interni o esterni)

### Gestione Encoder
- Supporto per encoder rotativi standard
- Debounce hardware e software dedicato
- Sistema di divisore per regolare la sensibilità (1-4)
- Rilevamento direzione di rotazione
- Calcolo velocità di rotazione
- Validazione stato encoder
- Supporto per pulsante integrato dell'encoder (opzionale)

### Sistema di Profili
- Gestione multi-profilo integrata
- Cambio profilo in tempo reale
- Configurazione indipendente per ogni profilo
- Numero di profili configurabile

### Sistema di Eventi
- Callbacks per eventi pulsanti
- Callbacks per eventi encoder
- Informazioni complete sullo stato in tempo reale
- Supporto per gestione eventi personalizzata

## Hardware Supportato

### Microcontrollori Testati
- Arduino Leonardo (ATmega32U4)
- Arduino Pro Micro (ATmega32U4)
- Arduino Mega 2560
- Arduino UNO

### Hardware Richiesto
- Microcontrollore Arduino o compatibile
- Pulsanti per la matrice
- Encoder rotativi (opzionali)
- Resistenze pull-up (se non si usano i pull-up interni)
- Diodi per la matrice (opzionali, consigliati per matrici grandi)

## Installazione

### Tramite Arduino IDE
1. Scarica la libreria come file ZIP
2. In Arduino IDE: Sketch -> Include Library -> Add .ZIP Library
3. Seleziona il file ZIP scaricato
4. Riavvia Arduino IDE

### Installazione Manuale
1. Scarica la libreria
2. Crea una cartella "SimRacingController" nella cartella libraries di Arduino
3. Copia tutti i file nella cartella creata
4. Riavvia Arduino IDE

## Schema di Collegamento

### Matrice di Pulsanti
```
Configurazione tipica 3x5:

Righe (OUTPUT):
- Riga 1 -> Pin 2
- Riga 2 -> Pin 3
- Riga 3 -> Pin 4

Colonne (INPUT_PULLUP):
- Colonna 1 -> Pin 5
- Colonna 2 -> Pin 6
- Colonna 3 -> Pin 7
- Colonna 4 -> Pin 8
- Colonna 5 -> Pin 9
```

### Encoder
```
Per ogni encoder:
- Pin A -> INPUT_PULLUP
- Pin B -> INPUT_PULLUP
- GND -> GND

Esempio per 4 encoder:
- Encoder 1: A->20, B->21
- Encoder 2: A->18, B->19
- Encoder 3: A->14, B->15
- Encoder 4: A->10, B->16
```

## Utilizzo della Libreria

### Inizializzazione Base
```cpp
#include "SimRacingController.h"

// Configurazione pin matrice
const int ROWS = 3;
const int COLS = 5;
const int ROW_PINS[ROWS] = {2, 3, 4};
const int COL_PINS[COLS] = {5, 6, 7, 8, 9};

// Configurazione encoder
const int NUM_ENCODERS = 4;
const int ENC_PINS_A[NUM_ENCODERS] = {20, 18, 14, 10};
const int ENC_PINS_B[NUM_ENCODERS] = {21, 19, 15, 16};

// Creazione controller
SimRacingController controller(
    ROW_PINS, ROWS,
    COL_PINS, COLS,
    ENC_PINS_A, ENC_PINS_B, NUM_ENCODERS,
    1,                        // Numero profili
    50,                       // Debounce bottoni (ms)
    5                         // Debounce encoder (ms)
);
```

### Implementazione Callbacks
```cpp
// Callback pulsanti
void onButtonEvent(int profile, int row, int col, bool pressed) {
    // Esempio: stampa evento pulsante
    if(pressed) {
        Serial.print("Profilo: "); Serial.print(profile);
        Serial.print(" Riga: "); Serial.print(row);
        Serial.print(" Col: "); Serial.println(col);
    }
}

// Callback encoder
void onEncoderEvent(int profile, int encoder, int direction) {
    // Esempio: stampa evento encoder
    Serial.print("Encoder "); Serial.print(encoder);
    Serial.print(" Dir: "); Serial.println(direction);
}
```

### Setup e Loop
```cpp
void setup() {
    // Registra callbacks
    controller.setButtonCallback(onButtonEvent);
    controller.setEncoderCallback(onEncoderEvent);
    
    // Inizializza
    controller.begin();
    
    // Configurazione encoder
    for(int i = 0; i < NUM_ENCODERS; i++) {
        controller.setEncoderDivisor(i, 4);
    }
}

void loop() {
    controller.update();
}
```

## API di Riferimento

### Costruttore
```cpp
SimRacingController(
    const int* rowPins,          // Array pin righe
    int numRows,                 // Numero righe
    const int* colPins,          // Array pin colonne
    int numCols,                 // Numero colonne
    const int* encoderPinsA,     // Array pin A encoder
    const int* encoderPinsB,     // Array pin B encoder
    int numEncoders,             // Numero encoder
    int numProfiles,             // Numero profili
    unsigned long buttonDebounce, // Tempo debounce pulsanti
    unsigned long encoderDebounce // Tempo debounce encoder
);
```

### Metodi Principali
```cpp
// Inizializzazione
void begin();                    // Inizializza hardware
void update();                   // Aggiorna stati (chiamare nel loop)

// Gestione profili
void setProfile(int profile);    // Imposta profilo corrente
int getProfile();               // Ottiene profilo corrente

// Configurazione encoder
void setEncoderDivisor(int encoderIndex, int32_t divisor);
void setEncoderPosition(int encoderIndex, int32_t position);

// Lettura stati
int32_t getEncoderPosition(int index);
int8_t getEncoderDirection(int index);
uint16_t getEncoderSpeed(int index);
bool isEncoderValid(int index);
bool getButtonState(int row, int col);

// Gestione callbacks
void setButtonCallback(ButtonCallback callback);
void setEncoderCallback(EncoderCallback callback);
```

### Tipi di Callback
```cpp
// Tipo callback pulsanti
typedef void (*ButtonCallback)(int profile, int row, int col, bool state);

// Tipo callback encoder
typedef void (*EncoderCallback)(int profile, int encoder, int direction);
```

## Esempi

### Esempio Base
Vedere il file `examples/Basic/Basic.ino`

### Esempio Completo
Vedere il file `SimRacingButtonBox.ino` per un esempio completo che include:
- Gestione completa matrice 3x5
- 4 encoder
- LED di stato
- Pulsante funzione
- Supporto per Assetto Corsa Competizione

## Risoluzione Problemi

### Problemi Comuni
1. **Rimbalzo Pulsanti**
   - Aumentare il tempo di debounce
   - Verificare pull-up
   - Controllare saldature

2. **Encoder Instabili**
   - Verificare debounce encoder
   - Aumentare divisore
   - Controllare collegamenti

3. **Matrice Non Risponde**
   - Verificare direzione pin (OUTPUT/INPUT)
   - Controllare pull-up
   - Verificare diodi se presenti

### Debug
- Abilitare DEBUG nel codice principale
- Usare Serial Monitor per debugging
- Verificare i valori di debounce
- Controllare la validità degli encoder

## Contribuire

### Come Contribuire
1. Fork del repository
2. Crea un branch per le modifiche
3. Commit delle modifiche
4. Push sul tuo fork
5. Crea una Pull Request

### Linee Guida
- Seguire stile di codifica esistente
- Documentare modifiche
- Aggiungere test se possibile
- Aggiornare README se necessario

## Licenza

Questo progetto è distribuito sotto licenza MIT.

## Crediti

Sviluppato da roncoa@gmail.com
GitHub: [url_repository]
