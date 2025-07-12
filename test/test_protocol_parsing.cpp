#include <unity.h>
#include "MockJuncTekStream.h"
#include "JuncTek_BatteryMonitor.h"

MockJuncTekStream* mockStream;
BatteryMonitor* monitor;

// Helper function for debug output
void debug(const char* msg) {
#ifdef NATIVE_BUILD
    printf("[DEBUG] %s\n", msg);
#endif
}

void debug(String msg) {
#ifdef NATIVE_BUILD
    printf("[DEBUG] %s\n", msg.c_str());
#endif
}

void setUp(void) {
    // Use new constructor: address=1, version=100, hallSensor=1, debug=true
    mockStream = new MockJuncTekStream(1, 100, 1, true);
    monitor = new BatteryMonitor();
    debug("=== Test Setup: Initializing BatteryMonitor ===");
    monitor->begin(1, *mockStream);
    debug("=== Setup Complete ===");
}

void tearDown(void) {
    delete monitor;
    delete mockStream;
}

// Test basic device information reading (R00 command)
void test_read_basic_info(void) {
    debug("[basic info]: Testing R00 command parsing and response generation");
#ifdef NATIVE_BUILD
    // Test R00 command parsing and response generation
    String testCommand = ":R00=1,123,";
    debug("[basic info]: Sending test command: " + testCommand);
    
    // Send command to mock stream
    for (int i = 0; i < testCommand.length(); i++) {
        mockStream->write((uint8_t)testCommand.charAt(i));
    }
    mockStream->write('\r');
    mockStream->write('\n');
    
    // Check that response is available
    debug("[basic info]: Checking for response availability");
    TEST_ASSERT_GREATER_THAN(0, mockStream->available());
    
    // Read response
    String response = "";
    while (mockStream->available()) {
        char c = mockStream->read();
        if (c != -1) response += c;
    }
    
    debug("[basic info]: Received response: " + response);
    
    // Verify response format: :r00=1,checksum,data...
    TEST_ASSERT_TRUE(response.length() > 0);
    TEST_ASSERT_EQUAL(':',  response.charAt(0));
    TEST_ASSERT_EQUAL('r',  response.charAt(1));
    TEST_ASSERT_EQUAL('0',  response.charAt(2));
    TEST_ASSERT_EQUAL('0',  response.charAt(3));
    TEST_ASSERT_EQUAL('=',  response.charAt(4));
    
    debug("[basic info]: Response format validation passed");
#else
    // Test with actual BatteryMonitor library
    // The library should parse R00 response correctly
    debug("[basic info]: Arduino build - using placeholder test");
    TEST_ASSERT_TRUE(true); // Placeholder for Arduino tests
#endif
}

// Test mock response generation
void test_mock_response_generation(void) {
    debug("[mock response]: Testing mock stream response generation");
    String lastCmd = mockStream->getLastCommand();
    debug("[mock response]: Initial last command: '" + lastCmd + "'");
    TEST_ASSERT_TRUE(lastCmd.length() == 0); // No command sent yet
    
    // Manually inject R00 command to test mock response
    debug("[mock response]: Injecting R00 command to test mock response");
    mockStream->write((const uint8_t*)":R00=1,2,1,\r\n", 13);
    
    // Check that mock generated correct response
    debug("[mock response]: Checking if mock generated response");
    TEST_ASSERT_TRUE(mockStream->available() > 0);
    
    String response = "";
    while (mockStream->available()) {
        response += (char)mockStream->read();
    }
    
    debug("[mock response]: Generated response: " + response);
    
    // Should contain device info
    TEST_ASSERT_TRUE(response.indexOf(":r00=1,") == 0);
    TEST_ASSERT_TRUE(response.indexOf("100,20,100,101") > 0);
    debug("[mock response]: Response content validation passed");
}

// Test voltage reading with known mock data
void test_read_voltage(void) {
    debug("[voltage]: Testing voltage reading with known mock data");
    // Set known voltage in mock (20.56V)
    debug("[voltage]: Setting mock voltage to 20.56V");
    mockStream->setVoltage(20.56);
    
    // Invalidate cache to force fresh read
    debug("[voltage]: Invalidating cache to force fresh read");
    monitor->invalidateCache();
    
    // Check cache status before reading
    int cacheTime = monitor->getCacheTime();
    debug("[voltage]: Cache time setting: " + String(cacheTime) + "ms");
    
    // Test the actual library function
    debug("[voltage]: Querying library for voltage");
    float voltage = monitor->getVoltage();
    
    debug("[voltage]: Library returned: " + String(voltage) + "V");
    // Should read 20.56V from mock
    TEST_ASSERT_FLOAT_WITHIN(0.01, 20.56, voltage);
    
    // Verify that correct command was sent
    String lastCmd = mockStream->getLastCommand();
    debug("[voltage]: Last command sent: " + lastCmd);
    TEST_ASSERT_TRUE(lastCmd.indexOf("R50") >= 0); // Should send R50 command
    debug("[voltage]: Command validation passed - R50 command detected");
}

