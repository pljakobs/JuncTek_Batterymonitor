# JuncTek Battery Monitor Examples

This directory contains example sketches demonstrating how to use the JuncTek Battery Monitor library.

## BasicUsage

The `BasicUsage` example demonstrates:
- How to initialize the battery monitor
- Reading basic measurements (voltage, current, temperature)
- Reading capacity information
- Configuring protection settings
- Reading protection settings

### Hardware Setup

#### ESP32
```cpp
// Use Serial2 (Hardware Serial)
Serial2.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
monitor.begin(1, Serial2);
```

#### ESP8266
```cpp
// Use SoftwareSerial
#include <SoftwareSerial.h>
SoftwareSerial batterySerial(D2, D3); // RX, TX
batterySerial.begin(115200);
monitor.begin(1, batterySerial);
```

#### Arduino Uno/Mega
```cpp
// Use SoftwareSerial
#include <SoftwareSerial.h>
SoftwareSerial batterySerial(2, 3); // RX, TX
batterySerial.begin(115200);
monitor.begin(1, batterySerial);
```

### Wiring

Connect your JuncTek KL-F series battery monitor to your microcontroller:

| JuncTek Monitor | ESP32 | ESP8266 | Arduino |
|----------------|-------|---------|---------|
| TX | GPIO16 (RX2) | D2 | Pin 2 |
| RX | GPIO17 (TX2) | D3 | Pin 3 |
| GND | GND | GND | GND |
| VCC | 3.3V or 5V | 3.3V | 5V |

**Note:** Check your specific JuncTek model's documentation for the exact pinout and voltage requirements.

### Expected Output

The example will output battery status information every 5 seconds:

```
=== Battery Status ===
Voltage: 12.45 V
Current: 2.30 A
Temperature: 25 °C
Remaining Capacity: 45.678 Ah
Cumulative Capacity: 123.456 Ah
Uptime: 3600 seconds
Battery Life Left: 1200 minutes
Internal Resistance: 0.015 Ohm

--- Protection Settings ---
OVP Voltage: 14.4 V
UVP Voltage: 10.0 V
OCP Forward: 10.0 A
OCP Reverse: 10.0 A
```

## Troubleshooting

1. **No data received**: Check wiring and baud rate settings
2. **Compilation errors**: Ensure you have the correct board selected and libraries installed
3. **Communication timeout**: Verify the device address (default is usually 1)
4. **Wrong readings**: Check protocol documentation in `KL-F_EN_manual.pdf`
