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

// Encoder configuration
const int NUM_ENCODERS = 2;
const int encoderPinsA[NUM_ENCODERS] = {10, 12};
const int encoderPinsB[NUM_ENCODERS] = {11, 13};
const int encoderBtnPins[NUM_ENCODERS] = {14, 15};  // Optional, can be omitted

// Create controller instance
SimRacingController controller;

// Matrix button callback
void onMatrixChange(int profile, int row, int col, bool state) {
    Serial.print("Matrix Button - Row: ");
    Serial.print(row);
    Serial.print(" Col: ");
    Serial.print(col);
    Serial.print(" State: ");
    Serial.println(state ? "PRESSED" : "RELEASED");
}

// Encoder rotation callback
void onEncoderChange(int profile, int encoder, int direction) {
    Serial.print("Encoder ");
    Serial.print(encoder);
    Serial.print(" Position: ");
    Serial.print(controller.getEncoderPosition(encoder));
    Serial.print(" Direction: ");
    Serial.println(direction > 0 ? "CW" : "CCW");
}

// Encoder button callback
void onEncoderButtonChange(int profile, int encoder, bool state) {
    Serial.print("Encoder ");
    Serial.print(encoder);
    Serial.print(" Button: ");
    Serial.println(state ? "PRESSED" : "RELEASED");
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("SimRacingController - Basic Example");
    
    // Configure matrix and encoders
    controller.setMatrix(rowPins, MATRIX_ROWS, colPins, MATRIX_COLS);
    controller.setEncoders(encoderPinsA, encoderPinsB, encoderBtnPins, NUM_ENCODERS);
    
    // Set callbacks
    controller.setMatrixCallback(onMatrixChange);
    controller.setEncoderCallback(onEncoderChange);
    controller.setEncoderButtonCallback(onEncoderButtonChange);
    
    // Initialize controller
    controller.begin();
    
    Serial.println("Controller ready!");
}

void loop() {
    // Update controller state
    controller.update();
}