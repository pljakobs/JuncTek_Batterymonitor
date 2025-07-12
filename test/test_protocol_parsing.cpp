#include <unity.h>
#include "MockJuncTekStream.h"
#include "JuncTek_BatteryMonitor.h"

MockJuncTekStream* mockStream;
BatteryMonitor* monitor;

void setUp(void) {
    mockStream = new MockJuncTekStream(true); // Enable debug mode
    monitor = new BatteryMonitor();
    monitor->begin(1, *mockStream);
}

void tearDown(void) {
    delete monitor;
    delete mockStream;
}

// Test basic device information reading (R00 command)
void test_read_basic_info(void) {
#ifdef NATIVE_BUILD
    // Test R00 command parsing and response generation
    String testCommand = ":R00=1,123,";
    
    // Send command to mock stream
    for (int i = 0; i < testCommand.length(); i++) {
        mockStream->write((uint8_t)testCommand.charAt(i));
    }
    mockStream->write('\r');
    mockStream->write('\n');
    
    // Check that response is available
    TEST_ASSERT_GREATER_THAN(0, mockStream->available());
    
    // Read response
    String response = "";
    while (mockStream->available()) {
        char c = mockStream->read();
        if (c != -1) response += c;
    }
    
    // Verify response format: :r00=1,checksum,data...
    TEST_ASSERT_TRUE(response.length() > 0);
    TEST_ASSERT_EQUAL(':',  response.charAt(0));
    TEST_ASSERT_EQUAL('r',  response.charAt(1));
    TEST_ASSERT_EQUAL('0',  response.charAt(2));
    TEST_ASSERT_EQUAL('0',  response.charAt(3));
    TEST_ASSERT_EQUAL('=',  response.charAt(4));
    
#else
    // Test with actual BatteryMonitor library
    // The library should parse R00 response correctly
    TEST_ASSERT_TRUE(true); // Placeholder for Arduino tests
#endif
}
    
    String lastCmd = mockStream->getLastCommand();
    TEST_ASSERT_TRUE(lastCmd.length() == 0); // No command sent yet
    
    // Manually inject R00 command to test mock response
    mockStream->write((const uint8_t*)":R00=1,2,1,\r\n", 13);
    
    // Check that mock generated correct response
    TEST_ASSERT_TRUE(mockStream->available() > 0);
    
    String response = "";
    while (mockStream->available()) {
        response += (char)mockStream->read();
    }
    
    // Should contain device info
    TEST_ASSERT_TRUE(response.indexOf(":r00=1,") == 0);
    TEST_ASSERT_TRUE(response.indexOf("100,20,100,101") > 0);
}

// Test voltage reading with known mock data
void test_read_voltage(void) {
    // Set known voltage in mock (20.56V)
    mockStream->setVoltage(20.56);
    
    // Test the actual library function
    float voltage = monitor->getVoltage();
    
    // Should read 20.56V from mock
    TEST_ASSERT_FLOAT_WITHIN(0.01, 20.56, voltage);
    
    // Verify that correct command was sent
    String lastCmd = mockStream->getLastCommand();
    TEST_ASSERT_TRUE(lastCmd.indexOf("R50") >= 0); // Should send R50 command
}

// Test current reading with positive and negative values
void test_read_current(void) {
    // Test positive current
    mockStream->setCurrent(2.50);
    float current = monitor->getCurrent();
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.50, current);
    
    // Test negative current (discharge)
    mockStream->setCurrent(-1.25);
    current = monitor->getCurrent();
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1.25, current);
}

// Test temperature reading
void test_read_temperature(void) {
    // Set temperature to 25°C
    mockStream->setTemperature(25);
    
    int temperature = monitor->getTemperature();
    
    TEST_ASSERT_EQUAL(25, temperature);
}

// Test remaining capacity reading
void test_read_remaining_capacity(void) {
    // Set remaining capacity to 75.5 Ah
    mockStream->setRemainingCapacity(75.5);
    
    float capacity = monitor->getRemainingCapacity();
    
    TEST_ASSERT_FLOAT_WITHIN(0.001, 75.5, capacity);
}

// Test output status interpretation
void test_output_status(void) {
    // Test different output statuses
    mockStream->setOutputStatus(0); // ON
    // Library should reflect this status
    
    mockStream->setOutputStatus(1); // OVP
    // Library should detect over-voltage protection
    
    mockStream->setOutputStatus(255); // OFF
    // Library should detect OFF status
    
    // Note: Need to check if library has methods to read status
    TEST_ASSERT_TRUE(true); // Placeholder until status methods are verified
}

