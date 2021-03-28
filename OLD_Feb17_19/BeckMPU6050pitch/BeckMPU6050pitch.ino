String	acSketchName	= "BeckMPU6050pitch.ino, ";
String	acFileDate		= "Aug 5, 2017, Ace-E";

#include <BeckLogLib.h>
//#include <Streaming.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

MPU6050 		mpu;
uint16_t 		packetSize;
uint16_t 		fifoCount;
uint8_t 		fifoBuffer[64];
Quaternion 	q;
VectorFloat	gravity;
float 			ypr[3];

const unsigned long	ulGyroSpacingMsec		= 500;
unsigned long				ulNextGyroMsec			= 0;

void setup() {
  Serial.begin(115200);
	//Serial << endl << "setup(): Begin " << acSketchName << acFileDate << endl;
	BLog("setup(): Begin " + acSketchName + acFileDate);
	Wire.begin();
	TWBR = 24;
	BLog("setup(): Call mpu.initialize()");
	mpu.initialize();
	BLog("setup(): After initialize(), fifoCount= ", mpu.getFIFOCount());

	BLog("setup(): Call mpu.dmpInitialize()");
	mpu.dmpInitialize();
	BLog("setup(): After dmpInitialize(), fifoCount= ", mpu.getFIFOCount());

	mpu.setXAccelOffset(-1343);
	mpu.setYAccelOffset(-1155);
	mpu.setZAccelOffset(1033);
	mpu.setXGyroOffset(19);
	mpu.setYGyroOffset(-27);
	mpu.setZGyroOffset(16);
	//mpu.setDMPEnabled(true);
	packetSize= mpu.dmpGetFIFOPacketSize();
	BLog("setup(): packetSize= ", packetSize);
	fifoCount = mpu.getFIFOCount();
	BLog("setup(): fifoCount= ", fifoCount);
	mpu.setDMPEnabled(true);
}	//setup


void loop() {
	GyroLoop();
	return;
}	//loop


void GyroLoop(){
	unsigned long ulCurrentMsec= millis();
	if (false || (ulCurrentMsec > ulNextGyroMsec)) {
		BLog("GyroLoop(): It's time again");
		ulNextGyroMsec= ulCurrentMsec + ulGyroSpacingMsec;
		while (fifoCount < packetSize) {
			//insert here your code
			fifoCount= mpu.getFIFOCount();
		}	//while
		BLog("GyroLoop(): After while(), fifoCount= ", fifoCount);

		if (fifoCount == 1024) {
			BLog("GyroLoop(): Call mpu.resetFIFO()");
			mpu.resetFIFO();
			BLog("GyroLoop(): After resetFIFO(), FIFOCount= ", mpu.getFIFOCount());
			Serial.println(F("FIFO overflow!"));
		}	//if(fifoCount==1024)
		else {
			BLog("GyroLoop(): fifoCount= ", fifoCount);
			if (fifoCount % packetSize != 0) {
				mpu.resetFIFO();
			}	//if(fifoCount%packetSize!=0)
			else {
				while (fifoCount >= packetSize) {
					mpu.getFIFOBytes(fifoBuffer, packetSize);
					fifoCount -= packetSize;
				}	//while
				//BLog("GyroLoop(): Call dmpGetQuaternion()");
				mpu.dmpGetQuaternion(&q, fifoBuffer);
				mpu.dmpGetGravity(&gravity, &q);
				mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

				BLog("GyroLoop(): Pitch= ", (ypr[1] * 180 / PI));
/*
				Serial.print("ypr\t");
				Serial.print(ypr[0] * 180 / PI);
				Serial.print("\t");
				Serial.print(ypr[1] * 180 / PI);
				Serial.print("\t");
				Serial.print(ypr[2] * 180 / PI);
				Serial.println();
*/
			}	//if(fifoCount%packetSize!=0)else
		}	//if(fifoCount==1024)else

	}	//if (milli() > ulNextGyroMilli)
	return;
}	//GyroLoop
//Last line.
