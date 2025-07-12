# JuncTek Battery Monitor - Mock Testing Framework

## Overview

This directory contains a comprehensive mock testing framework for the JuncTek Battery Monitor library. The tests verify that the library correctly implements the official JuncTek KL-F communication protocol.

## Protocol Specification

Based on the official JuncTek KL-F manual, the communication protocol uses:

- **Baud Rate**: 115200
- **Format**: `:W,R00~99=1-99,Checksum,Data field,<CR><LF>`
- **Commands**: Write (W) and Read (R) operations
- **Checksum**: Sum of ASCII values mod 255 + 1

### Key Commands

| Command | Function | Description |
|---------|----------|-------------|
| R00 | Read basic info | Device type, version, serial number |
| R50 | Read measured values | Voltage, current, capacity, temperature |
| R51 | Read settings | Protection values, calibration settings |
| W20 | Set over-voltage protection | Format: `:W20=addr,checksum,value,` |
| W21 | Set under-voltage protection | Format: `:W21=addr,checksum,value,` |
| W28 | Set battery capacity | Format: `:W28=addr,checksum,value,` |

### Data Formats

- **Voltage**: Value × 0.01V (e.g., 2056 = 20.56V)
- **Current**: Value × 0.01A (e.g., 200 = 2.00A)
- **Capacity**: Value × 0.001Ah (e.g., 5408 = 5.408Ah)
- **Temperature**: Value - 100°C (e.g., 134 = 34°C)

## Mock Testing Architecture

### MockJuncTekStream Class

The `MockJuncTekStream` class simulates a real JuncTek device by:

1. **Parsing Commands**: Correctly interprets protocol format
2. **Generating Responses**: Creates realistic device responses
3. **State Management**: Maintains internal device state
4. **Error Simulation**: Can inject protocol errors for testing

### Test Coverage

The test suite covers:

- ✅ **Protocol Parsing**: Command format validation
- ✅ **Data Conversion**: Voltage/current/temperature scaling
- ✅ **Checksum Verification**: Protocol integrity checks
- ✅ **Write Commands**: Parameter setting verification
- ✅ **Cache Behavior**: Response caching functionality
- ✅ **Error Handling**: Timeout and garbled data scenarios
- ✅ **Multi-Address**: Device addressing support

## Running Tests

### Prerequisites

```bash
# Install PlatformIO
pip install platformio

# Install Unity testing framework (done automatically)
```

### Execute Tests

```bash
# Run on native platform (fastest)
pio test -e native_test

# Run on ESP32 (embedded testing)
pio test -e esp32_test

# Run on Arduino Mega (AVR testing)
pio test -e arduino_test

# Run specific test
pio test -e native_test -f test_protocol_parsing
```

### Test Output

```
Testing JuncTek Protocol Parsing
================================
✓ test_read_voltage                (0.02s)
✓ test_read_current               (0.01s)
✓ test_read_temperature           (0.01s)
✓ test_write_ovp_setting          (0.03s)
✓ test_checksum_verification      (0.02s)
✓ test_cache_behavior             (0.15s)

6 Tests 0 Failures 0 Ignored
```

## Usage Examples

### Basic Mock Testing

```cpp
#include "MockJuncTekStream.h"
#include "JuncTek_BatteryMonitor.h"

MockJuncTekStream mockSerial(true); // Enable debug
BatteryMonitor monitor;

void setup() {
    monitor.begin(1, mockSerial);
    
    // Set known test values
    mockSerial.setVoltage(12.45);
    mockSerial.setCurrent(2.30);
    mockSerial.setTemperature(25);
    
    // Test library functions
    float voltage = monitor.getVoltage();    // Should return 12.45V
    float current = monitor.getCurrent();    // Should return 2.30A
    int temp = monitor.getTemperature();     // Should return 25°C
}
```

### Error Scenario Testing

```cpp
// Test checksum error handling
mockSerial.simulateChecksumError(true);
float voltage = monitor.getVoltage(); // Should handle error gracefully

// Test timeout scenario
mockSerial.simulateTimeout(true);
voltage = monitor.getVoltage(); // Should return cached or error value

// Test garbled data
mockSerial.simulateGarbledData(true);
voltage = monitor.getVoltage(); // Should parse or reject invalid data
```

### Protocol Validation

```cpp
// Verify correct command format
monitor.setOverVoltageProtection(14.4);
String lastCmd = mockSerial.getLastCommand();
// Should be: ":W20=1,checksum,1440,"

// Verify response parsing
mockSerial.injectResponse(":r50=1,215,2056,200,5408,4592,9437,14353,134,0,0,0,162,30682,\r\n");
float voltage = monitor.getVoltage(); // Should parse and return 20.56V
```

## Benefits

1. **Hardware-Independent**: Tests run without physical device
2. **Deterministic**: Consistent, repeatable test results
3. **Comprehensive**: Tests all protocol aspects systematically
4. **Fast Feedback**: Rapid development and debugging cycles
5. **Error Coverage**: Tests edge cases and error conditions
6. **CI/CD Ready**: Automated testing in build pipelines

## Integration with CI

The mock tests integrate seamlessly with GitHub Actions:

```yaml
- name: Run Protocol Tests
  run: |
    cd test_project
    pio test -e native_test -v
```

This ensures every commit validates the protocol implementation against the official specification!
