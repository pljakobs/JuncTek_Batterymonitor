#include "MockJuncTekStream.h"

MockJuncTekStream::MockJuncTekStream(bool debug) : readIndex(0), debugMode(debug) {
}

int MockJuncTekStream::available() {
    return responseBuffer.length() - readIndex;
}

int MockJuncTekStream::read() {
    if (readIndex < responseBuffer.length()) {
        return responseBuffer.charAt(readIndex++);
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
    
    // Extract function number
    int funcStart = 2;
    int funcEnd = cmd.indexOf('=');
    if (funcEnd == -1) return "";
    
    String funcStr = cmd.substring(funcStart, funcEnd);
    int functionNum = funcStr.toInt();
    
    // Extract address
    int addrStart = funcEnd + 1;
    int addrEnd = cmd.indexOf(',', addrStart);
    if (addrEnd == -1) return "";
    
    uint8_t address = cmd.substring(addrStart, addrEnd).toInt();
    
    // Extract checksum
    int checksumStart = addrEnd + 1;
    int checksumEnd = cmd.indexOf(',', checksumStart);
    if (checksumEnd == -1) checksumEnd = cmd.length();
    
    uint8_t receivedChecksum = cmd.substring(checksumStart, checksumEnd).toInt();
    
    // Verify checksum (optional - can be disabled for testing)
    // For now, let's assume checksum is correct
    
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
        // Write command
        if (processWriteCommand(cmd)) {
            // Return acknowledgment for successful write
            uint8_t checksum = calculateChecksum(":w" + String(functionNum) + "=" + String(address));
            return ":w" + String(functionNum) + "=" + String(address) + "," + String(checksum) + ",OK,\r\n";
        }
    }
    
    return "";
}

String MockJuncTekStream::generateR00Response(uint8_t address) {
    // Format: :r00=address,checksum,hallSensor,voltageRange,currentRange,version,serial,
    String response = ":r00=" + String(address) + ",";
    String data = String(state.hallSensor) + "," + 
                  String(state.voltage_range) + "," + 
                  String(state.current_range) + "," + 
                  String(state.version) + "," + 
                  String(state.serial) + ",";
    
    uint8_t checksum = calculateChecksum(":r00=" + String(address));
    response += String(checksum) + "," + data + "\r\n";
    
    return response;
}

String MockJuncTekStream::generateR50Response(uint8_t address) {
    // Format: :r50=address,checksum,voltage,current,remaining_cap,cumulative_cap,watt_hour,
    //         runtime,temperature,reserved1,output_status,current_direction,battery_life,internal_resistance,
    String response = ":r50=" + String(address) + ",";
    String data = String(state.voltage) + "," + 
                  String(state.current) + "," + 
                  String(state.remaining_capacity) + "," + 
                  String(state.cumulative_capacity) + "," + 
                  String(state.watt_hour) + "," + 
                  String(state.runtime) + "," + 
                  String(state.temperature) + "," + 
                  String(state.reserved1) + "," + 
                  String(state.output_status) + "," + 
                  String(state.current_direction) + "," + 
                  String(state.battery_life) + "," + 
                  String(state.internal_resistance) + ",";
    
    uint8_t checksum = calculateChecksum(":r50=" + String(address));
    response += String(checksum) + "," + data + "\r\n";
    
    return response;
}

String MockJuncTekStream::generateR51Response(uint8_t address) {
    // Format: :r51=address,checksum,ovp,uvp,ocp_forward,ocp_reverse,opp,otp,recovery_time,
    //         delay_time,battery_capacity,voltage_cal,current_cal,temp_cal,reserved2,relay_type,
    //         current_multiple,voltage_scale,current_scale,
    String response = ":r51=" + String(address) + ",";
    String data = String(state.ovp_voltage) + "," + 
                  String(state.uvp_voltage) + "," + 
                  String(state.ocp_forward) + "," + 
                  String(state.ocp_reverse) + "," + 
                  String(state.opp_power) + "," + 
                  String(state.otp_temp) + "," + 
                  String(state.recovery_time) + "," + 
                  String(state.delay_time) + "," + 
                  String(state.battery_capacity) + "," + 
                  String(state.voltage_cal) + "," + 
                  String(state.current_cal) + "," + 
                  String(state.temp_cal) + "," + 
                  String(state.reserved2) + "," + 
                  String(state.relay_type) + "," + 
                  String(state.current_multiple) + "," + 
                  String(state.voltage_scale) + "," + 
                  String(state.current_scale) + ",";
    
    uint8_t checksum = calculateChecksum(":r51=" + String(address));
    response += String(checksum) + "," + data + "\r\n";
    
    return response;
}

bool MockJuncTekStream::processWriteCommand(const String& cmd) {
    // Parse write command and update state
    // For now, return true (successful write)
    // TODO: Implement actual parameter parsing and state updates
    return true;
}
