//BeckLogLib.cpp, 2/16/19
#include <BeckLogLib.h>
#include <BeckNTPLib.h>
#include <NTPClient.h>
#include <Timezone.h>

time_t          _lLocalTime;

//Local prototypes
String  szPrintDigits  (int digits);

//LogToSerial() has multiple versions depending on there being a 2nd variable and its type.
void LogToSerial(String szLogString){
  String szTermString= szLogLineHeader();
  szTermString += szLogString;
  Serial << szTermString << endl;
  return;
} //LogToSerial:empty


void LogToSerial(String szLogString, String szLogValue){
  Serial << LOG0 << szLogString << " " << szLogValue << endl;
  return;
} //LogToSerial:String


void LogToSerial(String szLogString, int sLogValue){
  Serial << LOG0 << szLogString << " " << sLogValue << endl;
  return;
} //LogToSerial:int


void LogToSerial(String szLogString, float fLogValue){
  Serial << LOG0 << szLogString << " " << fLogValue << endl;
  return;
} //LogToSerial:float


String szLogLineHeader(void){
  String          szHeader          = "";
  String          szThousanths      = "";
  unsigned long   ulCurrentMillis   = millis();
  char            szDaysFloat[10];

  //Compute a float with N.NNN with a leading zero representing days uptime
  //Starts out "0.00", 0.01 of a day is 864 seconds or 14.4 minutes
  float   fDays = ((float)ulCurrentMillis / (float)lMsecPerDay);

  szHeader += fDays;
  szHeader += " ";
#if 1
  _lLocalTime= lGetLocalTime();
  szHeader += szFormatTimeString(_lLocalTime);   //szFormatTimeString has a space at the end

  //Replace the space with a decimal point
  //Follow "." with the lowest 3 digits of the msec count
  int wNumChar= szHeader.length();
  szHeader.setCharAt((wNumChar - 1), '.');
  szThousanths= (ulCurrentMillis % lMsecPerSec);

  //Make sure it is 3 digits, pad wit zeros if not
  wNumChar= szThousanths.length();
  switch (wNumChar){
    case 0:
      szThousanths += "000";
      break;
    case 1:
      szThousanths += "00";
      break;
    case 2:
      szThousanths += "0";
      break;
    default:
      break;
  } //switch

  szHeader += szThousanths;
  szHeader += " ";        //Adds a trailing space
#endif
  return szHeader;
} //szLogLineHeader


time_t lGetLocalTime(void){
  //return (oMT_Timezone.toLocal (now(), &pTimeChangeRule));
  return oNTPClient.getEpochTime();
} //lGetLocalTime


String szFormatTimeString(time_t lLocalTime) {
  String szReturnString = "";
  szReturnString += szPrintDigits(hour(lLocalTime));
  szReturnString += ":";
  szReturnString += szPrintDigits(minute(lLocalTime));
  szReturnString += ":";
  szReturnString += szPrintDigits(second(lLocalTime));
  szReturnString += " ";
  return szReturnString;
} //szFormatTimeString


String szFormatDateString(time_t lLocalTime){
  String szReturnString = "";
  szReturnString += szPrintDigits(month(lLocalTime));
  szReturnString += "/";
  szReturnString += szPrintDigits(day(lLocalTime));
  szReturnString += "/";
  szReturnString += String(year(lLocalTime));
  return szReturnString;
} //szFormatDateString


String szPrintDigits(int digits) {
  // utility for digital clock display: prints preceding colon and leading 0
  String digStr = "";

  if (digits < 10)
    digStr += '0';
  digStr += String(digits);

  return digStr;
} //szPrintDigits


//szAddLeadingZeros() adds 1 or 2 zeros (depending on sNumDigits being 3 or not).
String szAddZeros(int sValue, int sNumDigits){
  String szReturn;
  if ((sNumDigits == 3) && (sValue < 100)){
    szReturn= "0";
  } //if((sNumDigits==3)&&(sValue<100)
  if (sValue < 10){
    szReturn += "0";
  } //if(lValue<10)
  szReturn += String(sValue);
  return szReturn;
} //szAddZeros
//Last line.
