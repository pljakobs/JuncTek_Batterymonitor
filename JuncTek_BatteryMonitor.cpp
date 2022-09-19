  #include "JuncTek_BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(){
//empty constructor
}
BatteryMonitor::~BatteryMonitor(){
  
}
void BatteryMonitor::begin(int address, Stream &serialDevice){
  bm_serial=&serialDevice;
  bm_address=address;
  setValues.deviceAddress=bm_address;
  basicInfo.deviceAddress=bm_address;
  measuredValues.deviceAddress=bm_address;
  cacheTime=CACHE_TIME; 
  getBasicInfo();
  
  //Serial.printf("=== Battery Monitor basic info ===\nmax Voltage: %i\nmax Current: %i\nSensor Type: %i\nVersion    : %i\nSerial Nr : %i",  basicInfo.maxVoltage, basicInfo.maxCurrent,basicInfo.sensorType,basicInfo.deviceVersion,basicInfo.deviceSerialNumber);
  getMeasuredValues();
  //Serial.printf("=== Battery Monitor measured values ===\nUptime: %i\nbattery life left: %i\nTemperature: %i\nVoltage: %f\nCurrent: %f\nint Resistanc: %f\nCapacity: %f\ncumulative capa: %f\nOutput State: %i\ncurrent direction: %i",measuredValues.uptime,measuredValues.batteryLifeLeft, measuredValues.temperature, measuredValues.voltage, measuredValues.current, measuredValues.internalResistance, measuredValues.remainingCapacity, measuredValues.cumulativeCapacity,measuredValues.outputState,measuredValues.currentDir);  
}

void BatteryMonitor::setNewAddress(uint8_t newAddress){
    
}
void BatteryMonitor::setOutput(bool output){
	if(output){
   	sendMessage(bm_address, BM_F_TurnOnOutput, 1);
   }else{
   	sendMessage(bm_address, BM_F_TurnOnOutput, 0);
   }
}
void BatteryMonitor::setOverVoltageProtection(int voltage){
	int v=(int)(voltage*100);
	sendMessage(bm_address, BM_F_SetOVProt, v);
}
void BatteryMonitor::setOverVoltageProtection(float voltage){
	int v=(int)(voltage*100);
	sendMessage(bm_address, BM_F_SetOVProt, v);
}

void BatteryMonitor::setUnderVoltageProtection(int voltage){
  	int v=(int)(voltage*100);
	sendMessage(bm_address, BM_F_SetUVProt, v);
}
void BatteryMonitor::setUnderVoltageProtection(float voltage){
  	int v=(int)(voltage*100);
	sendMessage(bm_address, BM_F_SetUVProt, v);
}

void BatteryMonitor::setPositiveOverCurrentProtection(int current){
  	int c=(int)(current*100);
  	sendMessage(bm_address, BM_F_SetPOCProt, c);
}
void BatteryMonitor::setPositiveOverCurrentProtection(float current){
  	int c=(int)(current*100);
  	sendMessage(bm_address, BM_F_SetPOCProt, c);
}

void BatteryMonitor::setNegativeOverCurrentProtection(int current){
   int c=(int)(current*100);
  	sendMessage(bm_address, BM_F_SetNOCProt, c);
}
void BatteryMonitor::setNegativeOverCurrentProtection(float current){
   int c=(int)(current*100);
  	sendMessage(bm_address, BM_F_SetNOCProt, c);
}

void BatteryMonitor::setOverPowerProtection(int power){
   int p=(int)(power*100);
  	sendMessage(bm_address, BM_F_SetOPProt, p);
}
void BatteryMonitor::setOverPowerProtection(float power){
   int p=(int)(power*100);
  	sendMessage(bm_address, BM_F_SetOPProt, p);
}

void BatteryMonitor::setOverTemperatureProtection(int temperature){
   int t=temperature+100;
  	sendMessage(bm_address, BM_F_SetOTProt, t);
}
void BatteryMonitor::setOverTemperatureProtection(float temperature){
   int t=(int)(temperature+100);
  	sendMessage(bm_address, BM_F_SetOTProt, t);
}

void BatteryMonitor::setBatteryCapacity(int capacity){
	int c=capacity*10;
	sendMessage(bm_address, BM_F_SetBattCapa, c);
}
void BatteryMonitor::setBatteryCapacity(float capacity){
	int c=(int)(capacity*10);
	sendMessage(bm_address, BM_F_SetBattCapa, c);
}
void BatteryMonitor::setVoltageCalibration(int calibrationVoltage){
  
}
void BatteryMonitor::setCurrentCalibration(int calibrationCurrent){
  
}
void BatteryMonitor::setTemperatureCalibration(int calibrationTemperature){
  
}
void BatteryMonitor::setRelayType(int relayType){
  
}
void BatteryMonitor::resetFactorySettings(){
  
}
void BatteryMonitor::setCurrentMultiplier(int currentMultiplier){
  
}
void BatteryMonitor::setBatteryPercent(uint8_t batteryPercent){
  
}
void BatteryMonitor::zeroCurrent(){
  
}
void BatteryMonitor::clearAccountingData(){

}

