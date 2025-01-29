/**************************
 * SimRacingController
 * Basic Example
 * v 2.1.0
 * by roncoa@gmail.com
 * 27/01/2025
 **************************/

#include <SimRacingController.h>

// Matrix configuration
const int MATRIX_ROWS = 3;
const int MATRIX_COLS = 3;
const int rowPins[MATRIX_ROWS] = {2, 3, 4};
const int colPins[MATRIX_COLS] = {5, 6, 7};

// Direct GPIO configuration
const int NUM_GPIO = 2;
const int gpioPins[NUM_GPIO] = {8, 9};  // Direct buttons on pins 8 and 9

// Encoder configuration
const int NUM_ENCODERS = 2;
const int encoderPinsA[NUM_ENCODERS] = {10, 12};
const int encoderPinsB[NUM_ENCODERS] = {11, 13};
const int encoderBtnPins[NUM_ENCODERS] = {14, 15};  // Optional

// MCP23017 configuration
const uint8_t NUM_MCP = 2;
McpConfig mcpConfigs[NUM_MCP] = {
    McpConfig(0x20, true, false),  // Address 0x20, pullups on, no interrupt
    McpConfig(0x21, true, true, 16) // Address 0x21, pullups on, interrupt on pin 16
};

// Create controller
SimRacingController controller;

// Callback functions
void onMatrixChange(int profile, int row, int col, bool state) {
    Serial.printf("Matrix [%d,%d] = %d (Profile %d)\n", row, col, state, profile);
}

void onGpioChange(int profile, int gpio, bool state) {
    Serial.printf("GPIO %d = %d (Profile %d)\n", gpio, state, profile);
}

void onEncoderChange(int profile, int encoder, int direction) {
    Serial.printf("Encoder %d: %s (Profile %d)\n", 
        encoder, direction > 0 ? "CW" : "CCW", profile);
}

void onMcpChange(int profile, int device, int pin, bool state) {
    Serial.printf("MCP %d Pin %d = %d (Profile %d)\n", 
        device, pin, state, profile);
}

void setup() {
    Serial.begin(115200);
    
    // Configure components
    controller.setMatrix(rowPins, MATRIX_ROWS, colPins, MATRIX_COLS);
    controller.setGpio(gpioPins, NUM_GPIO);  // Configure direct GPIO buttons
    controller.setEncoders(encoderPinsA, encoderPinsB, encoderBtnPins, NUM_ENCODERS);
    controller.setMcpDevices(mcpConfigs, NUM_MCP);
    
    // Set callbacks
    controller.setMatrixCallback(onMatrixChange);
    controller.setGpioCallback(onGpioChange);  // GPIO callback
    controller.setEncoderCallback(onEncoderChange);
    controller.setMcpCallback(onMcpChange);
    
    // Optional: Set custom debounce times (in milliseconds)
    controller.setDebounceTime(50, 5);  // 50ms for matrix/GPIO, 5ms for encoders
    
    // Initialize controller
    if (!controller.begin()) {
        Serial.println("Error: " + String(controller.getLastError().message));
        while(1);
    }
}

void loop() {
    controller.update();
    
    // Optional: Direct state reading example
    if (controller.getGpioState(0)) {  // Check first GPIO button
        // Button is pressed
    }
}
