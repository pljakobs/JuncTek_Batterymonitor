# Checksum Verification Feature

This document describes the checksum verification feature added to the JuncTek Battery Monitor library to ensure data integrity in noisy environments.

## Overview

The library now verifies checksums on all incoming data messages before updating the internal data structures. This prevents erroneous readings caused by communication errors or electrical noise.

## New Features

### Cache State Management

The library now tracks the state of cached data using the `cacheState_t` enum:

- `valid`: Data is valid and fresh (recently read with successful checksum verification)
- `stale`: Data is valid but the cache time has elapsed without a successful re-read
- `invalid`: Data is invalid due to checksum verification failure or no successful reads

### New Functions

#### `getCacheState()`
Returns the current cache state as a `cacheState_t` value.

```cpp
cacheState_t state = monitor.getCacheState();
switch(state) {
    case valid:
        // Data is fresh and verified
        break;
    case stale:
        // Data is valid but old
        break;
    case invalid:
        // Data cannot be trusted
        break;
}
```

### Behavior Changes

1. **Checksum Verification**: All received messages are now verified before updating data structures
2. **Selective Updates**: Data is only updated when checksum verification succeeds
3. **Cache State Tracking**: The cache state is updated based on read success/failure
4. **Last Good Values**: When checksums fail, the library preserves the last known good values

### Error Handling

When a checksum verification fails:
- The data structures are NOT updated
- The cache state is set to `invalid`
- The cache timestamp is NOT updated
- Subsequent calls to getter functions will attempt to re-read the data

### Compatibility

This update is fully backward compatible. Existing code will continue to work without modifications, but will now benefit from checksum verification.

## Usage Example

```cpp
#include "JuncTek_BatteryMonitor.h"

BatteryMonitor monitor;

void setup() {
    monitor.begin(1, Serial2);
}

void loop() {
    // Check data validity before using
    if (monitor.getCacheState() != invalid) {
        float voltage = monitor.getVoltage();
        // Use voltage reading safely
    } else {
        // Handle invalid data case
        Serial.println("Warning: Invalid data");
    }
    delay(1000);
}
```

## Technical Details

### Checksum Algorithm

The library uses the same checksum algorithm for verification as is used for transmission:
```cpp
#define checksum(a) ((a%255)+1)
```

### Message Format

Messages follow this format:
```
:r<cmd>=<addr>,<checksum>,<data1>,<data2>,...<dataN>,
```

The checksum is calculated by summing all data fields and applying the checksum formula.

### Verification Process

1. Parse the received checksum from field 2
2. Sum all data fields (field 3 onwards)
3. Apply the checksum algorithm to the sum
4. Compare with the received checksum
5. Update data only if checksums match