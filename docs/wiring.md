# Hardware Wiring Guide v2.1.0

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
Each button should have a diode in series to prevent ghosting
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

## Direct GPIO Buttons
### Basic Configuration
```
Simple direct connection:
GPIO Pin (INPUT_PULLUP) --- [BTN] --- GND

Multiple buttons:
GPIO1 --- [BTN] --- GND
GPIO2 --- [BTN] --- GND
GPIO3 --- [BTN] --- GND
```

## Rotary Encoders
### Basic Configuration
```
Encoder pinout:
A --- Pin A (CLK)
B --- Pin B (DT)
C --- Common/GND
SW -- Button (optional)

All encoder pins should be set to INPUT_PULLUP
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

## Hardware Requirements

### Matrix Components
- Tactile switches or keyboard switches
- Diodes (1N4148 or similar) - one per button
- Pull-up resistors (10kΩ) if not using internal pull-ups

### GPIO Buttons
- Tactile switches
- Pull-up resistors (10kΩ) if not using internal pull-ups

### Encoders
- EC11 or similar rotary encoders
- Pull-up resistors (10kΩ) if not using internal pull-ups
- Capacitors (100nF) for additional debouncing (optional)

## Recommended Parts

### Buttons and Switches
- Matrix buttons: Cherry MX or similar
- Direct GPIO buttons: Quality tactile switches
- Diodes: 1N4148 or similar switching diodes
- Pull-up resistors: 10kΩ (if not using internal pull-ups)

### Encoders
- Type: EC11 series or similar quality encoders
- Resolution: 20-30 pulses per rotation recommended
- With or without push button function

### Additional Components
- Bypass capacitors: 100nF ceramic
- PCB mount connectors recommended
- Quality hook-up wire (22-28 AWG)

## Installation Tips

### General
- Keep wires short and organized
- Use connectors for removable parts
- Consider strain relief for cables

### Matrix
- Install diodes in correct orientation
- Check for proper row/column isolation
- Test each intersection individually

### Encoders
- Mount securely to prevent mechanical stress
- Consider mechanical alignment
- Test rotation before final assembly

### GPIO Buttons
- Keep wires short to minimize noise
- Consider adding external pull-up resistors for reliability
- Test each button individually

## Troubleshooting

### Matrix Issues
- Check diode orientation
- Verify pull-up resistors
- Test for shorts between rows/columns

### Encoder Issues
- Verify A/B pin connections
- Check for proper grounding
- Consider adding debounce capacitors

### GPIO Button Issues
- Check pull-up configuration
- Verify ground connections
- Test for stuck buttons

## Power Considerations
- Use external power for many components
- Add bypass capacitors near ICs
- Calculate total current requirements
- Consider voltage drops in long runs
