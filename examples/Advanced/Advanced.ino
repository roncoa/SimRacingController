/**************************
 * SimRacingController
 * Advanced Example
 * v 2.1.0
 * by roncoa@gmail.com
 * 27/01/2025
 **************************/

#include <SimRacingController.h>

// Pin configurations
#ifdef ARDUINO_ARCH_ESP32
    // Matrix configuration
    const int MATRIX_ROWS = 4;
    const int MATRIX_COLS = 4;
    const int rowPins[MATRIX_ROWS] = {4, 5, 6, 7};
    const int colPins[MATRIX_COLS] = {15, 16, 17, 18};

    // Encoder configuration
    const int NUM_ENCODERS = 3;
    const int encoderPinsA[NUM_ENCODERS] = {25, 27, 32};
    const int encoderPinsB[NUM_ENCODERS] = {26, 28, 33};
    const int encoderBtnPins[NUM_ENCODERS] = {23, 19, 34};

    // I2C configuration
    const int I2C_SDA = 21;
    const int I2C_SCL = 22;
#else
    // Matrix configuration
    const int MATRIX_ROWS = 3;
    const int MATRIX_COLS = 3;
    const int rowPins[MATRIX_ROWS] = {2, 3, 4};
    const int colPins[MATRIX_COLS] = {5, 6, 7};

    // Encoder configuration
    const int NUM_ENCODERS = 2;
    const int encoderPinsA[NUM_ENCODERS] = {10, 12};
    const int encoderPinsB[NUM_ENCODERS] = {11, 13};
    const int encoderBtnPins[NUM_ENCODERS] = {14, 15};
#endif

// I2C Expander configuration
const int NUM_EXPANDERS = 2;  // Using 2 expanders

// Number of available profiles
const int NUM_PROFILES = 3;

// Profile names for display
const char* profileNames[] = {
    "Racing",
    "Setup",
    "Menu"
};

// Create controller instance
SimRacingController controller;

// Matrix button callback
void onMatrixChange(int profile, int row, int col, bool state) {
    Serial.print("Profile: ");
    Serial.print(profileNames[profile]);
    Serial.print(" - Matrix [");
    Serial.print(row);
    Serial.print("][");
    Serial.print(col);
    Serial.print("] ");
    Serial.println(state ? "PRESSED" : "RELEASED");

    // Example: check for button combinations
    if(state && controller.getMatrixState(0, 0)) {
        Serial.println("Button combination detected!");
    }
}

// Expander callback
void onExpanderChange(int profile, int expander, uint8_t state) {
    Serial.print("Profile: ");
    Serial.print(profileNames[profile]);
    Serial.print(" - Expander ");
    Serial.print(expander);
    Serial.print(" State: 0x");
    Serial.println(state, HEX);

    // Example: check specific pins
    for(int pin = 0; pin < 8; pin++) {
        if(!(state & (1 << pin))) {  // Active LOW
            Serial.print("Pin ");
            Serial.print(pin);
            Serial.println(" is active");
        }
    }
}

// Encoder rotation callback
void onEncoderChange(int profile, int encoder, int direction) {
    int32_t position = controller.getEncoderPosition(encoder);
    uint16_t speed = controller.getEncoderSpeed(encoder);
    
    Serial.print("Profile: ");
    Serial.print(profileNames[profile]);
    Serial.print(" - Encoder ");
    Serial.print(encoder);
    Serial.print(" Dir: ");
    Serial.print(direction > 0 ? "CW" : "CCW");
    Serial.print(" Pos: ");
    Serial.print(position);
    Serial.print(" Speed: ");
    Serial.println(speed);

    // Example: automatic position reset at speed threshold
    if(speed > 50) {
        controller.setEncoderPosition(encoder, 0);
        Serial.println("Speed limit reached - Position reset");
    }
}

// Encoder button callback
void onEncoderButtonChange(int profile, int encoder, bool state) {
    static unsigned long pressTime = 0;
    
    Serial.print("Profile: ");
    Serial.print(profileNames[profile]);
    Serial.print(" - Encoder ");
    Serial.print(encoder);
    Serial.print(" Button ");
    Serial.println(state ? "PRESSED" : "RELEASED");

    // Example: long press detection
    if(state) {
        pressTime = millis();
    } else {
        if(millis() - pressTime > 1000) {
            Serial.println("Long press detected!");
            controller.setEncoderPosition(encoder, 0);
        }
    }
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("SimRacingController - Advanced Example");
    
    // Configure matrix
    controller.setMatrix(rowPins, MATRIX_ROWS, colPins, MATRIX_COLS);
    
    // Configure encoders
    controller.setEncoders(encoderPinsA, encoderPinsB, encoderBtnPins, NUM_ENCODERS);
    
    // Configure expanders if on ESP32
    #ifdef ARDUINO_ARCH_ESP32
        controller.setExpander(I2C_SDA, I2C_SCL, EXPANDER_PCF8574, NUM_EXPANDERS);
    #endif
    
    // Configure profiles and timing
    controller.setProfiles(NUM_PROFILES);
    controller.setDebounceTime(50, 5);  // matrix=50ms, encoder=5ms
    
    // Configure encoder sensitivity
    for(int i = 0; i < NUM_ENCODERS; i++) {
        controller.setEncoderDivisor(i, 4);  // 1 step = 4 detents
    }
    
    // Set callbacks
    controller.setMatrixCallback(onMatrixChange);
    controller.setEncoderCallback(onEncoderChange);
    controller.setEncoderButtonCallback(onEncoderButtonChange);
    controller.setExpanderCallback(onExpanderChange);
    
    // Initialize controller
    controller.begin();
    
    Serial.println("Controller initialized!");
    Serial.print("Active profile: ");
    Serial.println(profileNames[controller.getProfile()]);
}

void loop() {
    // Update controller state
    controller.update();
    
    // Example: automatic profile cycling
    static unsigned long lastProfileChange = 0;
    if(millis() - lastProfileChange > 5000) {  // Every 5 seconds
        lastProfileChange = millis();
        int nextProfile = (controller.getProfile() + 1) % NUM_PROFILES;
        controller.setProfile(nextProfile);
        Serial.print("Profile changed to: ");
        Serial.println(profileNames[nextProfile]);
    }

    // Example: print encoder states periodically
    static unsigned long lastStatusPrint = 0;
    if(millis() - lastStatusPrint > 1000) {  // Every second
        lastStatusPrint = millis();
        for(int i = 0; i < NUM_ENCODERS; i++) {
            if(!controller.isEncoderValid(i)) {
                Serial.print("Encoder ");
                Serial.print(i);
                Serial.println(" has errors!");
            }
        }
    }
}