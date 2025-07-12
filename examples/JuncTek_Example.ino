/**
 * JuncTek Battery Monitor Example with Checksum Verification
 * 
 * This example demonstrates how to use the JuncTek Battery Monitor library
 * with the new checksum verification feature that prevents erroneous readings
 * in noisy environments.
 */

#include "JuncTek_BatteryMonitor.h"
#if !defined(ESP32)
  #include <SoftwareSerial.h>
#endif

BatteryMonitor monitor;

// For boards without hardware serial, we'll use SoftwareSerial
#if !defined(ESP32)
  SoftwareSerial batterySerial(2, 3); // RX, TX pins - adjust as needed
#endif

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // Initialize the battery monitor on address 1
  #if defined(ESP32)
    // ESP32 has multiple hardware serial ports
    Serial2.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
    monitor.begin(1, Serial2);
  #else
    // Use SoftwareSerial for other boards
    batterySerial.begin(9600); // Lower baud rate for SoftwareSerial reliability
    monitor.begin(1, batterySerial);
  #endif
  
  Serial.println("JuncTek Battery Monitor initialized");
  Serial.println("Checksum verification enabled");
}

void loop() {
  // Get current cache state
  cacheState_t state = monitor.getCacheState();
  
  // Print cache state information
  Serial.print("Cache State: ");
  switch(state) {
    case valid:
      Serial.println("VALID - Data is fresh and verified");
      break;
    case stale:
      Serial.println("STALE - Data is valid but cache has expired");
      break;
    case invalid:
      Serial.println("INVALID - Checksum verification failed or no data");
      break;
  }
  
  // Only proceed with reading if data is valid or stale
  if (state != invalid) {
    // Read measured values
    float voltage = monitor.getVoltage();
    float current = monitor.getCurrent();
    int temperature = monitor.getTemperature();
    float capacity = monitor.getRemainingCapacity();
    
    // Display the readings
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println(" V");
    
    Serial.print("Current: ");
    Serial.print(current);
    Serial.println(" A");
    
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    
    Serial.print("Remaining Capacity: ");
    Serial.print(capacity);
    Serial.println(" Ah");
    
    // Get cache state again to see if it changed during reading
    cacheState_t newState = monitor.getCacheState();
    if (newState != state) {
      Serial.print("Cache state changed to: ");
      switch(newState) {
        case valid: Serial.println("VALID"); break;
        case stale: Serial.println("STALE"); break;
        case invalid: Serial.println("INVALID"); break;
      }
    }
  } else {
    Serial.println("WARNING: Data invalid due to checksum failure");
    Serial.println("Using last known good values or waiting for valid data...");
  }
  
  Serial.println("---");
  delay(2000); // Wait 2 seconds before next reading
}