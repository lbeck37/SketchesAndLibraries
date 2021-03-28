const char szSketchName[]  = "BeckE32_BLE_Client.ino";
const char szFileDate[]    = "Apr 26, 2018-m";
/**
 * A BLE client example that is rich in capabilities.
 */

#include "BLEDevice.h"
//#include "BLEScan.h"
#include <Streaming.h>

/*
// The remote service we wish to connect to.
static BLEUUID serviceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("0d563a58-196a-48ce-ace2-dfec78acc814");
*/

/*
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
*/
#define SERVICE_UUID          "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define NOTIFY_CHARACT_UUID   "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// The remote service we wish to connect to.
static BLEUUID    serviceUUID(SERVICE_UUID);

// The characteristic of the remote service we are interested in.
//static BLEUUID    charUUID(CHARACTERISTIC_UUID_TX);
static BLEUUID    oNotifyCharactUUID(NOTIFY_CHARACT_UUID);

static BLEAddress*                pServerAddress;
static BLERemoteCharacteristic*   pRemoteCharacteristic;
static boolean doConnect = false;
static boolean connected = false;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
/*
	Serial << "notifyCallback(): Characteristic UUID= "
			   << pBLERemoteCharacteristic->getUUID().toString().c_str()
				 << ", Data length= " << length << endl;
*/
	//Terminate the string.
	pData[length]= '\0';
	Serial << "notifyCallback(): Data= " << (char*)pData << endl;
	return;
} //notifyCallback


bool connectToServer(BLEAddress pAddress) {
    Serial.print("connectToServer(): Forming a connection to ");
    Serial.println(pAddress.toString().c_str());

    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println("connectToServer(): Created client");

    // Connect to the remove BLE Server.
    pClient->connect(pAddress);
    Serial.println("connectToServer(): Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("connectToServer(): Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      return false;
    }
    Serial.println("connectToServer(): Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(oNotifyCharactUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("connectToServer(): Failed to find our Notify characteristic UUID: ");
      Serial.println(oNotifyCharactUUID.toString().c_str());
      return false;
    } //if(pRemoteCharacteristic==nullptr)
    else {
      Serial.println("connectToServer(): Found our characteristic");

      // Read the value of the characteristic.
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("connectToServer(): The characteristic value was: ");
      Serial.println(value.c_str());

      Serial.println("connectToServer(): Calling pRemoteCharacteristic->registerForNotify()");
      pRemoteCharacteristic->registerForNotify(notifyCallback);
      //Fix from Internet
      const uint8_t v[]={0x1,0x0};
      pRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)v,2,true);
    } //else if(pRemoteCharacteristic==nullptr)
    return true;
} //connectToServer


//Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
  //Called for each advertising BLE server.
   Serial << "MyAdvertisedDeviceCallbacks: onResult(): Found: |" << advertisedDevice.toString().c_str()  << "|" << endl;

  //We have found a device, let us now see if it contains the service we are looking for.
  if ((advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID)) ||
      ( strstr(advertisedDevice.getName().c_str(), "RightPedal_BeckE32"))){
    Serial << "MyAdvertisedDeviceCallbacks: onResult(): Found match, address: " << advertisedDevice.getAddress().toString().c_str() << endl;
    Serial << "MyAdvertisedDeviceCallbacks: onResult(): " << szSketchName << ", " << szFileDate << endl;
    advertisedDevice.getScan()->stop();
    pServerAddress= new BLEAddress(advertisedDevice.getAddress());
    doConnect= true;
    } //if
  } //onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(115200);
  Serial << endl << endl << endl << endl;
  Serial << "setup(): First Line 37 " << endl;
  Serial << "setup(): Begin " << szSketchName << ", " << szFileDate << endl;
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan= BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  //pBLEScan->start(30);    //scan to run for 30 seconds
  pBLEScan->start(10);
} // End of setup.


void loop() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  //if (!connected && (doConnect == true)) {
  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      //Serial.println("loop(): We are now connected to the BLE Server.");
      connected= true;
    } //if(connectToServer((*pServerAddress))
    else {
      Serial.println("loop(): We have failed to connect to the server; there is nothing more we will do.");
    } //else  //if(connectToServer((*pServerAddress))
    doConnect= false;
  } //if(doConnect==true)

/*
  String szTime= "Seconds since boot= " + String(millis()/1000);
  if (connected){

    // If we are connected to a peer BLE Server, update the characteristic each time we are reached
    // with the current time since boot.
    String newValue= "Time since boot: " + String(millis()/1000);
    Serial.println("loop(): Setting new characteristic value to \"" + newValue + "\"");

    // Set the characteristic's value to be the array of bytes that is actually a string.
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());

    Serial << "loop(): Connected, " << szTime << endl;
  } //if(connected)
  else{
    Serial << "loop(): Not Connected, " << szTime << endl;
  }
  delay(2000); // Delay a second between loops.
*/
  delay(50);
  return;
} //loop
//Last line.
