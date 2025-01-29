/**************************
 * SimRacingController
 * Advanced Example
 * v 2.2.0
 * by roncoa@gmail.com
 * 29/01/2025
 **************************/

#include <SimRacingController.h>

// Pin configurations
#ifdef ARDUINO_ARCH_ESP32
    // Matrix configuration
    const int MATRIX_ROWS = 4;
    const int MATRIX_COLS = 4;
    const int rowPins[MATRIX_ROWS] = {4, 5, 6, 7};
    const int colPins[MATRIX_COLS] = {15, 16, 17, 18};

    // GPIO configuration
    const int NUM_GPIO = 2;
    const int gpioPins[NUM_GPIO] = {12, 14};

    // Encoder configuration
    const int NUM_ENCODERS = 3;
    const int encoderPinsA[NUM_ENCODERS] = {25, 27, 32};
    const int encoderPinsB[NUM_ENCODERS] = {26, 28, 33};
    const int encoderBtnPins[NUM_ENCODERS] = {23, 19, 34};

#else
    // Matrix configuration
    const int MATRIX_ROWS = 3;
    const int MATRIX_COLS = 3;
    const int rowPins[MATRIX_ROWS] = {2, 3, 4};
    const int colPins[MATRIX_COLS] = {5, 6, 7};

    // GPIO configuration
    const int NUM_GPIO = 2;
    const int gpioPins[NUM_GPIO] = {8, 9};

    // Encoder configuration
    const int NUM_ENCODERS = 2;
    const int encoderPinsA[NUM_ENCODERS] = {10, 12};
    const int encoderPinsB[NUM_ENCODERS] = {11, 13};
    const int encoderBtnPins[NUM_ENCODERS] = {14, 15};
#endif

// MCP23017 configuration
const uint8_t NUM_MCP = 2;
McpConfig mcpConfigs[NUM_MCP] = {
    McpConfig(0x20, true, false),     // Address 0x20, pullups on, no interrupt
    McpConfig(0x21, true, true, 16)   // Address 0x21, pullups on, interrupt on pin 16
};

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

// Error callback
bool onError(const ControllerError& error) {
    Serial.print("Error: ");
    Serial.print(error.code);
    Serial.print(" - ");
    Serial.println(error.message);
    return true; // Continue operation
}

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

// GPIO button callback
void onGpioChange(int profile, int gpio, bool state) {
    Serial.print("Profile: ");
    Serial.print(profileNames[profile]);
    Serial.print(" - GPIO ");
    Serial.print(gpio);
    Serial.print(" ");
    Serial.println(state ? "PRESSED" : "RELEASED");

    // Example: GPIO combination detection
    if(state && controller.getGpioState(1)) {
        Serial.println("GPIO combination detected!");
    }
}

// MCP23017 callback
void onMcpChange(int profile, int device, int pin, bool state) {
    Serial.print("Profile: ");
    Serial.print(profileNames[profile]);
    Serial.print(" - MCP ");
    Serial.print(device);
    Serial.print(" Pin ");
    Serial.print(pin);
    Serial.print(" ");
    Serial.println(state ? "PRESSED" : "RELEASED");
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
    Serial.println("SimRacingController - Advanced Example v2.2.0");
    
    // Configure inputs
    controller.setMatrix(rowPins, MATRIX_ROWS, colPins, MATRIX_COLS);
    controller.setGpio(gpioPins, NUM_GPIO);
    controller.setEncoders(encoderPinsA, encoderPinsB, encoderBtnPins, NUM_ENCODERS);
    controller.setMcpDevices(mcpConfigs, NUM_MCP);
    
    // Configure profiles and timing
    controller.setProfiles(NUM_PROFILES);
    controller.setDebounceTime(50, 5);  // matrix/gpio/mcp=50ms, encoder=5ms
    
    // Configure encoder sensitivity
    for(int i = 0; i < NUM_ENCODERS; i++) {
        controller.setEncoderDivisor(i, 4);  // 1 step = 4 detents
    }
    
    // Set callbacks
    controller.setErrorCallback(onError);
    controller.setMatrixCallback(onMatrixChange);
    controller.setGpioCallback(onGpioChange);
    controller.setMcpCallback(onMcpChange);
    controller.setEncoderCallback(onEncoderChange);
    controller.setEncoderButtonCallback(onEncoderButtonChange);
    
    // Initialize controller
    if (!controller.begin()) {
        Serial.println("Error: " + String(controller.getLastError().message));
        while(1);  // Stop if initialization fails
    }
    
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

    // Example: print status periodically
    static unsigned long lastStatusPrint = 0;
    if(millis() - lastStatusPrint > 1000) {  // Every second
        lastStatusPrint = millis();
        
        // Check encoder status
        for(int i = 0; i < NUM_ENCODERS; i++) {
            if(!controller.isEncoderValid(i)) {
                Serial.print("Encoder ");
                Serial.print(i);
                Serial.println(" has errors!");
            }
        }
        
        // Check power state
        if(controller.isInPowerSave()) {
            Serial.println("Controller in power save mode");
        }
    }
}