// Test current reading with positive and negative values
void test_read_current(void) {
    debug("[current]: Testing current reading with positive and negative values");
    
    // Test positive current
    debug("[current]: Setting mock current to +2.50A (charging)");
    mockStream->setCurrent(2.50);
    monitor->invalidateCache();
    
    debug("[current]: Querying library for positive current");
    float current = monitor->getCurrent();
    debug("[current]: Library returned: " + String(current) + "A");
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.50, current);
    
    // Test negative current (discharge)
    debug("[current]: Setting mock current to -1.25A (discharging)");
    mockStream->setCurrent(-1.25);
    monitor->invalidateCache();
    
    debug("[current]: Querying library for negative current");
    current = monitor->getCurrent();
    debug("[current]: Library returned: " + String(current) + "A");
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1.25, current);
    
    debug("[current]: Current reading tests passed");
}

// Test temperature reading
void test_read_temperature(void) {
    debug("[temperature]: Testing temperature reading");
    // Set temperature to 25°C
    debug("[temperature]: Setting mock temperature to 25°C");
    mockStream->setTemperature(25);
    monitor->invalidateCache();
    
    debug("[temperature]: Querying library for temperature");
    int temperature = monitor->getTemperature();
    
    debug("[temperature]: Library returned: " + String(temperature) + "°C");
    TEST_ASSERT_EQUAL(25, temperature);
    debug("[temperature]: Temperature reading test passed");
}

// Test remaining capacity reading
void test_read_remaining_capacity(void) {
    // Set remaining capacity to 75.5 Ah
    debug("[remaining capacity]: setting to 75.5 Ah");
    mockStream->setRemainingCapacity(75.5);
    monitor->invalidateCache();
    
    debug("[remaining capacity ]: querying library");
    float capacity = monitor->getRemainingCapacity();
    
    debug("[remaining capacity] library read: " + String(capacity));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 75.5, capacity);
}

// Test output status interpretation
void test_output_status(void) {
    debug("[output status]: Testing output status interpretation");
    
    // Test different output statuses
    debug("[output status]: Setting output status to 0 (ON)");
    mockStream->setOutputStatus(0); // ON
    // Library should reflect this status
    
    debug("[output status]: Setting output status to 1 (OVP)");
    mockStream->setOutputStatus(1); // OVP
    // Library should detect over-voltage protection
    
    debug("[output status]: Setting output status to 255 (OFF)");
    mockStream->setOutputStatus(255); // OFF
    // Library should detect OFF status
    
    // Note: Need to check if library has methods to read status
    debug("[output status]: Placeholder test - library status methods need verification");
    TEST_ASSERT_TRUE(true); // Placeholder until status methods are verified
}

// Test current direction reading
void test_current_direction(void) {
    debug("[current direction]: Testing current direction reading");
    
    // Test forward current
    debug("[current direction]: Setting current direction to 0 (forward/charging)");
    mockStream->setCurrentDirection(0);
    
    int direction = monitor->getCurrentDirection();
    debug("[current direction]: Library returned direction: " + String(direction));
    
    // Test reverse current  
    debug("[current direction]: Setting current direction to 1 (reverse/discharging)");
    mockStream->setCurrentDirection(1);
    
    direction = monitor->getCurrentDirection();
    debug("[current direction]: Library returned direction: " + String(direction));
    
    // Note: Need to verify if library has getCurrentDirection() method
    debug("[current direction]: Current direction test completed");
    TEST_ASSERT_TRUE(true); // Placeholder
}

