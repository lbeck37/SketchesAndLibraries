//BeckTanks.cpp
#include <BeckTanks.h>

BeckTanks::BeckTanks(BeckAtoD* pBeckAtoD, BeckDisplay* pBeckDisplay) {
  String szLogString="BeckTanks Constructor: Begin";
  LogToSerial(szLogString);
  pBeckDisplay_	= pBeckDisplay;
  pBeckAtoD_		= pBeckAtoD;
  return;
} //Constructor


void BeckTanks::Read(void) {
  double     adTankVoltReading[sNumTanks_];
  String     szLogString;

   if (millis() > ulNextTanksTime_) {
    ulNextTanksTime_= millis() + ulTanksReadTime_;
    String szLogString="BeckTanks::Read(): Read AtoD's";
    LogToSerial(szLogString);
    for (int sTank= eGrey1; sTank < eLastTank; sTank++) {
    	adVolts_[sTank]= pBeckAtoD_->dRead(sTank + 1, GAIN_ONE);
    }  //for
   }  //if (millis()>ulNextTanksTime)
   return;
} //Read


void BeckTanks::UpdateDisplay(void) {
  // szLogString="BeckTanks::UpdateDisplay(): Call pBeckDisplay_->UpdateTanks(adVolts_)";
  //LogToSerial(szLogString);
  pBeckDisplay_->UpdateTanks(adVolts_);
  return;
} //UpdateDisplay
//Last line.
