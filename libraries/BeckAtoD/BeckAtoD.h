//BeckAtoD.h, Dec 17, 2017, Lenny-d
#ifndef BECKATOD_H
#define BECKATOD_H
#include <BeckMiniLib.h>
#include <BeckI2C.h>
//#include <BeckADS1115Defines.h>
#include <Adafruit_ADS1015.h>   //Just for the defines

#define INT8        int8_t
#define UINT8       uint8_t
#define INT16       int16_t
#define UINT16      uint16_t
#define INT32       int32_t
#define UINT32      uint32_t
#define INT64       int64_t
#define UINT64      uint64_t

//const double			adChanDividers[]			= {2.982, 3.003, 3.002, 2.0};	//Resistor divider ratio to A/D input
const double			adChanDividers[]			= {23.42, 3.003, 3.002, 2.0};	//Resistor divider ratio to A/D input, 45V chan 1
const adsGain_t		aeGainChans[]					= {GAIN_TWO, GAIN_TWO, GAIN_TWO, GAIN_TWO};	//Resistor divider ratio to A/D input
const INT16				sMotorVoltsChan		  	= 0;		//Motor volts ADS1115 ADC input channel
const INT16				sMotorAmpChan		  		= 1;		//Motor amps from ACS712 current sense A0 output volts
const INT16				sACS712VccChan		  	= 2;		//ACS712 current sense Vcc channel
const INT16				sDACVddChan		  			= 3;		//MCP4725 3.3V Vdd channel

const double			dVccNominal						= 5.0; 							//Vcc voltage to ACS712 current sense
const double			dZeroAmpVoltsNominal	= dVccNominal / 2; 	//Voltage at 0 amps from ACS712 current sense
const double			dAmpsPerVoltNominal		= 10.0; 						// Amps/volt from ACS712 current sense
const double			dMax12Bits						= 4096.0;						//Max for 12bits, 0xFFF

typedef enum {
  eADS1115= 1,
  eESP32_AtoD
} AtoD_t;


class BeckAtoD {
public:
						BeckAtoD        (BeckI2C* pBeckI2C, AtoD_t eType);		//Constructor
  double    dReadRawVolts   (INT16 sChan, adsGain_t eGain);
  //  double 		dReadRealVolts	(INT16 sChan, adsGain_t eGain, const double dDivider[]);
  //  double 		dReadAmps      	(INT16 sChan, adsGain_t eGain, const double dDivider[]);
	double 		dReadRealVolts	(INT16 sChan);
	double 		dReadAmps      	(INT16 sChan);
	void			WriteDACVolts		(double dVolts);
protected:
  AtoD_t        eType_;
  BeckI2C*      pBeckI2C_;
  const UINT8   ucADS1115_Address_  					= 0x48;
  const UINT8   ucMCP4725_Address_  					= 0x60;
  const UINT16  usDefaultSingleChanReadConfig_=
                          ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                          ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                          ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                          ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                          ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                          ADS1015_REG_CONFIG_MODE_SINGLE  | // Single-shot mode (default)
                          ADS1015_REG_CONFIG_OS_SINGLE;     // Single-shot start conversion
  double    dRead_ADS1115   (INT16 sChannel);
};  //BeckAtoD
#endif  //BECKATOD_H
