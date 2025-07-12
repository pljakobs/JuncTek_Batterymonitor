# JuncTek KL-F Battery Monitor Communication Protocol

## Overview

The JuncTek KL-F Battery Monitor uses a command-line based communication protocol over serial connection at **115200 baud rate**. The PC sends commands to the device, which parses, executes, and returns results.

## Protocol Format

### Command Structure
```
:W,R00~99=1-99,1-255,Data field,<CR><LF>
```

| Component | Description | Values |
|-----------|-------------|---------|
| **Start bit** | `:` | Always `:` |
| **Function code** | Command type | `W` (write), `R` (read) |
| **Function No.** | Command identifier | `00-99` |
| **Connector** | `=` | Always `=` |
| **Address** | Device address | `1-99` (0 = broadcast) |
| **Spacer** | `,` | Always `,` |
| **Checksum** | Validation | `1-255` (see calculation below) |
| **Spacer** | `,` | Always `,` |
| **Data field** | Command parameters | Varies by command |
| **Spacer** | `,` | Always `,` |
| **Terminator** | End marker | `<CR><LF>` (0x0D 0x0A) |

### Checksum Calculation
The checksum is calculated as: `(sum of all ASCII values after checksum field) % 255 + 1`
If checksum = 0, verification is disabled.

## Write Commands (W)

### Device Configuration

| Function | Command | Example | Description |
|----------|---------|---------|-------------|
| **W01** | Set address | `:W01=1,3,2,<CR><LF>` | Set communication address to 2 |
| **W10** | Output control | `:W10=1,2,1,<CR><LF>` | Turn on output |
| | | `:W10=1,0,0,<CR><LF>` | Turn off output |

### Protection Settings

| Function | Command | Example | Description |
|----------|---------|---------|-------------|
| **W20** | Over-voltage protection | `:W20=1,216,2000,<CR><LF>` | Set OVP to 20.00V |
| **W21** | Under-voltage protection | `:W21=1,216,2000,<CR><LF>` | Set UVP to 20.00V |
| **W22** | Over-current protection | `:W22=1,216,2000,<CR><LF>` | Set OCP to 20A |
| **W23** | Negative over-current protection | `:W23=1,216,2000,<CR><LF>` | Set negative OCP to -20A |
| **W24** | Over-power protection | `:W24=1,216,2000,<CR><LF>` | Set OPP to 20W |
| **W25** | Over-temperature protection | `:W25=1,151,150,<CR><LF>` | Set OTP to 50℃ |
| | | `:W25=1,211,210,<CR><LF>` | Set OTP to 110℃ |

### Battery Configuration

| Function | Command | Example | Description |
|----------|---------|---------|-------------|
| **W28** | Battery capacity | `:W28=1,216,2000,<CR><LF>` | Set capacity to 200.0Ah |

### Calibration Settings

| Function | Command | Example | Description |
|----------|---------|---------|-------------|
| **W29** | Voltage calibration | `:W29=1,121,120,<CR><LF>` | Increase voltage by 20 units |
| | | `:W29=1,81,80,<CR><LF>` | Decrease voltage by 20 units |
| **W30** | Current calibration | `:W30=1,121,120,<CR><LF>` | Increase current by 20 units |
| | | `:W30=1,81,80,<CR><LF>` | Decrease current by 20 units |
| **W31** | Temperature calibration | `:W31=1,104,103,<CR><LF>` | Increase temperature by 3℃ |
| | | `:W31=1,99,98,<CR><LF>` | Decrease temperature by 2℃ |

### System Settings

| Function | Command | Example | Description |
|----------|---------|---------|-------------|
| **W34** | Relay type | `:W34=1,2,1,<CR><LF>` | Set to normally closed |
| | | `:W34=1,0,0,<CR><LF>` | Set to normally open |
| **W35** | Factory reset | `:W35=1,2,1,<CR><LF>` | Restore factory settings |
| **W36** | Current multiplier | `:W36=1,4,3,<CR><LF>` | Set current multiple to 3 |

### Battery Management

| Function | Command | Example | Description |
|----------|---------|---------|-------------|
| **W60** | Battery percentage | `:W60=1,51,50,<CR><LF>` | Set remaining capacity to 50% |
| **W61** | Zero current | `:W61=1,2,1,<CR><LF>` | Perform current zeroing |
| **W62** | Clear data | `:W62=1,2,1,<CR><LF>` | Clear accumulated data |

## Read Commands (R)

### R00 - Basic Device Information

**Command:** `:R00=1,2,1,<CR><LF>`

**Response:** `:r00=1,47,1120,100,101,<CR><LF>`

| Field | Position | Description | Example |
|-------|----------|-------------|---------|
| Address | 1 | Communication address | `1` |
| Checksum | 2 | Response checksum | `47` |
| Sensor Info | 3 | Format: `[sensor][voltage][current]` | `1120` |
| | | - Sensor type (1=Hall, 2=Sampler) | `1` (Hall sensor) |
| | | - Max voltage (×100V) | `1` (100V) |
| | | - Max current (×10A) | `20` (200A) |
| Version | 4 | Firmware version | `100` (v1.00) |
| Serial | 5 | Machine serial number | `101` |

### R50 - Live Measured Values

**Command:** `:R50=2,2,1,<CR><LF>`

**Response:** `:r50=2,215,2056,200,5408,4592,9437,14353,134,0,0,0,162,30682,<CR><LF>`

