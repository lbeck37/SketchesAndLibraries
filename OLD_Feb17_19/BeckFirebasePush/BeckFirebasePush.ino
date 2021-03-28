// 4/23/16 Was FirebasePush_ESP8266.ino example
// Maybe not fully working, used this with printouts until I determined it was the
// stringing of Firebase() ctor and auth() call together.
static const char acSketchName[]  = "BeckFirebasePush.ino";
static const char acFileDate[]    = "Apr 24, 2016F";

// FirebasePush_ESP8266 is a sample that push a new timestamp to firebase
// on each reset.

#include <BeckLib.h>
#include <Firebase.h>

static const char   	acRouterName[]      = "Aspot24";
static const char   	acRouterPW[]        = "Qazqaz11";
static const String 	acDatabaseURL		= "intense-fire-3958.firebaseio.com";
static const String 	acFirebaseSecret	= "LhXHxFsUn7SVYoRC82dKKSqqD67Ls9nfdtMBAWUe";
String	szLogLine;
// create firebase client.
//Firebase fbase = Firebase("example.firebaseio.com").auth("secret_or_token");
//Firebase fbase = Firebase("intense-fire-3958.firebaseio.com").auth("LhXHxFsUn7SVYoRC82dKKSqqD67Ls9nfdtMBAWUe");
//Firebase fbase = Firebase(acDatabaseURL).auth(acFirebaseSecret);

void setup() {
  Serial.begin(115200);
  Serial << endl;
  Serial << LOG0 << " Setup() Begin " << acSketchName << ", version: " << acFileDate << endl;

  // connect to wifi.
  //WiFi.begin("SSID", "PASSWORD");
  Serial << LOG0 << " Call WiFi.begin(" << acRouterName << ", " << acRouterPW << ")" << endl;
  WiFi.begin(acRouterName, acRouterPW);

  Serial << LOG0 << " Call WiFi.waitForConnectResult()" << endl;
  while(WiFi.waitForConnectResult() != WL_CONNECTED){
	Serial << LOG0 << " WiFi failed, retrying." << endl;
	Serial << LOG0 << " Call WiFi.begin(" << acRouterName << ", " << acRouterPW << ")" << endl;
    WiFi.begin(acRouterName, acRouterPW);
   }

  Serial << LOG0 << " My WiFi IP address= " << WiFi.localIP() << endl;

  Serial << LOG0 << " Setup() Call  Firebase(acDatabaseURL).auth(acFirebaseSecret)" << endl;
  Serial << LOG0 << "   acDatabaseURL   = " << acDatabaseURL << endl;
  Serial << LOG0 << "   acFirebaseSecret= " << acFirebaseSecret << endl;
  //Creation and call to auth() need to be separate, return value from auth() is not same Firebase object.
  Firebase fbase = Firebase(acDatabaseURL);
  fbase.auth(acFirebaseSecret);
  fbase.PrintPrivates();

   // add a new entry.
  FirebasePush push = fbase.push("/logs", "{\".sv\": \"timestamp\"}");
  if (push.error()) {
      //Serial.println("Firebase push failed");
      //Serial.println(push.error().message());
      Serial << LOG0 << " Setup() fbase.push() failed, error= " << push.error().message() << endl;
   }

  // print key.
  //Serial.println(push.name());
  Serial << LOG0 << " push.name()= " << push.name() << endl;

  fbase.PrintPrivates();
  Serial << LOG0 << " Setup(): Call fbase.get(\"/logs\")" << endl;
  // get all entries.
  FirebaseGet get = fbase.get("/logs");
  if (get.error()) {
      //Serial.println("Firebase get failed");
      //Serial.println(get.error().message());
      Serial << LOG0 << " Setup() fbase.get() failed, error= " << get.error().message() << endl;
  }
  // print json.
  //Serial.println(get.json());
  Serial << LOG0 << " Setup() Call get.json()= " << get.json() << endl;
  Serial << LOG0 << " Setup() Done " << acSketchName << ", version: " << acFileDate << endl;
}

void loop() {
}
