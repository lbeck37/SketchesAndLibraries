//BeckFirebase.cpp, September 18, 2016
#include <BeckFirebase.h>

static const String     		sDatabaseURL   		= "intense-fire-3958.firebaseio.com";
//static const String     		sDatabaseURL   		= "intense-fire-37.firebaseio.com";
static const String     		sFirebaseSecret  	= "LhXHxFsUn7SVYoRC82dKKSqqD67Ls9nfdtMBAWUe";
static const String         	sLogPath      		= "/Logs/";
static const char           	acMyFbaseName[]     = "BeckESP8266Base_3dotESP";

//static const char           	acMyFbaseName[]     = "BeckESP8266Base_1dotESP";
//static const char           	acMyFbaseName[]     = "BeckESP8266Base_2dotESP";//Has WiFi problem.
//static const char           	acMyFbaseName[]     = "Powershift_3dotESP";
//static const char           	acMyFbaseName[]     = "BeckESP8266Base_CloneNodeMCU";

BeckFirebase*			      pBeckFBase;

BeckFirebase::BeckFirebase(String sDatabaseURL,String sFirebaseSecret,
		                   String sLogPath, String sMyName){
	sDatabaseURL_		= sDatabaseURL;
	sFirebaseSecret_	= sFirebaseSecret;
	sLogPath_			= sLogPath;
	sMyName_			= sMyName;
	sPushPath_			= sLogPath_ + sMyName_;

	BLogS("BeckFirebase() cstor: Call new Firebase(" + sDatabaseURL_ + ")");
	pFBase_= new Firebase(sDatabaseURL_);

	BLogS("BeckFirebase() cstor: Call Firebase.auth(" + sFirebaseSecret_ + ")");
	pFBase_->auth(sFirebaseSecret_);

	BLogS("BeckFirebase() cstor: Firebase client created to " + sDatabaseURL_);
	return;
}	//BeckFirebase cstor


void BeckFirebase::LogToSerial(String sLogline){
	Serial << sLogline << endl;
	return;
}	//LogToSerial


bool BeckFirebase::LogToFirebase(String sLogline){
	if (bFirebaseOk_) {
		String sJSONPushString= sMakeJSONObject("Log", sLogline);
		FirebasePush push = pFBase_->push(sPushPath_, sJSONPushString);
		if (push.error()) {
			bFirebaseOk_= false;
			BLogS("LogToFirebase(): Firebase push failed, Error: " + String(push.error().message()));
		}	//if(push.error())
	}	//if(bFirebaseOk_)
	else
	{
		//BLogS("LogToFirebase(): bFirebaseOk_ is false, skipped write to Firebase");
	}	//if(bFirebaseOk_)else
	return bFirebaseOk_;
}	//LogToFirebase


void BeckFirebase::LogToBoth(String sLogline){
  //Put a "*" in front of log line if not logging to Firebase
  if (!bFirebaseOk_) {
    sLogline= "*" + sLogline;
  } //if(bFirebaseOk_)else

  String sFullLogline=LOG0 + sLogline;
	LogToSerial  (sFullLogline);
#ifndef NO_FIREBASE
	LogToFirebase(sFullLogline);
#endif
	return;
}	//LogToBoth


String BeckFirebase::GetDatabaseURL(void){
	return sDatabaseURL_;
}	//GetDatabaseURL


String BeckFirebase::GetLogPath(void){
	return sLogPath_;
}	//GetLogPath


String BeckFirebase::GetPushPath(void){
	return sPushPath_;
}	//GetPushPath


String BeckFirebase::sMakeJSONObject(String sName, String sValue){
  String szJSONObject= "{\"";
  szJSONObject += sName;
  szJSONObject += "\": \"";
  szJSONObject += sValue;
  szJSONObject += "\"}";
  return szJSONObject;
} //sMakeJSONObject
//***End of BeckFirebase class methods***


BeckFirebase* StartBeckFirebase(String sDatabaseURL, String sFirebaseSecret, String sLogPath, String sMyName){
	BLogS("BeckFirebase() cstor: Call new Firebase(" + sDatabaseURL + ")");
	BeckFirebase* pBeckFirebase= new BeckFirebase(sDatabaseURL, sFirebaseSecret, sLogPath, sMyName);
	pBeckFBase= pBeckFirebase;
	if (bWiFiOn) {
	  TestFirebase();
	} //if(bWiFiOn)
	else {
    pBeckFBase->bFirebaseOk_= false;
  } //if(bWiFiOn)else
	return(pBeckFBase);
}	//StartBeckFirebase


bool TestFirebase(void){
	BLogS("TestFirebase(): Make test call to Firebase.");
	pBeckFBase->bFirebaseOk_= true;
	if (!BLogF("BeckFirebase() Test")) {
		BLogS("BeckFirebase(): Test to Firebase failed, setting bFirebaseOk_ to false");
		pBeckFBase->bFirebaseOk_= false;
	}	//if(!BLogF("BeckFirebase()Test"))

	return pBeckFBase->bFirebaseOk_;
}	//TestFirebase


void SendInfoToLog(void){
	BLog("SendInfoToLog(): GetDatabaseURL()= |" + pBeckFBase->GetDatabaseURL() + "|");
	BLog("SendInfoToLog(): GetLogPath()= |" + pBeckFBase->GetLogPath() + "|");
	BLog("SendInfoToLog(): GetPushPath()= |" + pBeckFBase->GetPushPath() + "|");

	LogESPValues();
	return;
} //SendInfoToLog
