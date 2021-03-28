//BeckTanks.h 2/26/17
#ifndef BECKTANKS_H
#define BECKTANKS_H
#include <BeckLib.h>
#include <BeckAtoD.h>
#include <BeckDisplay.h>

typedef enum {
  eGrey1= 0,
  eGrey2,
  eBlack,
  eLastTank
} Tank_t;

class BeckTanks {
protected:
  INT16           sNumTanks_          = eLastTank;
  double					adVolts_[eLastTank];
  const UINT32    ulTanksReadTime_    = 1000;   //Spacing between reading tanks

  uint32_t        ulNextTanksTime_    = 0;  //msec when the tanks will be read
  BeckAtoD*				pBeckAtoD_;
  BeckDisplay*		pBeckDisplay_;

public:
  double    adTankPercentFull_[eLastTank];

            BeckTanks     (BeckAtoD* pBeckAtoD, BeckDisplay* pBeckDisplay);
  void      Read          (void);
  void 			UpdateDisplay	(void);
};  //BeckTanks
#endif  //BECKTANKS_H