| Field | Position | Description | Example | Units |
|-------|----------|-------------|---------|-------|
| Address | 1 | Communication address | `2` | - |
| Checksum | 2 | Response checksum | `215` | - |
| Voltage | 3 | Battery voltage | `2056` | 20.56V (÷100) |
| Current | 4 | Battery current | `200` | 2.00A (÷100) |
| Remaining Capacity | 5 | Remaining battery capacity | `5408` | 5.408Ah (÷1000) |
| Cumulative Capacity | 6 | Total used capacity | `4592` | 4.592Ah (÷1000) |
| Energy | 7 | Energy consumption | `9437` | 0.09437kWh (÷100000) |
| Runtime | 8 | Operating time | `14353` | 14353 seconds |
| Temperature | 9 | Ambient temperature | `134` | 34℃ (-100) |
| Reserved | 10 | Future use | `0` | - |
| Output Status | 11 | Output state | `0` | See status codes below |
| Current Direction | 12 | Current flow direction | `0` | 0=forward, 1=reverse |
| Battery Life | 13 | Remaining battery life | `162` | 162 minutes |
| Internal Resistance | 14 | Battery internal resistance | `30682` | 306.82mΩ (÷100) |

#### Output Status Codes
| Code | Status | Description |
|------|--------|-------------|
| 0 | ON | Normal operation |
| 1 | OVP | Over-voltage protection |
| 2 | OCP | Over-current protection |
| 3 | LVP | Low-voltage protection |
| 4 | NCP | Negative current protection |
| 5 | OPP | Over-power protection |
| 6 | OTP | Over-temperature protection |
| 255 | OFF | Output disabled |

### R51 - Configuration Settings

**Command:** `:R51=1,2,1,<CR><LF>`

**Response:** `:r51=1,211,3000,100,2000,2000,10000,151,10,7,200,120,90,101,0,0,2,12,13,<CR><LF>`

| Field | Position | Description | Example | Units |
|-------|----------|-------------|---------|-------|
| Address | 1 | Communication address | `1` | - |
| Checksum | 2 | Response checksum | `211` | - |
| OVP Voltage | 3 | Over-voltage protection | `3000` | 30.00V (÷100) |
| UVP Voltage | 4 | Under-voltage protection | `100` | 1.00V (÷100) |
| Forward OCP | 5 | Forward over-current protection | `2000` | 20.00A (÷100) |
| Reverse OCP | 6 | Reverse over-current protection | `2000` | -20.00A (÷100) |
| OPP Power | 7 | Over-power protection | `10000` | 100.00W (÷100) |
| OTP Temperature | 8 | Over-temperature protection | `151` | 51℃ (-100) |
| Recovery Time | 9 | Protection recovery time | `10` | 10 seconds |
| Delay Time | 10 | Protection delay time | `7` | 7 seconds |
| Battery Capacity | 11 | Preset battery capacity | `200` | 20.0Ah (÷10) |
| Voltage Calibration | 12 | Voltage fine-tuning factor | `120` | +20 (100=no adjustment) |
| Current Calibration | 13 | Current fine-tuning factor | `90` | -10 (100=no adjustment) |
| Temp Calibration | 14 | Temperature calibration | `101` | +1℃ (100=no adjustment) |
| Reserved | 15 | Future use | `0` | - |
| Relay Type | 16 | Relay configuration | `0` | 0=normally open, 1=normally closed |
| Current Multiple | 17 | Current multiplier | `2` | 2× |
| Voltage Scale | 18 | Voltage curve scale | `12` | 12V/div |
| Current Scale | 19 | Current curve scale | `13` | 13A/div |

## Data Format Notes

### Scaling Factors
- **Voltage:** Divide by 100 (e.g., 2056 = 20.56V)
- **Current:** Divide by 100 (e.g., 200 = 2.00A)
- **Power:** Divide by 100 (e.g., 10000 = 100.00W)
- **Capacity (Ah):** Divide by 1000 (e.g., 5408 = 5.408Ah)
- **Capacity (Battery):** Divide by 10 (e.g., 200 = 20.0Ah)
- **Energy:** Divide by 100000 (e.g., 9437 = 0.09437kWh)
- **Temperature:** Subtract 100 (e.g., 134 = 34℃)
- **Resistance:** Divide by 100 (e.g., 30682 = 306.82mΩ)

### Address Range
- **1-99:** Individual device addresses
- **0:** Broadcast address (affects all devices)

### Communication Settings
- **Baud Rate:** 115200
- **Data Bits:** 8
- **Stop Bits:** 1
- **Parity:** None
- **Flow Control:** None

## Error Handling

The device will respond with error codes in the checksum field if commands fail. Always verify the checksum and response format to ensure successful communication.

## Example Communication Session

```
PC → Device: :R00=1,2,1,<CR><LF>
Device → PC: :r00=1,47,1120,100,101,<CR><LF>

PC → Device: :R50=1,2,1,<CR><LF>
Device → PC: :r50=1,215,2056,200,5408,4592,9437,14353,134,0,0,0,162,30682,<CR><LF>

PC → Device: :W20=1,216,2000,<CR><LF>
Device → PC: :w20=1,73,OK,<CR><LF>
```

This protocol enables complete control and monitoring of the JuncTek KL-F Battery Monitor through programmatic interfaces.
