/**
 * JuncTek Battery Monitor Example with Checksum Verification
 * 
 * This example demonstrates how to use the JuncTek Battery Monitor library
 * with the new checksum verification feature that prevents erroneous readings
 * in noisy environments.
 */

#include "JuncTek_BatteryMonitor.h"

BatteryMonitor monitor;

void setup() {
  Serial.begin(9600);
  Serial2.begin(115200); // Adjust baud rate as needed for your device
  
  // Initialize the battery monitor on address 1
  monitor.begin(1, Serial2);
  
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