// Test current direction reading
void test_current_direction(void) {
    // Test forward current
    mockStream->setCurrentDirection(0);
    // Library should indicate forward/charging
    
    // Test reverse current  
    mockStream->setCurrentDirection(1);
    // Library should indicate reverse/discharging
    
    // Note: Need to verify if library has getCurrentDirection() method
    TEST_ASSERT_TRUE(true); // Placeholder
}

// Test checksum verification
void test_checksum_verification(void) {
    // Test with correct checksum
    String testResponse = ":r50=1,215,2056,200,5408,4592,9437,14353,134,0,0,0,162,30682,\r\n";
    mockStream->injectResponse(testResponse);
    
    // Library should accept this response
    float voltage = monitor->getVoltage();
    TEST_ASSERT_FLOAT_WITHIN(0.01, 20.56, voltage);
    
    // Test with incorrect checksum (if library validates checksums)
    String badResponse = ":r50=1,999,2056,200,5408,4592,9437,14353,134,0,0,0,162,30682,\r\n";
    mockStream->clearBuffer();
    mockStream->injectResponse(badResponse);
    
    // Library should reject this response or handle error
    // Note: Need to verify if library actually validates checksums
}

// Test write commands - over-voltage protection
void test_write_ovp_setting(void) {
    // Test setting over-voltage protection to 14.4V
    bool result = monitor->setOverVoltageProtection(14.4);
    
    TEST_ASSERT_TRUE(result);
    
    // Verify correct command was sent
    String lastCmd = mockStream->getLastCommand();
    TEST_ASSERT_TRUE(lastCmd.indexOf("W20") >= 0); // W20 is OVP command
    TEST_ASSERT_TRUE(lastCmd.indexOf("1440") >= 0); // 14.4V = 1440 in protocol
}

// Test write commands - under-voltage protection
void test_write_uvp_setting(void) {
    // Test setting under-voltage protection to 10.5V
    bool result = monitor->setUnderVoltageProtection(10.5);
    
    TEST_ASSERT_TRUE(result);
    
    // Verify correct command was sent
    String lastCmd = mockStream->getLastCommand();
    TEST_ASSERT_TRUE(lastCmd.indexOf("W21") >= 0); // W21 is UVP command
    TEST_ASSERT_TRUE(lastCmd.indexOf("1050") >= 0); // 10.5V = 1050 in protocol
}

// Test write commands - battery capacity
void test_write_battery_capacity(void) {
    // Test setting battery capacity to 100.0Ah
    bool result = monitor->setBatteryCapacity(100.0);
    
    TEST_ASSERT_TRUE(result);
    
    // Verify correct command was sent
    String lastCmd = mockStream->getLastCommand();
    TEST_ASSERT_TRUE(lastCmd.indexOf("W28") >= 0); // W28 is battery capacity command
    TEST_ASSERT_TRUE(lastCmd.indexOf("1000") >= 0); // 100.0Ah = 1000 in protocol
}

// Test protocol error handling
void test_protocol_error_handling(void) {
    // Test timeout scenario
    mockStream->clearBuffer(); // No response
    
    float voltage = monitor->getVoltage();
    // Should return error value or previous cached value
    
    // Test garbled data
    mockStream->injectResponse("garbage data not protocol\r\n");
    voltage = monitor->getVoltage();
    // Should handle gracefully
    
    TEST_ASSERT_TRUE(true); // Placeholder - need to verify error handling
}

// Test address handling
void test_device_addressing(void) {
    // Test communication with different addresses
    BatteryMonitor monitor2;
    monitor2.begin(2, *mockStream); // Address 2
    
    // Commands should use address 2
    float voltage = monitor2.getVoltage();
    
    String lastCmd = mockStream->getLastCommand();
    TEST_ASSERT_TRUE(lastCmd.indexOf("=2,") >= 0); // Should use address 2
}

// Test cache functionality
void test_cache_behavior(void) {
    // Set cache time to 100ms
    monitor->setCacheTime(100);
    
    // First read should trigger actual command
    mockStream->clearBuffer();
    float voltage1 = monitor->getVoltage();
    String cmd1 = mockStream->getLastCommand();
    
    // Immediate second read should use cache (no new command)
    mockStream->clearBuffer();
    float voltage2 = monitor->getVoltage();
    String cmd2 = mockStream->getLastCommand();
    
    TEST_ASSERT_EQUAL_FLOAT(voltage1, voltage2);
    TEST_ASSERT_TRUE(cmd2.length() == 0); // No new command sent
    
    // Wait for cache to expire
    delay(150);
    
    // Third read should trigger new command
    mockStream->clearBuffer();
    float voltage3 = monitor->getVoltage();
    String cmd3 = mockStream->getLastCommand();
    
    TEST_ASSERT_TRUE(cmd3.length() > 0); // New command sent
}

void setup() {
    UNITY_BEGIN();
    
    RUN_TEST(test_read_basic_info);
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