// Test checksum verification
void test_checksum_verification(void) {
    debug("[checksum]: Testing checksum verification");
    
    // Test with correct checksum
    String testResponse = ":r50=1,215,2056,200,5408,4592,9437,14353,134,0,0,0,162,30682,\r\n";
    debug("[checksum]: Injecting response with correct checksum");
    debug("[checksum]: Response: " + testResponse);
    mockStream->injectResponse(testResponse);
    monitor->invalidateCache();
    
    // Library should accept this response
    debug("[checksum]: Querying library for voltage (should accept response)");
    float voltage = monitor->getVoltage();
    debug("[checksum]: Library returned voltage: " + String(voltage) + "V");
    TEST_ASSERT_FLOAT_WITHIN(0.01, 20.56, voltage);
    
    // Test with incorrect checksum (if library validates checksums)
    String badResponse = ":r50=1,999,2056,200,5408,4592,9437,14353,134,0,0,0,162,30682,\r\n";
    debug("[checksum]: Clearing buffer and injecting response with bad checksum");
    debug("[checksum]: Bad response: " + badResponse);
    mockStream->clearBuffer();
    mockStream->injectResponse(badResponse);
    
    // Library should reject this response or handle error
    debug("[checksum]: Querying library for voltage (should handle bad checksum)");
    voltage = monitor->getVoltage();
    debug("[checksum]: Library returned voltage: " + String(voltage) + "V");
    // Note: Need to verify if library actually validates checksums
    debug("[checksum]: Checksum verification test completed");
}

// Test write commands - over-voltage protection
void test_write_ovp_setting(void) {
    debug("[OVP write]: Testing over-voltage protection setting");
    
    // Test setting over-voltage protection to 14.4V
    debug("[OVP write]: Setting over-voltage protection to 14.4V");
    bool result = monitor->setOverVoltageProtection(14.4f);
    
    debug("[OVP write]: Library returned: " + String(result ? "SUCCESS" : "FAILED"));
    TEST_ASSERT_TRUE(result);
    
    // Verify correct command was sent
    String lastCmd = mockStream->getLastCommand();
    debug("[OVP write]: Last command sent: " + lastCmd);
    TEST_ASSERT_TRUE(lastCmd.indexOf("W20") >= 0); // W20 is OVP command
    TEST_ASSERT_TRUE(lastCmd.indexOf("1440") >= 0); // 14.4V = 1440 in protocol
    debug("[OVP write]: Command validation passed - W20 with 1440 detected");
}

// Test write commands - under-voltage protection
void test_write_uvp_setting(void) {
    debug("[UVP write]: Testing under-voltage protection setting");
    
    // Test setting under-voltage protection to 10.5V
    debug("[UVP write]: Setting under-voltage protection to 10.5V");
    bool result = monitor->setUnderVoltageProtection(10.5f);
    
    debug("[UVP write]: Library returned: " + String(result ? "SUCCESS" : "FAILED"));
    TEST_ASSERT_TRUE(result);
    
    // Verify correct command was sent
    String lastCmd = mockStream->getLastCommand();
    debug("[UVP write]: Last command sent: " + lastCmd);
    TEST_ASSERT_TRUE(lastCmd.indexOf("W21") >= 0); // W21 is UVP command
    TEST_ASSERT_TRUE(lastCmd.indexOf("1050") >= 0); // 10.5V = 1050 in protocol
    debug("[UVP write]: Command validation passed - W21 with 1050 detected");
}

// Test write commands - battery capacity
void test_write_battery_capacity(void) {
    debug("[Battery capacity]: Testing battery capacity setting");
    
    // Test setting battery capacity to 100.0Ah
    debug("[Battery capacity]: Setting battery capacity to 100.0Ah");
    bool result = monitor->setBatteryCapacity(100.0f);
    
    debug("[Battery capacity]: Library returned: " + String(result ? "SUCCESS" : "FAILED"));
    TEST_ASSERT_TRUE(result);
    
    // Verify correct command was sent
    String lastCmd = mockStream->getLastCommand();
    debug("[Battery capacity]: Last command sent: " + lastCmd);
    TEST_ASSERT_TRUE(lastCmd.indexOf("W28") >= 0); // W28 is battery capacity command
    TEST_ASSERT_TRUE(lastCmd.indexOf("1000") >= 0); // 100.0Ah = 1000 in protocol
    debug("[Battery capacity]: Command validation passed - W28 with 1000 detected");
}

