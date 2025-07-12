#include "MockJuncTekStream.h"

#ifdef NATIVE_BUILD
// Define the mock Serial object for native builds
MockSerial Serial;
#endif

MockJuncTekStream::MockJuncTekStream(uint8_t deviceAddress, uint16_t version, uint8_t hallSensor, bool debug)
    : readIndex(0), debugMode(debug) {
    // Initialize device state with provided values
    state.hallSensor = hallSensor;
    state.voltage_range = 100; // default 100V
    state.current_range = 20;  // default 200A
    state.version = version;
    state.serial = 101; // default serial
    // Optionally, you can add more parameters to the constructor for other fields
    // and set them here as needed.
}

MockJuncTekStream::MockJuncTekStream(bool debug) : MockJuncTekStream(1, 100, 1, debug) {}

int MockJuncTekStream::available() {
    return responseBuffer.length() - readIndex;
}

int MockJuncTekStream::read() {
    if (readIndex < responseBuffer.length()) {
        char ch = responseBuffer.charAt(readIndex++);
        int asciiValue = (int)ch;
        #ifdef NATIVE_BUILD
        printf("[MockRead: char='%c' ascii=%d] ", ch, asciiValue);
        #endif
        return asciiValue;
    }
    return -1;
}

int MockJuncTekStream::peek() {
    if (readIndex < responseBuffer.length()) {
        return responseBuffer.charAt(readIndex);
    }
    return -1;
}

size_t MockJuncTekStream::write(uint8_t data) {
    static String commandBuffer = "";
    
    if (data == '\r' || data == '\n') {
        if (commandBuffer.length() > 0) {
            lastCommand = commandBuffer;
            if (debugMode) {
                Serial.print("Mock received: ");
                Serial.println(commandBuffer);
            }
            
            // Process the command and generate response
            String response = parseCommand(commandBuffer);
            if (response.length() > 0) {
                injectResponse(response);
                if (debugMode) {
                    Serial.print("Mock responding: ");
                    Serial.println(response);
                }
            }
            commandBuffer = "";
        }
    } else {
        commandBuffer += (char)data;
    }
    
    return 1;
}

size_t MockJuncTekStream::write(const uint8_t *buffer, size_t size) {
    size_t written = 0;
    for (size_t i = 0; i < size; i++) {
        written += write(buffer[i]);
    }
    return written;
}

void MockJuncTekStream::injectResponse(const String& response) {
    responseBuffer = response;
    readIndex = 0;
}

void MockJuncTekStream::clearBuffer() {
    responseBuffer = "";
    readIndex = 0;
}

uint8_t MockJuncTekStream::calculateChecksum(const String& data) {
    // JuncTek checksum: sum of all ASCII values after ':' and before checksum field
    // Then take remainder when divided by 255, add 1
    int sum = 0;
    bool startCounting = false;
    int commaCount = 0;
    
    for (int i = 0; i < data.length(); i++) {
        char c = data.charAt(i);
        if (c == ':') {
            startCounting = true;
            continue;
        }
        if (startCounting && c == ',') {
            commaCount++;
            if (commaCount >= 2) break; // Stop before checksum field
        }
        if (startCounting && c != ',') {
            sum += (int)c;
        }
    }
    
    return (sum % 255) + 1;
}

String MockJuncTekStream::parseCommand(const String& cmd) {
    // Parse JuncTek command format: :W,R00~99=1-99,Checksum,Data field,<CR><LF>
    if (cmd.length() < 8 || cmd.charAt(0) != ':') {
        return ""; // Invalid command
    }
    
    char operation = cmd.charAt(1); // W or R
    
    // Extract function number (2 digits)
    String funcStr = cmd.substring(2, 4);
    int functionNum = funcStr.toInt();
    
    // Extract address
    int addrStart = cmd.indexOf('=') + 1;
    int addrEnd = cmd.indexOf(',', addrStart);
    if (addrEnd == -1) return "";
    
    uint8_t address = cmd.substring(addrStart, addrEnd).toInt();
    
    if (operation == 'R' || operation == 'r') {
        // Read command
        switch (functionNum) {
            case 0:
                return generateR00Response(address);
            case 50:
                return generateR50Response(address);
            case 51:
                return generateR51Response(address);
            default:
                return ""; // Unknown function
        }
    } else if (operation == 'W' || operation == 'w') {
        // Write command - process and return acknowledgment
        if (processWriteCommand(cmd, functionNum, address)) {
            // Return acknowledgment for successful write
            uint8_t checksum = calculateChecksum(":w" + String(functionNum));
            return ":w" + String(functionNum) + "=" + String(address) + "," + String(checksum) + ",OK,\r\n";
        }
    }
    
    return "";
}