int BatteryMonitor::getUptime(){
  getMeasuredValues();
  return measuredValues.uptime;
}
int BatteryMonitor::getBatteryLifeLeft(){
    getMeasuredValues();
    return measuredValues.batteryLifeLeft;
}
int BatteryMonitor::getTemperature(){
    getMeasuredValues();
    return measuredValues.temperature;
}
   
int BatteryMonitor::getProtectionTemperature(){
  int temp;
  return temp;
}
int BatteryMonitor::getProtectionRecoveryTime(){
  int time;
  return time;
}
int BatteryMonitor::getProtectionDelayTime(){
  int time;
  return time;
}
int BatteryMonitor::getCapacity(){
  //getMeasuredValues();
  //return measuredValues.remainingCapacity;
  return 0;
}
int BatteryMonitor::getVoltageCalibration(){
  int volt;
  return volt;
}
int BatteryMonitor::getCurrentCalibration(){
  int curr;
  return curr;
}
int BatteryMonitor::getTemperatureCalibration(){
  int temp;
  return temp;
}
int BatteryMonitor::getVoltageScale(){
  int scale;
  return scale;
}
int BatteryMonitor::getCurrentScale(){
  int scale;
  return scale;
}

float BatteryMonitor::getVoltage() {
  getMeasuredValues();
  return measuredValues.voltage;
}
float BatteryMonitor::getCurrent(){
  getMeasuredValues();
  return measuredValues.current;
}
float BatteryMonitor::getInternalResistance(){
  getMeasuredValues();
  return measuredValues.internalResistance;
}
float BatteryMonitor::getRemainingCapacity(){
  getMeasuredValues();
  return measuredValues.remainingCapacity;
}
float BatteryMonitor::getCumulativeCapacity(){
  getMeasuredValues();
  return measuredValues.cumulativeCapacity;
}

float BatteryMonitor::getOVPVoltage(){
  float voltage;
  return voltage;
}
float BatteryMonitor::getUVPVoltage(){
  float voltage;
  return voltage;
}
float BatteryMonitor::getOCPForwardCurrent(){
  float curr;
  return curr;
}
float BatteryMonitor::getOCPReverseCurrent(){
  float curr;
  return curr;
}
float BatteryMonitor::getOPPPower(){
  float power;
  return power;
}


