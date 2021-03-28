//BeckAtoD.cpp, Dec 18, 2017, Lenny-a
#include <BeckAtoD.h>

BeckAtoD::BeckAtoD(BeckI2C* pBeckI2C, AtoD_t eType){
  String szLogString="BeckAtoD Constructor: Begin";
  LogToSerial(szLogString);
  pBeckI2C_= pBeckI2C;
  eType_= eType;
  return;
} //Constructor


void BeckAtoD::WriteDACVolts(double dVolts) {
	double	dVddVolts;		//Vdd (3.3V) supply to MCP4725 DAC
	UINT16	usCode;

	dVddVolts= dReadRealVolts(sDACVddChan);
	usCode= (UINT16)((dVolts/dVddVolts) * dMax12Bits);
  Serial << "WriteDACVolts(): dVolts, usCode: " << dVolts << ", " << usCode << endl;

  Wire.beginTransmission(ucMCP4725_Address_);
  Wire.write(0x40);														// Writes data to the DAC
  Wire.write(usCode / 16);               			// Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
  Wire.write((usCode % 16) << 4);         		// Lower data bits          (D3.D2.D1.D0.x.x.x.x)
  Wire.endTransmission();

	return;
} //WriteDACVolts


double BeckAtoD::dReadRawVolts(INT16 sChan, adsGain_t eGain) {
  double	dRawVolts= dRead_ADS1115(sChan) / adChanDividers[sChan];	//dRead_ADS1115 returns corrected voltage
  return(dRawVolts);
} //dReadRawVolts


double BeckAtoD::dReadRealVolts(INT16 sChan) {
  double	dRealVolts= dRead_ADS1115(sChan);
  return(dRealVolts);
} //dReadRealVolts


double BeckAtoD::dReadAmps(INT16 sChan) {
	//Read current from ACS712 +/-20A full scale current sense
	//Vcc supply is nominal 5V but is read on channel 2 to use in current calculation
	//Zero amps is at 1/2 Vcc (2.5V nominal) and swings +/-2.0V (0.5 - 4.5)
  double	dVccVolts;		//Vcc volt supply to ACS712
	double	dVccCorrection;
	double	dZeroAmpVolts;
	double	dAmpsPerVolt;
	double	dACS712Volts;
	double  aAmpsReturn;

  dVccVolts= dReadRealVolts(sACS712VccChan);
  dVccCorrection= dVccVolts / dVccNominal;
  dZeroAmpVolts = dZeroAmpVoltsNominal * dVccCorrection;
  dAmpsPerVolt  = dAmpsPerVoltNominal  / dVccCorrection;
  dACS712Volts  = dReadRealVolts(sMotorAmpChan);

  aAmpsReturn= (dACS712Volts - dZeroAmpVolts) * dAmpsPerVolt;
  Serial << "dReadAmps(): sChan, dVccVolts, dACS712Volts, dVccCorrection, dZeroAmpVolts,  dAmpsPerVolt:"
  		<< sChan << ", " << dVccVolts << ", " << dACS712Volts << ", " << dVccCorrection << ", "
			<< dZeroAmpVolts << ", " << dAmpsPerVolt  << endl;
  return(aAmpsReturn);
} //dReadAmps


double BeckAtoD::dRead_ADS1115(INT16 sChan) {
	double dRealVoltsRead;
  double dVoltsFullScale= 2.048;
  double dCountFullScale= 32767.0;

  UINT16  usConfig= usDefaultSingleChanReadConfig_;
  //Serial << "dRead_ADS1115(): Begin " << endl;

  usConfig |= aeGainChans[sChan];
  switch (sChan) {
    case (0):
      usConfig |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
      break;
    case (1):
      usConfig |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
      break;
    case (2):
      usConfig |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
      break;
    case (3):
      usConfig |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
      break;
    default:
      Serial << "dRead_ADS1115(): Bad sChan switch= " << sChan << endl;
      break;
  } //switch

  pBeckI2C_->WriteI2cRegister(ucADS1115_Address_, ADS1015_REG_POINTER_CONFIG, usConfig);
  delay(50);      //Adafruit code only delays for 8

  INT16 sVoltCount= pBeckI2C_->ReadI2cRegister(ucADS1115_Address_, ADS1015_REG_POINTER_CONVERT);
  Serial << "dRead_ADS1115(): sChan, eGain, sVoltCount " << sChan << ", "
  		   << aeGainChans[sChan] << ", " << sVoltCount << endl;

  dRealVoltsRead= (((double)sVoltCount * dVoltsFullScale) / dCountFullScale) * adChanDividers[sChan];
  Serial << "dRead_ADS1115(): dRealVoltsRead= " << dRealVoltsRead << endl;
  return(dRealVoltsRead);
} //dRead_ADS1115
//Last line.
