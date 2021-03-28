//BeckMiniLib.cpp, Beck 2/17/19a
#include <BeckMiniLib.h>
#include <BeckLogLib.h>

const long    lMsecPerSec         = 1000;
const long    lMsecPerMin         = 60 * lMsecPerSec;
const long    lMsecPerHour        = 60 * lMsecPerMin;
const long    lMsecPerDay         = 24 * lMsecPerHour;
const long    lSerialMonitorBaud  = 115200;

//Digital Pins
#ifdef ESP32
  //BlynkBeck uses pins 4, 5, 15, 16
  //static const int       sSelectButton;
  //static const int       sBacklightPin;
  const int       sUpButtonPin       =  0;
  const int       sDownButtonPin     =  2;
  const byte      cI2C_SDA_Pin       =  26;
  const byte      cI2C_SCL_Pin       =  27;
  const byte      cSPI_A0CmdData_Pin =  4;
  const byte      cSPI_MISO_Pin      = 12;
  const byte      cSPI_MOSI_Pin      = 13;
  const byte      cSPI_CLK_Pin       = 14;
  const byte      cSPI_Select_Pin    = 15;
  const int       sServoPin          = 16;
#endif  //ESP32

unsigned long   ulLastTaskMsec        = 0;      //For checking time handling tasks

//Local function prototypes
String  szPrintDigits         (int digits);


void CheckTaskTime(String szTask){
  unsigned long    ulMaxTaskMsec= lMsecPerSec / 2;  //Half second time limit before reporting task.
  unsigned long    ulNowMsec= millis();
  unsigned long    ulTaskMsec= ulNowMsec - ulLastTaskMsec;
  if (ulTaskMsec >  ulMaxTaskMsec){
    float fTaskSeconds= (float)ulTaskMsec / 1000.0;
    Serial << LOG0 << "CheckTaskTime(): " << szTask << " task took " << fTaskSeconds << " seconds"<< endl;
  } //
  ulLastTaskMsec= millis();
  return;
} //CheckTaskTime


void ClearTaskTime2(unsigned long* pulLastTaskMsec){
  if (pulLastTaskMsec){
    *pulLastTaskMsec= millis();
  }
  else{
    Serial << LOG0 << "ClearTaskTime2(): ERROR: Passed in NULL pointer" << endl;
  }
  return;
} //ClearTaskTime2


void CheckTaskTime2(String szTask, unsigned long* pulLastTaskMsec){
  unsigned long    ulMaxTaskMsec= lMsecPerSec / 2;  //Half second time limit before reporting task.
  unsigned long    ulNowMsec= millis();
  if (pulLastTaskMsec == NULL){
    pulLastTaskMsec= &ulLastTaskMsec;
  } //if (plLastTaskMsec==NULL)
  unsigned long    ulTaskMsec= ulNowMsec - *pulLastTaskMsec;

  if (ulTaskMsec >  ulMaxTaskMsec){
    float fTaskSeconds= (float)ulTaskMsec / 1000.0;
    Serial << LOG0 << "CheckTaskTime2(): " << szTask << " task took " << fTaskSeconds << " seconds"<< endl;
  } //
  *pulLastTaskMsec= millis();
  return;
} //CheckTaskTime2
//Last line.
