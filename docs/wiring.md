# Hardware Wiring Guide v2.1.0

## Button Matrix

### Basic Configuration
```
3x5 Matrix Example:
[ROW1] --- [BTN+DIODE] --- [COL1]
[ROW1] --- [BTN+DIODE] --- [COL2]
[ROW2] --- [BTN+DIODE] --- [COL1]
[ROW2] --- [BTN+DIODE] --- [COL2]

Notes:
- ROW pins: OUTPUT, normally HIGH
- COL pins: INPUT_PULLUP
- Each button needs a diode in series to prevent ghosting
- Active LOW logic (button pressed = LOW)
```

### Connection Diagram
```
Arduino        Button Matrix
---------     -------------
ROW1 (2) ----+----[BTN]----[>|]----+----[BTN]----[>|]----+
             |                      |                      |
ROW2 (3) ----+----[BTN]----[>|]----+----[BTN]----[>|]----+
             |                      |                      |
ROW3 (4) ----+----[BTN]----[>|]----+----[BTN]----[>|]----+
             |                      |                      |
             |     COL1 (5)        |     COL2 (6)         |  COL3 (7)
             ↓                      ↓                      ↓
         INPUT_PULLUP          INPUT_PULLUP           INPUT_PULLUP

[>|] = Diode (1N4148 or similar, cathode to column)
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

Notes:
- All GPIO pins configured as INPUT_PULLUP
- Active LOW logic (button pressed = LOW)
- No diodes needed
```

## Rotary Encoders

### Basic Configuration
```
Encoder pinout:
A --- Pin A (CLK)
B --- Pin B (DT)
C --- Common/GND
SW -- Button (optional)

Notes:
- All pins configured as INPUT_PULLUP
- No external resistors needed if using internal pull-ups
- Optional capacitors for additional debouncing
- Supports up to 4x resolution
```

### Connection Diagram
```
Arduino         Encoder
-------         -------
Pin A   <----- CLK (A)  ----[100nF]---- GND  (Optional debounce)
Pin B   <----- DT  (B)  ----[100nF]---- GND  (Optional debounce)
Button  <----- SW  (opt) ---[100nF]---- GND  (Optional debounce)
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

Notes:
- Keep wires short and equal length for each encoder
- Consider adding shield connection to GND
- Capacitors recommended for long wires
```

## MCP23017 I2C Expander

### Basic Configuration
```
Arduino         MCP23017
-------         --------
SDA     <----> SDA      <--[4.7kΩ]--> VCC
SCL     <----> SCL      <--[4.7kΩ]--> VCC
INT     <----- INT      (optional)
VCC     <----> VDD      ---[100nF]--> GND
GND     <----> VSS
         ----> A0,A1,A2 (Address select)
         ----> RESET    --> VCC
```

### Address Configuration
```
A2  A1  A0  | Address
--------------------------
GND GND GND | 0x20 (default)
GND GND VCC | 0x21
GND VCC GND | 0x22
GND VCC VCC | 0x23
VCC GND GND | 0x24
VCC GND VCC | 0x25
VCC VCC GND | 0x26
VCC VCC VCC | 0x27
```

### Multiple MCP23017 Configuration
```
Arduino         MCP23017 #1     MCP23017 #2
-------         -----------     -----------
SDA     <----> SDA      <----> SDA
SCL     <----> SCL      <----> SCL
INT1    <----- INTA
INT2    <----------------- INTA
GND     <----> VSS      <----> VSS
VCC     <----> VDD      <----> VDD
         ----> A0 (GND)  ----> A0 (VCC)  # Different addresses
         ----> A1 (GND)  ----> A1 (GND)
         ----> A2 (GND)  ----> A2 (GND)

Notes:
- Each MCP23017 needs unique address
- Keep I2C lines short
- Use 4.7kΩ pullups on SDA/SCL
- Add bypass capacitors near each IC
```

### Button Connection to MCP23017
```
MCP23017        Buttons
--------        -------
GPA0    <----- [BTN] ----- GND
GPA1    <----- [BTN] ----- GND
...
GPB7    <----- [BTN] ----- GND

Notes:
- All pins configured as inputs
- Internal pullups can be enabled
- No diodes needed
- Active LOW logic
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
- Shielded cable for long runs

### MCP23017 Components
- MCP23017 IC
- Pull-up resistors (4.7kΩ) for I2C bus
- Bypass capacitors (100nF) near each IC
- Optional interrupt pull-up resistors (10kΩ)
- Optional address selection jumpers

## Installation Tips

### General
- Keep wires short and organized
- Use connectors for removable parts
- Add bypass capacitors near ICs
- Consider strain relief for cables
- Use proper power distribution

### Matrix
- Install diodes in correct orientation (cathode to column)
- Check for proper row/column isolation
- Test each intersection individually
- Consider pull-downs for outputs (optional)
- Keep rows and columns perpendicular when possible

### Encoders
- Mount securely to prevent mechanical stress
- Consider mechanical alignment
- Test rotation before final assembly
- Add debounce capacitors if needed
- Use shielded cables for long runs
- Keep A/B wires same length

### MCP23017
- Keep I2C lines short and equal length
- Use twisted pair for I2C lines
- Add pull-up resistors to I2C bus
- Add bypass capacitors near each IC
- Consider interrupt line routing
- Use address selection jumpers
- Add power filtering if needed

## Power Considerations
- Use external power for many components
- Add bypass capacitors near ICs
- Calculate total current requirements
- Consider voltage drops in long runs
- Use appropriate power distribution
- Consider separate power for noisy components
- Add bulk capacitors for power stability

## Common Issues and Solutions

### Matrix Ghosting
- Verify diode orientation
- Check for missing diodes
- Test for shorts between rows/columns
- Verify pull-up resistors
- Check for damaged switches

### Encoder Issues
- Check pin connections
- Verify common ground
- Add debounce capacitors
- Check encoder quality
- Verify mechanical mounting
- Test with different divisor settings
- Monitor error count
- Check wire lengths

### MCP23017 Issues
- Verify I2C address configuration
- Check pull-up resistors
- Measure I2C voltage levels
- Test each device individually
- Check interrupt configuration
- Verify power supply stability
- Monitor communication timeouts
- Check for address conflicts
- Verify bypass capacitors

### Power Issues
- Check voltage levels at all points
- Monitor power consumption
- Add power filtering
- Use separate grounds appropriately
- Check for ground loops
- Consider power sequencing
- Monitor voltage under load
