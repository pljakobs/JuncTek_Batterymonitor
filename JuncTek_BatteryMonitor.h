#ifndef _BATTERYMONITORH_
#define _BATTERYMONITORH_

#ifndef Stream_h
#include <Stream.h>
#endif 

#ifndef HardwareSerial_h
#include <HardwareSerial.h>
#endif

#define MAXDEVS 4         // max number of battery monitor devices to be supported

//#define DEBUG

/* Format:
 *  
 *  SOM           :
 *  Function Code W/R
 *  Function Nr   00-99
 *  Connector     =
 *  Address       1-99
 *  Spacer        ,
 *  Checksum      1-255
 *  data          depending on function, comma separated
 *  
 */
 
#define SERIAL_TIMEOUT	  1000 // timeout to wait for a return message in ms

#define BM_F_SetAddress   01
#define BM_F_TurnOnOutput 10
#define BM_F_SetOVProt    20
#define BM_F_SetUVProt    21
#define BM_F_SetOCProt    22
#define BM_F_SetUCProt    23
#define BM_F_SetOPProt    24
#define BM_F_SetUPProt    25
#define BM_F_SetBattCapa  28
#define BM_F_SetVoltCalbr 29
#define BM_F_SetCurrCalbr 30
#define BM_F_SetTempCalbr 31
#define BM_F_SetRelayType 34
#define BM_F_ResumeFctSettings 36
#define BM_F_SetCurrMult  36
#define BM_F_SetBattPerc  60
#define BM_F_ZeroCurrent  61
#define BM_F_ClearAccData 62

#define BM_F_ReadBasicInf 00
#define BM_F_ReadMsrdVals 50
#define BM_F_ReadSetVals  51

#define checksum(a) ((a%255)+1)

typedef enum {
        reading,
        cr,
        crlf
}msgState_t;

typedef  struct{
    int 
        deviceAddress,
        checksum,
        maxVoltage,
        maxCurrent,
		  sensorType;
    /*
    *  enum sensorType{
    *    HALLSENSOR = 1,
    *    SAMPLER=2
    *};
    */
    int deviceVersion;
    int deviceSerialNumber;
} basicInfo_t;


typedef struct {
    int 
        deviceAddress,
        checksum,    
        uptime,
        batteryLifeLeft,
        temperature,
        outputState,
        currentDir;

    float
        voltage,
        current,
        internalResistance,
        remainingCapacity,
        cumulativeCapacity;
	/*
   * enum outputState{
   *     ON = 0,
   *     OVP = 1,
   *     OCP = 2,
   *     LVP = 3,
   *     NCP = 4,
   *     OPP = 5,
   *     OTP = 6,
   *     OFF =255
   * };
   *
	*
   * enum currentDir{
   *     discharging = 0,
   *     charging =1
   * };
   */
}measuredValues_t;

typedef struct{
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
    };
}setValues_t;

class BatteryMonitor{
  public:
  
  BatteryMonitor(int address, Stream &SerialDevice);
  //BatteryMonitor(int address, HardwareSerial &SerialDevice);
  ~BatteryMonitor();
  void
    setAddress(uint8_t newAddress),
    setOutoupt(bool output),
    setOverVoltageProtection(int voltage),
    setUnderVoltageProtection(int voltage),
    setOverCurrentProtection(int current),
    setUnderCurrentProtection(int current),
    setOverPowerProtection(int power),
    setUnderPowerProtection(int power),
    setBatteryCapacity(int capacity),
    setVoltageCalibration(int calibrationVoltage),
    setCurrentCalibration(int calibrationCurrent),
    setTemperatureCalibration(int calibrationTemperature),
    setRelayType(int relayType),
    resetFactorySettings(),
    setCurrentMultiplier(int currentMultiplier),
    setBatteryPercent(uint8_t batteryPercent),
    zeroCurrent(),
    clearAccountingData();

  int 
    getUptime(),
    getBatteryLifeLeft(),
    getTemperature(),
   
    getProtectionTemperature(),
    getProtectionRecoveryTime(),
    getProtectionDelayTime(),
    getCapacity(),
    getVoltageCalibration(),
    getCurrentCalibration(),
    getTemperatureCalibration(),
    getVoltageScale(),
    getCurrentScale();

  float
    getVoltage(),
    getCurrent(),
    getInternalResistance(),
    getRemainingCapacity(),
    getCumulativeCapacity(),

    getOVPVoltage(),
    getUVPVoltage(),
    getOCPForwardCurrent(),
    getOCPReverseCurrent(),
    getOPPPower();

  private:
  String 
      getStringField(String message, int idx),
      readMessage();
  void
    getBasicInfo(),
    getMeasuredValues(),
    getSetValues(),
    sendMessage(int address, int command, int parameter),
    debug(const char msg[]),
    debug(char msg[]),
    debug(char c),
    debug(String &msg),
    debug(int i);

  Stream 			  *bm_serial;
  setValues_t       setValues;
  measuredValues_t  measuredValues;
  basicInfo_t       basicInfo;
  int               bm_address;
  //Stream            &bm_serial;
};

#endif