void BatteryMonitor::getBasicInfo(){
  // result syntax: :r00=<addr>,<checksum>,<sensor_type:1><voltage:1><amperage:2>,<firmware_version>,<machine_serial_nr>,
  String message,field;
  
  debug("getting basic Info");
   
  sendMessage(bm_address, BM_F_ReadBasicInf, 1);
  message=readMessage();

  debug("get sensor type, max voltage and max current");
  field=getStringField(message,3);
  debug("field:");
  debug(field);
  basicInfo.sensorType=field.substring(0,1).toInt();
  debug("sensorType");
  debug(basicInfo.sensorType);
  basicInfo.maxVoltage=field.substring(1,2).toInt()*100;
  debug("maxVoltage");
  debug(basicInfo.maxVoltage);  
  basicInfo.maxCurrent=field.substring(2).toInt()*10;
  debug("maxCurrent");
  debug(basicInfo.maxCurrent);  
  
  
  debug("get device version");
  basicInfo.deviceVersion=getStringField(message,4).toInt();
  debug("get device serial number");
  basicInfo.deviceSerialNumber=getStringField(message,5).toInt();
}
void BatteryMonitor::getMeasuredValues(){
  String message;
  if(!checkCache()){
	  sendMessage(bm_address, BM_F_ReadMsrdVals, 1);
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
	  debug("get uptime");
	  measuredValues.uptime=getStringField(message,8).toInt();
	  debug("get battery life left");
	  measuredValues.batteryLifeLeft=getStringField(message,13).toInt();
	  debug("get temperature");
	  measuredValues.temperature=getStringField(message,9).toInt()-100;
	
	  debug("get Voltage");
	  measuredValues.voltage=getStringField(message,3).toFloat()/100;
	  measuredValues.current=getStringField(message,4).toFloat()/100;
	  measuredValues.internalResistance=getStringField(message,14).toFloat()/100;
	  measuredValues.remainingCapacity=getStringField(message,5).toFloat()/1000;
	  measuredValues.cumulativeCapacity=getStringField(message,5).toFloat()/1000;
	
	  measuredValues.outputState=getStringField(message,11).toInt();
	  measuredValues.currentDir=getStringField(message,12).toInt();
	  measuredValues.lastReadTime=millis();
	}
}
void BatteryMonitor::getSetValues(){
	String message;	
	sendMessage(bm_address, BM_F_ReadSetVals,1)
  	message=readMessage();
  	/*
  		 1: deviceAddress
  		 2: checksum
  		 3: OVPVoltage
  		 4: UVPVoltage
  		 5: OCPForwardCurrent
  		 6: OCPReverseCurrent
  		 7: OPPPower
  		 8: protectionTemperature
  		 9: protectionRecoveryTime
  		10: protectionDelayTime
  		11: presentCapacity
  		12: voltageCalibration
  		13: currentCalibration
  		14: temperatureCalibration
  		15: reserved
  		16: relayType
  		17: currentMultiple
  		18: voltageScale
  		19: currentScale
  	*/
  	
  	setValues.OVPVoltage					=	getStringField(message, 3).toFloat()/100;
  	setValues.UVPVoltage					=	getStringField(message, 4).toFloat()/100;
  	setValues.OCPForwardCurrent		=	getStringField(message, 5).toFloat/100;
  	setValues.OCPReverseCurrent		=	getStringField(message, 6).toFloat/100;
  	setValues.OPPPower					=	getStringField(message, 7).toFloat()/100;
  	setValues.protectionTemperature	=	getStringField(message, 8).toInt()-100;
  	setValues.protectionRecoveryTime	=	getStringField(message, 9).toInt();
	setValues.protectionDelayTime		=	getStringField(message,10).toInt();
	setValues.presentCapacity			=	getStringField(message,11).
  	
  	setValues.voltageScale				=	getStringField(message.)
  	/*
  	    int 
        deviceAddress,
        checksum,
        protectionTemperature,
        protectionRecoveryTime,
        protectionDelayTime,
        presetCapacity,
        voltageCalibration,
        currentCalibration,
        temperatureCalibration,
        voltageScale,
        currentScale;

    float        
        OVPVoltage,
        UVPVoltage,
        OCPForwardCurrent,
        OCPReverseCurrent,
        OPPPower;

    enum relayType{
        normallyOpen=0,
        normallyClosed=1
   */
}

String BatteryMonitor::getStringField(String message, int idx){
  String field;
  int curr_idx,next_idx;
  next_idx=message.indexOf("=");
  //Serial.println(message);
  for(int i=1;i<=idx;i++){
    if(next_idx!=-1){
      curr_idx=next_idx;
      next_idx=message.indexOf(",",curr_idx+1);
      //Serial.printf("Fieldnum: %i, current idx: %i, next idx: %i\n", i, curr_idx, next_idx);
    }else{
      curr_idx=0;         // setting the indexes to 0 and 1
      next_idx=1;         // will make sure the return string is length 0
      i=idx;              // exit the loop
    }
  }
  field=message.substring(curr_idx+1,next_idx);
  debug("getStringField returns");
  debug(field);
  return field;
}

void BatteryMonitor::sendMessage(int address, int command, int parameter){
  char message[40];
  debug("sendMessage\nrequest:");
  sprintf(message, ":R%02i=%i,%i,%i,\r\n", command, address, checksum(parameter), parameter);
  debug(message);
  bm_serial->print(message);
}

String BatteryMonitor::readMessage(){   
    String _message;
    msgState_t _msgState;
    char c;
    debug("readMessage");
    if (_msgState==crlf){
      //Serial.println(".....new message");
      _message="";
      _msgState=reading;
    }
    
    if (sizeof(_message)==0) Serial.print("[start]");
    int t_start=millis();
    while(!bm_serial->available() && millis()-t_start < SERIAL_TIMEOUT);
    while(bm_serial->available() && _msgState != crlf){
        c=bm_serial->read();
        debug(c);
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
    debug("finished read message\nmessage:");
    debug(_message);
    return _message;
}
void BatteryMonitor::setCacheTime(int cTime){
	cacheTime=cTime;
	}
	
int BatteryMonitor::getCacheTime(){
	return cacheTime;
}

bool BatteryMonitor::checkCache(){
	return(millis()-measuredValues.lastReadTime<cacheTime);
}

void BatteryMonitor::debug(const char msg[]){
    #ifdef DEBUG
    Serial.println(msg);
    #endif
    }

void BatteryMonitor::debug(char msg[]){
    #ifdef DEBUG
    Serial.println(msg);
    #endif
    }
void BatteryMonitor::debug(char c){
    #ifdef DEBUG
    Serial.print(c);
    #endif
    }
void BatteryMonitor::debug(String &msg){
    #ifdef DEBUG
    Serial.println(msg);
    #endif
    }
void BatteryMonitor::debug(int i){
    #ifdef DEBUG
    Serial.println(i);
    #endif
    }

