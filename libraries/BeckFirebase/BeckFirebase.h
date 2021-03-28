// BeckFirebase.h, 9/18/16
#include <BeckLib.h>

#define BLogF		    pBeckFBase->LogToFirebase

class BeckFirebase {
public:
	BeckFirebase(String sDatabaseURL,String sFirebaseSecret, String sPushPath, String sMyName);

	BeckFirebase() {};
	~BeckFirebase() {};

	void 		    LogToSerial(String sLogline);
	bool		    LogToFirebase(String sLogline);
	void		    LogToBoth(String sLogline);
	String		  GetDatabaseURL(void);
	String		  GetLogPath(void);
	String		  GetPushPath(void);

	bool		    bFirebaseOk_= true;

protected:
	String		  sDatabaseURL_;
	String		  sFirebaseSecret_;
	//Firebase 	oFBase_;
	Firebase* 	pFBase_;
	String		  sLogPath_;
	String 		  sMyName_;
	String		  sPushPath_;

	String		sMakeJSONObject(String sName, String sValue);
};	//BeckFirebase

//Non-class function protos.
BeckFirebase*	StartBeckFirebase(String sDatabaseURL, String sFirebaseSecret, String sLogPath, String sMyName);
bool 			TestFirebase(void);
void			SendInfoToLog(void);

extern BeckFirebase*			      pBeckFBase;
