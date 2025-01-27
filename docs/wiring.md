# Hardware Wiring Guide v2.1.0

## Button Matrix
### Basic Configuration
```
3x5 Matrix Example:
[ROW1] --- [BTN] --- [COL1]
[ROW1] --- [BTN] --- [COL2]
[ROW2] --- [BTN] --- [COL1]
[ROW2] --- [BTN] --- [COL2]

Notes:
- ROW pins: OUTPUT, normally HIGH
- COL pins: INPUT_PULLUP
- Each button needs a diode in series to prevent ghosting
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

All pins should be set to INPUT_PULLUP
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

## I2C Expanders

### MCP23017 Configuration
```
Arduino         MCP23017
-------         --------
SDA     <----> SDA
SCL     <----> SCL
GND     <----> GND/VSS
VCC     <----> VDD
         ----> A0 (GND for first chip, VCC for second)
         ----> A1 (GND)
         ----> A2 (GND)
         ----> RESET (VCC)
```

### PCF8574/PCF8574A Configuration
```
Arduino         PCF8574/A
-------         ---------
SDA     <----> SDA
SCL     <----> SCL
GND     <----> GND
VCC     <----> VCC
         ----> A0 (GND for first chip, VCC for second)
         ----> A1 (GND)
         ----> A2 (GND)
```

### I2C Addresses
```
Device          Base Address   Address Range
-------         ------------   -------------
MCP23017        0x20          0x20-0x27
PCF8574         0x20          0x20-0x27
PCF8574A        0x38          0x38-0x3F
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

### I2C Expanders
- MCP23017, PCF8574, or PCF8574A
- Pull-up resistors (4.7kΩ) for I2C lines
- Bypass capacitors (100nF) near each IC

## Installation Tips

### General
- Keep wires short and organized
- Use connectors for removable parts
- Add bypass capacitors near ICs
- Consider strain relief for cables

### Matrix
- Install diodes in correct orientation (cathode to column)
- Check for proper row/column isolation
- Test each intersection individually
- Consider pull-downs for outputs (optional)

### Encoders
- Mount securely to prevent mechanical stress
- Consider mechanical alignment
- Test rotation before final assembly
- Add debounce capacitors if needed

### I2C Expanders
- Keep I2C lines short
- Use appropriate pull-up resistors
- Check address conflicts
- Add bypass capacitors
- Consider address jumpers for multiple devices

## Power Considerations
- Use external power for many components
- Add bypass capacitors near ICs
- Calculate total current requirements
- Consider voltage drops in long runs
- Use appropriate power distribution

## Common Issues and Solutions

### Matrix Ghosting
- Verify diode orientation
- Check for missing diodes
- Test for shorts between rows/columns
- Verify pull-up resistors

### Encoder Issues
- Check pin connections
- Verify common ground
- Add debounce capacitors
- Check encoder quality
- Verify mechanical mounting

### I2C Issues
- Verify addresses
- Check pull-up resistors
- Measure I2C voltage levels
- Test each device individually
- Check for address conflicts