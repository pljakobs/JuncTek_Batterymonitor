  

void BatteryMonitor(int address, Stream *serialDevice){
  bm_serial=serialDevice;
  bm_address=address;
  setValues.deviceAddress=bm_address;
  basicInfo.deviceAddress=bm_address;
  measuredValues.deviceAddress=bm_address;
  
}
void ~BatteryMonitor(){
  
}
void setAddress(uint8_t newAddress){
    
}
void setOutoupt(bool output){
    
  }
void setOverVoltageProtection(int voltage){
  
}
void setUnderVoltageProtection(int voltage){
  
}
void setOverCurrentProtection(int current){
  
}
void setUnderCurrentProtection(int current){
  
}
void setOverPowerProtection(int power){
  
}
void setUnderPowerProtection(int power){
  
}
void setBatteryCapacity(int capacity){
  
}
void setVoltageCalibration(int calibrationVoltage){
  
}
void setCurrentCalibration(int calibrationCurrent){
  
}
void setTemperatureCalibration(int calibrationTemperature){
  
}
void setRelayType(int relayType){
  
}
void resetFactorySettings(){
  
}
void setCurrentMultiplier(int currentMultiplier){
  
}
void setBatteryPercent(uint8_t batteryPercent){
  
}
void zeroCurrent(){
  
}
void clearAccountingData(){

}

int getUptime(){
  
}
int getBatteryLifeLeft(){
  
}
int getTemperature(){
  
}
   
int getProtectionTemperature(){
  
}
int getProtectionRecoveryTime(){
  
}
int getProtectionDelayTime(){
  
}
int getCapacity(){
  
}
int getVoltageCalibration(){
  
}
int getCurrentCalibration(){
  
}
int getTemperatureCalibration(){
  
}
int getVoltageScale(){
  
}
int getCurrentScale(){
  
}

float getVoltage() {
  
}
float getCurrent(){
  
}
float getInternalResistance(){
  
}
float getRemainingCapacity(){
  
}
float getCumulativeCapacity(){
  
}

float getOVPVoltage(){
  
}
float getUVPVoltage(){
  
}
float getOCPForwardCurrent(){
  
}
float getOCPReverseCurrent(){
  
}
float getOPPPower(){
  
}


void getBasicInfo(){
  // result syntax: :r00=<addr>,<checksum>,<sensor_type:1><voltage:1><amperage:2>,<firmware_version>,<machine_serial_nr>,
  String message;
  sendMessage(bm_address, BM_F_ReadBasicInf, 1);
  message=readMessage();

  basicInfo.maxVoltage=getStringField(message,3).toFloat()/100;
  basicInfo.maxCurrent=getStringField(message,4).toFloat()/100;
  basicInfo.sensortype=getStringField(message,5).toInt();
  basicInfo.deviceVersion=getStringField(message,6).toInt();
  basicInfo.deviceSerialNumber=getStringField(message,7).toInt();
}
void getMeasuredValues(){
  String messsage;
  sendMessage(bm_addrss, BM_F_ReadMsrdVals, 1);
  message=readMessage();

  /*
  * :r50=<addr>,
  * 01 - <addr>
  * 02 - <checksum>,
  * 03 - <Voltage (1/100V)>,
  * 04 - <Current (1/100A)>,
  * 05 - <remaining capacity mAh>,
  * 06 - <cumulative capacity mAh>,
  * 07 - <Capacity (mWh)>,
  * 08 - <running time (s)>,
  * 09 - <temperature (°C+100)>,
  * 10 - <reserved>,
  * 11 - <output State>,
  * 12 - <current direction>,
  * 13 - <remaining battery life (minutes)>,
  * 14 - <internal resistance (mOhm/100)>
  */
 
  measuredValues.uptime=getStringField(message,8).toInt();
  measuredValues.batteryLifeLeft=getStringField(message,13).toInt();
  measuredValues.temperature=getStringField(message,9).toInt()-100;

  measuredValues.voltage=getStringField(message,3).toFloat()/100;
  measuredValues.current=getStringField(message,4).toFloat()/100;
  measuredValues.internalResistance=getStringField(message,14).toFloat()/100;
  measuredValues.remainingCapacity=getStringField(message,5).toFloat();
  measuredValues.cumulativeCapacity=getStringField(message,5).toFloat();

  measuredValues.outputState=getStringField(message,11).toInt();
  measuredValues.currentDir=getStringField(message,12).toInt();
}
void getSetValues(){
  
}

String getStringField(String message, int idx){
  String field;
  int curr_idx,next_idx;
  next_idx=message.indexOf("=");
  //Serial.println(message);
  for(int i=1;i<=idx;i++){
    if(next_idx!=-1){
      curr_idx=next_idx;
      next_idx=message.indexOf(",",curr_idx+1);
      Serial.printf("Fieldnum: %i, current idx: %i, next idx: %i\n", i, curr_idx, next_idx);
    }else{
      curr_idx=0;         // setting the indexes to 0 and 1
      next_idx=1;         // will make sure the return string is length 0
      i=idx;              // exit the loop
    }
  }
  field=message.substring(curr_idx+1,next_idx);
  return field;
}

void sendMessage(int address, int command, int parameter){
  char message[40];
  sprintf(message, "R%02i:=%i,%i,%i\r\n", command, address, checksum(parameter), parameter);
  bm_serial.print(message);
}

String readMessage(){   
    String _message;
    msgState_t _msgState;
    char c;
    //Serial.println(".....start read loop");
    if (_msgState==crlf){
      //Serial.println(".....new message");
      _message="";
      _msgState=reading;
    }
    
a    if (sizeof(_message)==0) Serial.print("[start]");
    while(battMon.available() && _msgState != crlf){
        c=battMon.read();
        //Serial.print(c);
        _message=_message+String(c);
        //Serial.print(c);
        if (c=='\r') {
          _msgState=cr;
          //Serial.print("{cr}");
        }
        if (_msgState==cr && c=='\n') _msgState=crlf;
        
        //if (_msgState==reading) Serial.print("[r]");
        //if (_msgState==cr) Serial.print("[CR]");
        //if (_msgState==crlf) Serial.println("[CRLF]");
    }
    //Serial.println(".....finished read loop");
    //Serial.println(_message);
    return _message;
}
