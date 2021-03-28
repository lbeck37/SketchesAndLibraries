const char szSketchName[]  = "Beck_FirebaseBasic.ino";
const char szFileDate[]    = "8/26/19z";

#include <FirebaseESP8266.h>
#include <FirebaseESP8266HTTPClient.h>
#include <FirebaseJson.h>
#include <jsmn.h>
/*
 * Created by K. Suwatchai (Mobizt)
 * Email: k_suwatchai@hotmail.com
 * Github: https://github.com/mobizt
 * Copyright (c) 2019 mobizt
*/
//FirebaseESP8266.h must be included before ESP8266WiFi.h
//#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <Streaming.h>

#define FIREBASE_HOST "fir-mariestep1.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyBVOUnaaCjuoFWLsiyVi7ZSW_PsiHLam1A"
#define WIFI_SSID "Aspot24"
#define WIFI_PASSWORD "Qazqaz11"

//Define FirebaseESP8266 data object
FirebaseData firebaseData;

void printJsonObjectContent(FirebaseData &data);

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial << endl << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial << "Call Firebase.begin()" << endl;
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Serial << "Call Firebase.reconnectWiFi()" << endl;
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  //Serial << "Call  " << endl;
  Serial << "Call Firebase.setReadTimeout()" << endl;
  Firebase.setReadTimeout(firebaseData, 1000 * 60);

  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Serial << "Call Firebase.setwriteSizeLimit()" << endl;
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  
  /*
  This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
  Firewall that allows only GET and POST requests.
  
  Firebase.enableClassicRequest(firebaseData, true);
  */

  String path = "/ESP8266_Test";

  FirebaseJson json;
 
  Serial.println("------------------------------------");
  Serial.println("Set double test...");

  for (uint8_t i = 0; i < 10; i++){
    Serial << "setup(): Call Firebase.setDouble(), i= " << i << endl;
    if (Firebase.setDouble(firebaseData, path + "/Double/Data" + (i + 1), ((i + 1) * 10) + 0.123456789)){
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      Serial.println("ETag: " + firebaseData.ETag());
      Serial.print("VALUE: ");
      if (firebaseData.dataType() == "int")
        Serial.println(firebaseData.intData());
      else if (firebaseData.dataType() == "float")
        Serial.println(firebaseData.floatData(), 5);
      else if (firebaseData.dataType() == "double")
        printf("%.9lf\n", firebaseData.doubleData());
      else if (firebaseData.dataType() == "boolean")
        Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
      else if (firebaseData.dataType() == "string")
        Serial.println(firebaseData.stringData());
      else if (firebaseData.dataType() == "json")
        printJsonObjectContent(firebaseData);
      Serial.println("------------------------------------");
      Serial.println();
    }
    else {
      //Serial.println("FAILED");
      Serial << "setup(): setDouble() failed, call errorReason() to find out why" << endl;
      String szErrorReason= firebaseData.errorReason();
      Serial << "setup(): szErrorReason= " << szErrorReason << endl;
      //Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }

  Serial.println("------------------------------------");
  Serial.println("Get double test...");

  for (uint8_t i = 0; i < 10; i++){
    if (Firebase.getInt(firebaseData, path + "/Double/Data" + (i + 1))){
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      Serial.println("ETag: " + firebaseData.ETag());
      Serial.print("VALUE: ");
      if (firebaseData.dataType() == "int")
        Serial.println(firebaseData.intData());
      else if (firebaseData.dataType() == "float")
        Serial.println(firebaseData.floatData(), 5);
      else if (firebaseData.dataType() == "double")
        printf("%.9lf\n", firebaseData.doubleData());
      else if (firebaseData.dataType() == "boolean")
        Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
      else if (firebaseData.dataType() == "string")
        Serial.println(firebaseData.stringData());
      else if (firebaseData.dataType() == "json")
        printJsonObjectContent(firebaseData);
      Serial.println("------------------------------------");
      Serial.println();
    }
    else {
      Serial.println("setup(): FAILED");
      Serial.println("setup(): REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }

  Serial.println("------------------------------------");
  Serial.println("Push integer test...");

  for (uint8_t i = 0; i < 5; i++) {
    if (Firebase.pushInt(firebaseData, path + "/Push/Int", (i + 1))) {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.print("PUSH NAME: ");
      Serial.println(firebaseData.pushName());
      Serial.println("ETag: " + firebaseData.ETag());
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }

  Serial.println("------------------------------------");
  Serial.println("Push JSON test...");

  for (uint8_t i = 5; i < 10; i++) {
    json.clear().addInt("Data" + String(i + 1),i + 1);

    if (Firebase.pushJSON(firebaseData, path + "/Push/Int", json)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.print("PUSH NAME: ");
      Serial.println(firebaseData.pushName());
      Serial.println("ETag: " + firebaseData.ETag());
      Serial.println("------------------------------------");
      Serial.println();
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }

  Serial.println("------------------------------------");
  Serial.println("Update test...");

  for (uint8_t i = 0; i < 5; i++) {
    json.clear().addDouble("Data" + String(i + 1),i + 5.5);

    if (Firebase.updateNode(firebaseData, path + "/float", json))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      //No ETag available
      Serial.print("VALUE: ");
      if (firebaseData.dataType() == "int")
        Serial.println(firebaseData.intData());
      else if (firebaseData.dataType() == "float")
        Serial.println(firebaseData.floatData(), 5);
      else if (firebaseData.dataType() == "double")
        printf("%.9lf\n", firebaseData.doubleData());
      else if (firebaseData.dataType() == "boolean")
        Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
      else if (firebaseData.dataType() == "string")
        Serial.println(firebaseData.stringData());
      else if (firebaseData.dataType() == "json")
        printJsonObjectContent(firebaseData);
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
  }
} //setup


void loop()
{
} //loop


void printJsonObjectContent(FirebaseData &data){
  size_t tokenCount = data.jsonObject().parse(false).getJsonObjectIteratorCount();
  String key;
  String value;
  FirebaseJsonObject jsonParseResult;
  Serial.println();
  for (size_t i = 0; i < tokenCount; i++)
  {
    data.jsonObject().jsonObjectiterator(i,key,value);
    jsonParseResult = data.jsonObject().parseResult();
    Serial.print("KEY: ");
    Serial.print(key);
    Serial.print(", ");
    Serial.print("VALUE: ");
    Serial.print(value); 
    Serial.print(", ");
    Serial.print("TYPE: ");
    Serial.println(jsonParseResult.type);        
  }
}
//Last line.
