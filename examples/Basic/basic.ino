/**
 * Basic example showing button matrix and encoder handling
 */
#include <SimRacingController.h>

// Matrix configuration
#define ROWS 3
#define COLS 5
const int ROW_PINS[ROWS] = {2, 3, 4};
const int COL_PINS[COLS] = {5, 6, 7, 8, 9};

// Encoder configuration
#define NUM_ENCODERS 4
const int ENC_PINS_A[NUM_ENCODERS] = {20, 18, 14, 10};
const int ENC_PINS_B[NUM_ENCODERS] = {21, 19, 15, 16};
const int ENC_BTN_PINS[NUM_ENCODERS] = {22, 23, 24, 25};

// Controller instance
SimRacingController controller(
    ROW_PINS, ROWS,
    COL_PINS, COLS,
    ENC_PINS_A, ENC_PINS_B,
    ENC_BTN_PINS,
    NUM_ENCODERS,
    1  // Single profile
);

void onButtonEvent(int profile, int row, int col, bool pressed) {
    Serial.print("Button [");
    Serial.print(row); Serial.print(","); Serial.print(col);
    Serial.print("] "); Serial.println(pressed ? "Pressed" : "Released");
}

void onEncoderEvent(int profile, int encoder, int direction) {
    Serial.print("Encoder "); Serial.print(encoder);
    Serial.print(" Direction "); Serial.println(direction);
}

void onEncoderButtonEvent(int profile, int encoder, bool pressed) {
    Serial.print("Encoder "); Serial.print(encoder);
    Serial.print(" Button "); Serial.println(pressed ? "Pressed" : "Released");
}

void setup() {
    Serial.begin(115200);
    while(!Serial) { ; }
    
    controller.setButtonCallback(onButtonEvent);
    controller.setEncoderCallback(onEncoderEvent);
    controller.setEncoderButtonCallback(onEncoderButtonEvent);
    controller.begin();
}

void loop() {
    controller.update();
}
