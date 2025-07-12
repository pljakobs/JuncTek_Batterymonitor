/*
 * JuncTek Battery Monitor Example
 * 
 * This example demonstrates basic usage of the JuncTek Battery Monitor library.
 * It reads battery parameters and configures protection settings.
 * 
 * Hardware setup:
 * - Connect the JuncTek KL-F series battery monitor to your microcontroller
 * - Use appropriate serial pins (adjust in the code below)
 * 
 * For ESP32: Use Serial2 (pins 16, 17 by default)
 * For ESP8266: Use SoftwareSerial
 * For Arduino: Use SoftwareSerial
 */

#include "JuncTek_BatteryMonitor.h"

// For non-ESP32 boards, include SoftwareSerial
#ifndef ESP32
  #include <SoftwareSerial.h>
#endif

// Create battery monitor instance
BatteryMonitor monitor;

// Create SoftwareSerial instance for non-ESP32 boards at global scope
#ifndef ESP32
SoftwareSerial batterySerial(2, 3); // RX, TX pins - adjust as needed
#endif

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("JuncTek Battery Monitor Example");
  Serial.println("================================");
  
  // Initialize the battery monitor
  // Address 1 is the default address for most JuncTek monitors
  #ifdef ESP32
    // ESP32 has multiple hardware serial ports
    Serial2.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
    monitor.begin(1, Serial2);
  #else
    // Use SoftwareSerial for other boards
    batterySerial.begin(115200);
    monitor.begin(1, batterySerial);
  #endif
  
  Serial.println("Battery monitor initialized");
  
  // Configure protection settings (optional)
  Serial.println("Configuring protection settings...");
  
  // Set over-voltage protection to 14.4V
  if (monitor.setOverVoltageProtection(14.4f)) {
    Serial.println("✓ Over-voltage protection set to 14.4V");
  } else {
    Serial.println("✗ Failed to set over-voltage protection");
  }
  
  // Set under-voltage protection to 10.0V
  if (monitor.setUnderVoltageProtection(10.0f)) {
    Serial.println("✓ Under-voltage protection set to 10.0V");
  } else {
    Serial.println("✗ Failed to set under-voltage protection");
  }
  
  // Set battery capacity to 100Ah
  if (monitor.setBatteryCapacity(100.0f)) {
    Serial.println("✓ Battery capacity set to 100Ah");
  } else {
    Serial.println("✗ Failed to set battery capacity");
  }
  
  Serial.println("Setup complete!");
  Serial.println();
}

void loop() {
  Serial.println("=== Battery Status ===");
  
  // Read basic measurements
  float voltage = monitor.getVoltage();
  float current = monitor.getCurrent();
  int temperature = monitor.getTemperature();
  
  Serial.print("Voltage: ");
  Serial.print(voltage, 2);
  Serial.println(" V");
  
  Serial.print("Current: ");
  Serial.print(current, 2);
  Serial.println(" A");
  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  // Read capacity information
  float remainingCapacity = monitor.getRemainingCapacity();
  float cumulativeCapacity = monitor.getCumulativeCapacity();
  
  Serial.print("Remaining Capacity: ");
  Serial.print(remainingCapacity, 3);
  Serial.println(" Ah");
  
  Serial.print("Cumulative Capacity: ");
  Serial.print(cumulativeCapacity, 3);
  Serial.println(" Ah");
  
  // Read additional information
  int uptime = monitor.getUptime();
  int batteryLifeLeft = monitor.getBatteryLifeLeft();
  float internalResistance = monitor.getInternalResistance();
  
  Serial.print("Uptime: ");
  Serial.print(uptime);
  Serial.println(" seconds");
  
  Serial.print("Battery Life Left: ");
  Serial.print(batteryLifeLeft);
  Serial.println(" minutes");
  
  Serial.print("Internal Resistance: ");
  Serial.print(internalResistance, 3);
  Serial.println(" Ohm");
  
  // Read protection settings
  Serial.println("\n--- Protection Settings ---");
  Serial.print("OVP Voltage: ");
  Serial.print(monitor.getOverVoltageProtectionVoltage(), 1);
  Serial.println(" V");
  
  Serial.print("UVP Voltage: ");
  Serial.print(monitor.getUnderVoltageProtectionVoltage(), 1);
  Serial.println(" V");
  
  Serial.print("OCP Forward: ");
  Serial.print(monitor.getOverCurrentProtectionForwardCurrent(), 1);
  Serial.println(" A");
  
  Serial.print("OCP Reverse: ");
  Serial.print(monitor.getOverCurrentProtectionReverseCurrent(), 1);
  Serial.println(" A");
  
  Serial.println("\n" + String('-', 40));
  
  // Wait 5 seconds before next reading
  delay(5000);
}
