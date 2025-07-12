# JuncTek_Batterymonitor

Arduino library for JuncTek Battery Monitor devices with checksum verification for reliable communication in noisy environments.

## Features

- **Checksum Verification**: Automatically verifies message integrity to prevent erroneous readings
- **Cache State Management**: Tracks data validity (valid/stale/invalid) for robust error handling  
- **Multi-device Support**: Handle up to 4 battery monitor devices
- **Configurable Cache Time**: Optimize communication frequency vs. data freshness
- **Comprehensive Data Access**: Read voltage, current, temperature, capacity, and protection settings

## New in Latest Version

### Checksum Verification
The library now verifies checksums on all incoming messages to ensure data integrity in noisy environments. Invalid messages are rejected and the library preserves the last known good values.

### Cache State Tracking
Monitor data validity with three states:
- `valid`: Fresh, verified data
- `stale`: Valid but expired cache  
- `invalid`: Checksum verification failed

```cpp
cacheState_t state = monitor.getCacheState();
if (state != invalid) {
    float voltage = monitor.getVoltage(); // Safe to use
}
```

See [CHECKSUM_VERIFICATION.md](CHECKSUM_VERIFICATION.md) for detailed documentation.

## Basic Usage

```cpp
#include "JuncTek_BatteryMonitor.h"

BatteryMonitor monitor;

void setup() {
    Serial.begin(9600);
    Serial2.begin(115200);
    monitor.begin(1, Serial2); // Address 1, use Serial2
}

void loop() {
    // Check data validity
    if (monitor.getCacheState() != invalid) {
        float voltage = monitor.getVoltage();
        float current = monitor.getCurrent();
        int temperature = monitor.getTemperature();
        
        Serial.print("Voltage: "); Serial.println(voltage);
        Serial.print("Current: "); Serial.println(current);
        Serial.print("Temperature: "); Serial.println(temperature);
    }
    delay(1000);
}
```

## Installation

1. Download or clone this repository
2. Place in your Arduino libraries folder
3. Include in your sketch: `#include "JuncTek_BatteryMonitor.h"`

## Compatibility

- Backward compatible with existing code
- Arduino IDE and PlatformIO supported
- Works with ESP32, ESP8266, Arduino Uno, and other Arduino-compatible boards