String MockJuncTekStream::generateR00Response(uint8_t address) {
    // R00 Response format from PROTOCOL.md:
    // :r00=address,checksum,sensor_voltage_current,version,serial,
    // Example: :r00=1,47,1120,100,101,
    // Where 1120 = 1(Hall sensor) + 1(100V) + 20(200A)
    
    String sensorInfo = String(state.hallSensor) + String(state.voltage_range/100) + String(state.current_range);
    String data = sensorInfo + "," + String(state.version) + "," + String(state.serial) + ",";
    
    uint8_t checksum = calculateChecksum(":r00=" + String(address) + "," + data);
    String response = ":r00=" + String(address) + "," + String(checksum) + "," + data + "\r\n";
    
    return response;
}

String MockJuncTekStream::generateR50Response(uint8_t address) {
    // R50 Response format from PROTOCOL.md (live measured values):
    // :r50=address,checksum,voltage,current,remaining_cap,cumulative_cap,energy,
    //      runtime,temperature,reserved,output_status,current_direction,battery_life,internal_resistance,
    // Example: :r50=2,215,2056,200,5408,4592,9437,14353,134,0,0,0,162,30682,
    
    String data = String(state.voltage) + "," +           // voltage (×100)
                  String(state.current) + "," +           // current (×100) 
                  String(state.remaining_capacity) + "," + // remaining capacity (×1000)
                  String(state.cumulative_capacity) + "," +// cumulative capacity (×1000)
                  String(state.watt_hour) + "," +         // energy (×100000)
                  String(state.runtime) + "," +           // runtime in seconds
                  String(state.temperature) + "," +       // temperature (+100)
                  "0," +                                   // reserved
                  String(state.output_status) + "," +     // output status
                  String(state.current_direction) + "," + // current direction
                  String(state.battery_life) + "," +      // battery life in minutes
                  String(state.internal_resistance) + ","; // internal resistance (×100)
    
    uint8_t checksum = calculateChecksum(":r50=" + String(address) + "," + data);
    String response = ":r50=" + String(address) + "," + String(checksum) + "," + data + "\r\n";
    
    return response;
}

String MockJuncTekStream::generateR51Response(uint8_t address) {
    // R51 Response format from PROTOCOL.md (configuration settings):
    // :r51=address,checksum,ovp,uvp,ocp_forward,ocp_reverse,opp,otp,recovery_time,
    //      delay_time,battery_capacity,voltage_cal,current_cal,temp_cal,reserved,relay_type,
    //      current_multiple,voltage_scale,current_scale,
    // Example: :r51=1,211,3000,100,2000,2000,10000,151,10,7,200,120,90,101,0,0,2,12,13,
    
    String data = String(state.ovp_voltage) + "," +       // OVP voltage (×100)
                  String(state.uvp_voltage) + "," +       // UVP voltage (×100)
                  String(state.ocp_forward) + "," +       // Forward OCP (×100)
                  String(state.ocp_reverse) + "," +       // Reverse OCP (×100)
                  String(state.opp_power) + "," +         // OPP power (×100)
                  String(state.otp_temp) + "," +          // OTP temperature (+100)
                  String(state.recovery_time) + "," +     // Recovery time
                  String(state.delay_time) + "," +        // Delay time
                  String(state.battery_capacity) + "," +  // Battery capacity (×10)
                  String(state.voltage_cal) + "," +       // Voltage calibration
                  String(state.current_cal) + "," +       // Current calibration
                  String(state.temp_cal) + "," +          // Temp calibration (+100)
                  String(state.reserved2) + "," +         // Reserved
                  String(state.relay_type) + "," +        // Relay type
                  String(state.current_multiple) + "," +  // Current multiple
                  String(state.voltage_scale) + "," +     // Voltage scale
                  String(state.current_scale) + ",";      // Current scale
    
    uint8_t checksum = calculateChecksum(":r51=" + String(address) + "," + data);
    String response = ":r51=" + String(address) + "," + String(checksum) + "," + data + "\r\n";
    
    return response;
}

bool MockJuncTekStream::processWriteCommand(const String& cmd, int functionNum, uint8_t address) {
    // Parse write command and update state based on function number
    // Extract data field from command
    int dataStart = cmd.lastIndexOf(',') + 1;
    int dataEnd = cmd.indexOf('\r', dataStart);
    if (dataEnd == -1) dataEnd = cmd.indexOf('\n', dataStart);
    if (dataEnd == -1) dataEnd = cmd.length();
    
    String dataField = cmd.substring(dataStart, dataEnd);
    
    // Process different write functions
    switch (functionNum) {
        case 50: // W50 - write protection settings
            // For testing, we'll accept any write to protection settings
            return true;
        case 51: // W51 - write system configuration  
            // For testing, we'll accept any write to system config
            return true;
        default:
            return false; // Unknown write function
    }
}
