#ifndef _BATTERYMONITORH_
#define _BATTERYMONITORH_

#ifndef Stream_h
#include <Stream.h>
#endif 

#ifndef HardwareSerial_h
#include <HardwareSerial.h>
#endif

#define MAXDEVS 4         // max number of battery monitor devices to be supported

#define DEBUG

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
#define CACHE_TIME			100 // cache time. If you request a measured value 
										 // and more than CACHE_TIME milliseconds have elapsed
										 // since that value was last read, the library will 
										 // automatically re-read all values. Reading all messages
										 // takes roughly 50ms, so sampling more than 20 times per
										 // second is pointless
										 // the actual cache time can be set through the setCacheTime(millis) 
										 // function. A higher cache time would be useful to make sure 
										 // consistent values are being read even if the actual calls to
										 // get the different values are spaced farther apart.

#define BM_F_SetAddress   01
#define BM_F_TurnOnOutput 10
#define BM_F_SetOVProt    20
#define BM_F_SetUVProt    21
#define BM_F_SetPOCProt   22
#define BM_F_SetNOCProt   23
#define BM_F_SetOPProt    24
#define BM_F_SetOTProt    25
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
        currentDir,
        lastReadTime;

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
        currentScale,
        relayType;

    float        
        OVPVoltage,
        UVPVoltage,
        OCPForwardCurrent,
        OCPReverseCurrent,
        OPPPower;
	 /*
    enum relayType{
        normallyOpen=0,
        normallyClosed=1

    };
    */
}setValues_t;

class BatteryMonitor{
  public:
  
  BatteryMonitor();
  //BatteryMonitor(int address, HardwareSerial &SerialDevice);
  ~BatteryMonitor();
  void
    getBasicInfo(),
    getMeasuredValues(),
    getSetValues(),
  	 begin(int address, Stream &SerialDevice),
    setNewAddress(uint8_t newAddress),
    setOutput(bool output),
    setOverVoltageProtection(int voltage),
    setUnderVoltageProtection(int voltage),
    setPositiveOverCurrentProtection(int current),
    setNegativeOverCurrentProtection(int current),
    setOverPowerProtection(int power),
    setOverTemperatureProtection(int temperature),
    
    setOverVoltageProtection(float voltage),
    setUnderVoltageProtection(float voltage),
    setPositiveOverCurrentProtection(float current),
    setNegativeOverCurrentProtection(float current),
    setOverPowerProtection(float power),
    setOverTemperatureProtection(float temperature),

    setBatteryCapacity(int capacity),
    setVoltageCalibration(int calibrationVoltage),
    setCurrentCalibration(int calibrationCurrent),
    setTemperatureCalibration(int calibrationTemperature),
	 
	 setBatteryCapacity(float capacity),
	 setVoltageCalibration(float calibrationVoltage),
    setCurrentCalibration(float calibrationCurrent),
    setTemperatureCalibration(float calibrationTemperature),
    //setPresetCapacity(int capacity,)
    setRelayType(int relayType),
    resetFactorySettings(),
    setCurrentMultiplier(int currentMultiplier),
    setBatteryPercent(int batteryPercent),
    zeroCurrent(),
    clearAccountingData(),
    setCacheTime(int);

  int 
    getUptime(),
    getBatteryLifeLeft(),
    getTemperature(),
   
    getProtectionTemperature(),
    getProtectionRecoveryTime(),
    getProtectionDelayTime(),
    getPresetCapacity(),
    getVoltageCalibration(),
    getCurrentCalibration(),
    getTemperatureCalibration(),
    getVoltageScale(),
    getCurrentScale(),
    getCacheTime(),
    getRelayType();

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

    sendMessage(int address, int command, int parameter),
    debug(const char msg[]),
    debug(char msg[]),
    debug(char c),
    debug(String &msg),
    debug(int i);
  bool
  	 checkCache();

  Stream 			  *bm_serial;
  setValues_t       setValues;
  measuredValues_t  measuredValues;
  basicInfo_t       basicInfo;
  int               bm_address, cacheTime;
  //Stream            &bm_serial;
};

#endif