// Test protocol error handling
void test_protocol_error_handling(void) {
    debug("[Error handling]: Testing protocol error response handling");
    
    // Test timeout scenario
    debug("[Error handling]: Testing timeout scenario with no response");
    mockStream->clearBuffer(); // No response
    
    float voltage = monitor->getVoltage();
    debug("[Error handling]: Library returned voltage with no response: " + String(voltage) + "V");
    // Should return error value or previous cached value
    
    // Test garbled data
    debug("[Error handling]: Testing garbled data response");
    mockStream->injectResponse("garbage data not protocol\r\n");
    voltage = monitor->getVoltage();
    debug("[Error handling]: Library returned voltage with garbled data: " + String(voltage) + "V");
    // Should handle gracefully
    
    debug("[Error handling]: Protocol error handling tests completed");
    TEST_ASSERT_TRUE(true); // Placeholder - need to verify error handling
}

// Test address handling
void test_device_addressing(void) {
    debug("[Device addressing]: Testing device address handling");
    
    // Test communication with different addresses
    debug("[Device addressing]: Creating monitor with address 2");
    BatteryMonitor monitor2;
    monitor2.begin(2, *mockStream); // Address 2
    
    // Commands should use address 2
    debug("[Device addressing]: Requesting voltage from address 2");
    float voltage = monitor2.getVoltage();
    debug("[Device addressing]: Library returned voltage: " + String(voltage) + "V");
    
    String lastCmd = mockStream->getLastCommand();
    debug("[Device addressing]: Last command sent: " + lastCmd);
    TEST_ASSERT_TRUE(lastCmd.indexOf("=2,") >= 0); // Should use address 2
    debug("[Device addressing]: Address validation passed - address 2 detected in command");
}

// Test cache functionality
void test_cache_behavior(void) {
    debug("[Cache behavior]: Testing cache functionality");
    
    // Set cache time to 100ms
    debug("[Cache behavior]: Setting cache time to 100ms");
    monitor->setCacheTime(100);
    
    // First read should trigger actual command
    debug("[Cache behavior]: First voltage read - should trigger command");
    mockStream->clearBuffer();
    float voltage1 = monitor->getVoltage();
    String cmd1 = mockStream->getLastCommand();
    debug("[Cache behavior]: Voltage 1: " + String(voltage1) + "V, Command: " + cmd1);
    debug("[Cache behavior]: Cache time after first read: " + String(monitor->getCacheTime()) + "ms");
    
    // Immediate second read should use cache (no new command)
    debug("[Cache behavior]: Second voltage read - should use cache");
    mockStream->clearBuffer();
    float voltage2 = monitor->getVoltage();
    String cmd2 = mockStream->getLastCommand();
    debug("[Cache behavior]: Voltage 2: " + String(voltage2) + "V, Command: " + cmd2);
    debug("[Cache behavior]: Cache status: " + String(cmd2.length() == 0 ? "HIT" : "MISS"));
    
    TEST_ASSERT_EQUAL_FLOAT(voltage1, voltage2);
    TEST_ASSERT_TRUE(cmd2.length() == 0); // No new command sent
    
    // Wait for cache to expire
    debug("[Cache behavior]: Waiting 150ms for cache to expire");
    delay(150);
    
    // Third read should trigger new command
    debug("[Cache behavior]: Third voltage read - cache should be expired");
    mockStream->clearBuffer();
    float voltage3 = monitor->getVoltage();
    String cmd3 = mockStream->getLastCommand();
    debug("[Cache behavior]: Voltage 3: " + String(voltage3) + "V, Command: " + cmd3);
    debug("[Cache behavior]: Cache status: " + String(cmd3.length() > 0 ? "EXPIRED" : "ERROR"));
    
    TEST_ASSERT_TRUE(cmd3.length() > 0); // New command sent
    debug("[Cache behavior]: Cache behavior tests completed");
}

void setup() {
    UNITY_BEGIN();
    
    RUN_TEST(test_read_basic_info);
    RUN_TEST(test_mock_response_generation);
    RUN_TEST(test_read_voltage);
    RUN_TEST(test_read_current);
    RUN_TEST(test_read_temperature);
    RUN_TEST(test_read_remaining_capacity);
    RUN_TEST(test_output_status);
    RUN_TEST(test_current_direction);
    RUN_TEST(test_checksum_verification);
    RUN_TEST(test_write_ovp_setting);
    RUN_TEST(test_write_uvp_setting);
    RUN_TEST(test_write_battery_capacity);
    RUN_TEST(test_protocol_error_handling);
    RUN_TEST(test_device_addressing);
    RUN_TEST(test_cache_behavior);
    
    UNITY_END();
}

void loop() {
    // Empty
}

#ifdef NATIVE_BUILD
// Native build - use main() function
int main() {
    setup();
    return 0;
}
#endif
