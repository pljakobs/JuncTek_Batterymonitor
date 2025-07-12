#ifndef MOCKJUNCTEKSTREAM_H
#define MOCKJUNCTEKSTREAM_H

#ifdef NATIVE_BUILD
    #include <iostream>
    #include <string>
    #include <cstdint>
    #include <cstring>
    #include <chrono>
    #include <thread>
    
    // Native environment compatibility layer
    class String {
    private:
        std::string data;
    public:
        String() = default;
        String(const char* str) : data(str) {}
        String(const std::string& str) : data(str) {}
        String(char ch) : data(1, ch) {}  // Single character constructor
        String(int value) : data(std::to_string(value)) {}
        String(uint8_t value) : data(std::to_string(value)) {}
        String(uint32_t value) : data(std::to_string(value)) {}
        String(float value) : data(std::to_string(value)) {}
        
        size_t length() const { return data.length(); }
        char charAt(size_t index) const { return index < data.length() ? data[index] : 0; }
        int indexOf(char ch, size_t start = 0) const { 
            if (start >= data.length()) return -1; // Arduino behavior
            size_t pos = data.find(ch, start);
            return pos != std::string::npos ? pos : -1;
        }
        int indexOf(const char* substr, size_t start = 0) const { 
            if (start >= data.length() || !substr) return -1; // Arduino behavior
            size_t pos = data.find(substr, start);
            return pos != std::string::npos ? pos : -1;
        }
        int lastIndexOf(char ch) const { 
            size_t pos = data.rfind(ch);
            return pos != std::string::npos ? pos : -1;
        }
        String substring(size_t start, size_t end = std::string::npos) const {
            if (start >= data.length()) return String(""); // Arduino behavior - return empty string
            if (end == std::string::npos) end = data.length();
            if (end > data.length()) end = data.length();
            if (start >= end) return String(""); // Arduino behavior - return empty string
            return String(data.substr(start, end - start));
        }
        int toInt() const { 
            try {
                return std::stoi(data); 
            } catch (const std::exception&) {
                return 0; // Arduino behavior - return 0 for invalid strings
            }
        }
        float toFloat() const { 
            try {
                return std::stof(data); 
            } catch (const std::exception&) {
                return 0.0f; // Arduino behavior - return 0.0 for invalid strings
            }
        }
        String& operator+=(const String& other) { data += other.data; return *this; }
        String& operator+=(char ch) { data += ch; return *this; }
        String& operator+=(const char* str) { data += str; return *this; }
        String operator+(const String& other) const { return String(data + other.data); }
        String operator+(const char* str) const { return String(data + str); }
        const char* c_str() const { return data.c_str(); }
    };
    
    // Free function for const char* + String
    inline String operator+(const char* lhs, const String& rhs) {
        return String(std::string(lhs) + std::string(rhs.c_str()));
    }
    
    // Mock Serial for native environment
    class MockSerial {
    public:
        void print(const String& str) { std::cout << str.c_str(); }
        void println(const String& str) { std::cout << str.c_str() << std::endl; }
        void print(const char* s) { std::cout << s; }
        void println(const char* s) { std::cout << s << std::endl; }
    };
    extern MockSerial Serial;
    
    // Add missing Arduino functions for native builds
    inline unsigned long millis() {
        static auto start = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        return duration.count();
    }
    
    inline void delay(unsigned long ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
    
    // Native Stream base class
    class Stream {
    public:
        virtual ~Stream() = default;
        virtual int available() = 0;
        virtual int read() = 0;
        virtual int peek() = 0;
        virtual size_t write(uint8_t data) = 0;
        virtual size_t write(const uint8_t *buffer, size_t size) = 0;
        
        // Add print methods that BatteryMonitor uses
        virtual void print(const String& str) { 
            for (size_t i = 0; i < str.length(); i++) {
                write((uint8_t)str.charAt(i));
            }
        }
        virtual void println(const String& str) { 
            print(str);
            write('\r');
            write('\n');
        }
    };
    
#else
    #include <Arduino.h>
    #include <Stream.h>
#endif

/**
 * Mock Stream implementation for testing JuncTek Battery Monitor protocol
 * 
 * Based on the official JuncTek KL-F protocol specification:
 * Format: :W,R00~99=1-99,Checksum,Data field,<CR><LF>
 * 
 * This mock allows testing protocol parsing without actual hardware.
 */
class MockJuncTekStream : public Stream {
private:
    String responseBuffer;
    int readIndex;
    String lastCommand;
    bool debugMode;
    
    // Mock device state
    struct DeviceState {
        // Basic info (R00)
        uint8_t hallSensor = 1;      // 1=Hall sensor, 2=sampler
        uint16_t voltage_range = 100; // 100V
        uint16_t current_range = 20;  // 200A (20 = 200A)
        uint16_t version = 100;       // 1.00
        uint16_t serial = 101;        // Serial number
        
        // Measured values (R50)
        uint16_t voltage = 2056;      // 20.56V (value * 0.01)
        int16_t current = 200;        // 2.00A (value * 0.01)
        uint16_t remaining_capacity = 5408;  // 5.408Ah (value * 0.001)
        uint16_t cumulative_capacity = 4592; // 4.592Ah (value * 0.001)
        uint16_t watt_hour = 9437;    // 0.09437kWh (value * 0.00001)
        uint32_t runtime = 14353;     // 14353 seconds
        uint8_t temperature = 134;    // 34°C (value - 100)
        uint8_t reserved1 = 0;
        uint8_t output_status = 0;    // 0=ON, 1=OVP, 2=OCP, 3=LVP, 4=NCP, 5=OPP, 6=OTP, 255=OFF
        uint8_t current_direction = 0; // 0=forward, 1=reverse
        uint16_t battery_life = 162;  // 162 minutes
        uint16_t internal_resistance = 30682; // 306.82mΩ (value * 0.01)
        
        // Protection settings (R51)
        uint16_t ovp_voltage = 3000;  // 30.00V (value * 0.01)
        uint16_t uvp_voltage = 100;   // 1.00V (value * 0.01)
        uint16_t ocp_forward = 2000;  // 20.00A (value * 0.01)
        uint16_t ocp_reverse = 2000;  // -20.00A (value * 0.01)
        uint16_t opp_power = 10000;   // 100.00W (value * 0.01)
        uint8_t otp_temp = 151;       // 51°C (value - 100)
        uint8_t recovery_time = 10;   // 10s
        uint8_t delay_time = 7;       // 7s
        uint16_t battery_capacity = 200; // 20.0Ah (value * 0.1)
        uint8_t voltage_cal = 120;    // +20 fine-tuning (100 = no adjustment)
        uint8_t current_cal = 90;     // -10 fine-tuning (100 = no adjustment)
        uint8_t temp_cal = 101;       // +1°C (100 = no adjustment)
        uint8_t reserved2 = 0;
        uint8_t relay_type = 0;       // 0=normally open, 1=normally closed
        uint8_t current_multiple = 2; // Current multiplier
        uint8_t voltage_scale = 12;   // Voltage curve scale 12V/div
        uint8_t current_scale = 13;   // Current curve scale 13A/div
    } state;
    
    uint8_t calculateChecksum(const String& data);
    String parseCommand(const String& cmd);
    String generateR00Response(uint8_t address);
    String generateR50Response(uint8_t address);
    String generateR51Response(uint8_t address);
    bool processWriteCommand(const String& cmd, int functionNum, uint8_t address);
    
public:
    // New constructor with address, version, and sensor type
    MockJuncTekStream(uint8_t deviceAddress, uint16_t version = 100, uint8_t hallSensor = 1, bool debug = false);
    MockJuncTekStream(bool debug = false); // legacy, delegates to above
    
    // Stream interface
    int available() override;
    int read() override;
    int peek() override;
    size_t write(uint8_t data) override;
    size_t write(const uint8_t *buffer, size_t size) override;
    
    // Mock control methods
    void injectResponse(const String& response);
    void clearBuffer();
    String getLastCommand() const { return lastCommand; }
    void setDebugMode(bool enabled) { debugMode = enabled; }
    
    // Device state manipulation for testing
    void setVoltage(float volts) { state.voltage = (uint16_t)(volts * 100); }
    void setCurrent(float amps) { state.current = (int16_t)(amps * 100); }
    void setTemperature(int celsius) { state.temperature = celsius + 100; }
    void setRemainingCapacity(float ah) { state.remaining_capacity = (uint16_t)(ah * 1000); }
    void setOutputStatus(uint8_t status) { state.output_status = status; }
    void setCurrentDirection(uint8_t direction) { state.current_direction = direction; }
    
    // Simulate protocol errors for testing
    void simulateChecksumError(bool enable);
    void simulateTimeout(bool enable);
    void simulateGarbledData(bool enable);
};

#endif // MOCKJUNCTEKSTREAM_H
