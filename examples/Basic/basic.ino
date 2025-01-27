/**************************
 * SimRacingController
 * Basic Example
 * v 2.1.0
 * by roncoa@gmail.com
 * 27/01/2025
 **************************/

#include <SimRacingController.h>

// Matrix configuration - 3x3 button matrix
const int MATRIX_ROWS = 3;
const int MATRIX_COLS = 3;
const int rowPins[MATRIX_ROWS] = {2, 3, 4};
const int colPins[MATRIX_COLS] = {5, 6, 7};

// GPIO configuration - 2 direct buttons
const int NUM_GPIO = 2;
const int gpioPins[NUM_GPIO] = {8, 9};

// Encoder configuration - 1 encoder with button
const int NUM_ENCODERS = 1;
const int encoderPinsA[NUM_ENCODERS] = {10};
const int encoderPinsB[NUM_ENCODERS] = {11};
const int encoderBtnPins[NUM_ENCODERS] = {12};

// Create controller instance
SimRacingController controller;

// Callback functions
void onMatrixChange(int profile, int row, int col, bool state) {
    Serial.print("Matrix Button - Row: ");
    Serial.print(row);
    Serial.print(" Col: ");
    Serial.print(col);
    Serial.print(" State: ");
    Serial.println(state ? "PRESSED" : "RELEASED");
}

void onGpioChange(int profile, int gpio, bool state) {
    Serial.print("GPIO Button - Pin: ");
    Serial.print(gpio);
    Serial.print(" State: ");
    Serial.println(state ? "PRESSED" : "RELEASED");
}

void onEncoderChange(int profile, int encoder, int direction) {
    Serial.print("Encoder - Direction: ");
    Serial.print(direction > 0 ? "CW" : "CCW");
    Serial.print(" Position: ");
    Serial.println(controller.getEncoderPosition(encoder));
}

void onEncoderButtonChange(int profile, int encoder, bool state) {
    Serial.print("Encoder Button - State: ");
    Serial.println(state ? "PRESSED" : "RELEASED");
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("SimRacingController - Basic Example");
    
    // Configure components
    controller.setMatrix(rowPins, MATRIX_ROWS, colPins, MATRIX_COLS);
    controller.setGpio(gpioPins, NUM_GPIO);
    controller.setEncoders(encoderPinsA, encoderPinsB, encoderBtnPins, NUM_ENCODERS);
    
    // Set callbacks
    controller.setMatrixCallback(onMatrixChange);
    controller.setGpioCallback(onGpioChange);
    controller.setEncoderCallback(onEncoderChange);
    controller.setEncoderButtonCallback(onEncoderButtonChange);
    
    // Initialize controller
    controller.begin();
    
    Serial.println("Controller initialized!");
}

void loop() {
    // Update controller state
    controller.update();
}
