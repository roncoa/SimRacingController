# Hardware Wiring Guide

## Button Matrix

### Basic Configuration
```
3x5 Matrix Example:

[ROW1] --- [BTN] --- [COL1]
[ROW1] --- [BTN] --- [COL2]
[ROW2] --- [BTN] --- [COL1]
[ROW2] --- [BTN] --- [COL2]

ROW pins: OUTPUT, normally HIGH
COL pins: INPUT_PULLUP
```

### Connection Diagram
```
Arduino        Button Matrix
---------     -------------
ROW1 (2) ----+----[BTN]----+----[BTN]----+
             |             |             |
ROW2 (3) ----+----[BTN]----+----[BTN]----+
             |             |             |
ROW3 (4) ----+----[BTN]----+----[BTN]----+
             |             |             |
             |  COL1 (5)   |  COL2 (6)   |  COL3 (7)
             ↓             ↓             ↓
         INPUT_PULLUP  INPUT_PULLUP  INPUT_PULLUP
```

## Rotary Encoders

### Basic Configuration
```
Encoder pinout:
A --- Pin A (CLK)
B --- Pin B (DT)
C --- Common/GND
SW -- Button (optional)
```

### Connection Diagram
```
Arduino         Encoder
-------         -------
Pin A   <----- CLK (A)
Pin B   <----- DT  (B)
Button  <----- SW  (opt)
GND    <----- Common (C)
```

### Multiple Encoders
```
Arduino         Encoder1        Encoder2
-------         --------        --------
20      <----- CLK (A)
21      <----- DT  (B)
22      <----- SW  (opt)
18      <------------------- CLK (A)
19      <------------------- DT  (B)
23      <------------------- SW  (opt)
GND     <----- GND    <----- GND
```

## Status LED
```
Arduino         LED
-------         ---
Pin     -----> LED+ (with resistor)
GND     -----> LED-
```

## Function Button
```
Arduino         Button
-------         ------
Pin     <----- Button
        -----> GND
```

## Power Considerations
- Use external power for many LEDs
- Add bypass capacitors for noise reduction
- Use appropriate current limiting resistors

## Recommended Parts
- Encoders: EC11 or similar
- Buttons: Cherry MX or similar
- Pull-up resistors: 10kΩ if not using internal pull-ups
- LED resistors: 220Ω to 1kΩ depending on LED
