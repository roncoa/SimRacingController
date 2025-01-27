/**************************
 * SimRacingController
 * Advanced Example
 * v 2.0.0
 * by roncoa@gmail.com
 * 27/01/2025
 **************************/

#include <SimRacingController.h>

// Matrix configuration - 4x4 button matrix
const int MATRIX_ROWS = 4;
const int MATRIX_COLS = 4;
const int rowPins[MATRIX_ROWS] = {2, 3, 4, 5};
const int colPins[MATRIX_COLS] = {6, 7, 8, 9};

// GPIO configuration - 4 direct buttons
const int NUM_GPIO = 4;
const int gpioPins[NUM_GPIO] = {10, 11, 12, 13};

// Encoder configuration - 2 encoders with buttons
const int NUM_ENCODERS = 2;
const int encoderPinsA[NUM_ENCODERS] = {14, 16};
const int encoderPinsB[NUM_ENCODERS] = {15, 17};
const int encoderBtnPins[NUM_ENCODERS] = {18, 19};

// Number of profiles
const int NUM_PROFILES = 3;

// Create controller instance
SimRacingController controller;

// Function prototypes
void handleProfile0(int row, int col, bool state);
void handleProfile1(int row, int col, bool state);
void handleProfile2(int row, int col, bool state);

// Profile names for display
const char* profileNames[] = {
    "Race Mode",
    "Pit Mode",
    "Setup Mode"
};

// Callback for matrix buttons
void onMatrixChange(int profile, int row, int col, bool state) {
    Serial.print("Profile: ");
    Serial.print(profileNames[profile]);
    Serial.print(" - Matrix Button [");
    Serial.print(row);
    Serial.print("][");
    Serial.print(col);
    Serial.print("] ");
    Serial.println(state ? "PRESSED" : "RELEASED");
    
    // Handle button based on current profile
    switch(profile) {
        case 0: handleProfile0(row, col, state); break;
        case 1: handleProfile1(row, col, state); break;
        case 2: handleProfile2(row, col, state); break;
    }
}

// Callback for GPIO buttons
void onGpioChange(int profile, int gpio, bool state) {
    Serial.print("Profile: ");
    Serial.print(profileNames[profile]);
    Serial.print(" - GPIO ");
    Serial.print(gpio);
    Serial.print(" ");
    Serial.println(state ? "PRESSED" : "RELEASED");
    
    // Example: GPIO 0 cycles through profiles when pressed
    if(gpio == 0 && state) {
        int nextProfile = (controller.getProfile() + 1) % NUM_PROFILES;
        controller.setProfile(nextProfile);
        Serial.print("Switched to profile: ");
        Serial.println(profileNames[nextProfile]);
    }
}

// Callback for encoder rotation
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
    
    // Example: Reset position if speed is high
    if(speed > 50) {
        controller.setEncoderPosition(encoder, 0);
        Serial.println("Speed limit reached - Position reset");
    }
}

// Callback for encoder buttons
void onEncoderButtonChange(int profile, int encoder, bool state) {
    Serial.print("Profile: ");
    Serial.print(profileNames[profile]);
    Serial.print(" - Encoder ");
    Serial.print(encoder);
    Serial.print(" Button ");
    Serial.println(state ? "PRESSED" : "RELEASED");
    
    // Example: Long press detection
    static unsigned long pressTime = 0;
    if(state) {
        pressTime = millis();
    } else {
        if(millis() - pressTime > 1000) {
            Serial.println("Long press detected!");
            controller.setEncoderPosition(encoder, 0);
        }
    }
}

// Profile-specific button handlers
void handleProfile0(int row, int col, bool state) {
    if(state) {
        Serial.println("Race Mode Action");
        // Add race-specific actions
    }
}

void handleProfile1(int row, int col, bool state) {
    if(state) {
        Serial.println("Pit Mode Action");
        // Add pit-specific actions
    }
}

void handleProfile2(int row, int col, bool state) {
    if(state) {
        Serial.println("Setup Mode Action");
        // Add setup-specific actions
    }
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("SimRacingController - Advanced Example");
    
    // Configure components
    controller.setMatrix(rowPins, MATRIX_ROWS, colPins, MATRIX_COLS);
    controller.setGpio(gpioPins, NUM_GPIO);
    controller.setEncoders(encoderPinsA, encoderPinsB, encoderBtnPins, NUM_ENCODERS);
    
    // Configure profiles and timing
    controller.setProfiles(NUM_PROFILES);
    controller.setDebounceTime(50, 5);  // matrix/gpio=50ms, encoder=5ms
    
    // Configure encoder sensitivity
    controller.setEncoderDivisor(0, 4);  // First encoder: 1 step = 4 detents
    controller.setEncoderDivisor(1, 2);  // Second encoder: 1 step = 2 detents
    
    // Set callbacks
    controller.setMatrixCallback(onMatrixChange);
    controller.setGpioCallback(onGpioChange);
    controller.setEncoderCallback(onEncoderChange);
    controller.setEncoderButtonCallback(onEncoderButtonChange);
    
    // Initialize controller
    controller.begin();
    
    Serial.println("Controller initialized!");
    Serial.print("Active profile: ");
    Serial.println(profileNames[controller.getProfile()]);
}

void loop() {
    // Update controller state
    controller.update();
    
    // Add your main loop code here
}
