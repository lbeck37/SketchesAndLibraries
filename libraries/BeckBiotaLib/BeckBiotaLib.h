// BeckBiotaLib.h 12/28/19a
#pragma once

enum ProjectType{
  eNoProject  = 0,
  eThermoDev,
  eFireplace,
  eGarage,
  eHeater,
  eFrontLights,
  ePitchMeter,
  eLastProjectType
};

extern ProjectType   _eProjectType;

extern bool          _bSystemOk;

extern char          _acHostname   [50];
extern char          _acAlexaName  [50];
extern char          _acProjectType[50];
extern char          _acRouterName[];
extern char          _acRouterPW[];
extern char          _acAccessPointSSID[];
extern char          _acAccessPointPW[];

#include <BeckDisplayLib.h>
#include <BeckI2cLib.h>
#include <BeckLogLib.h>
#include <BeckMPU9150Lib.h>
#include <BeckThermoLib.h>

bool  SetupSystem           (ProjectType eProjectType);
void  SwitchProjectType     (ProjectType eProjectType);
//Last line.
