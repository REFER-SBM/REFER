
//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2025 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

#include <vector>
#include "DataOptHC.h"

CDataOptHC::CDataOptHC()
//Parameters to configure the optimization-based inflation by Hill Climbing
{
	iMaxIterations = 100;
	dThresholdRejectBestCost = 0;
	dThresholdRejectBestCostInLevel = 0;
	iStyleIncrement = 0;  // Parametric= 0, Prefixed= 1
	iIncrementPrefixed = 0;  // User= 0, Marill= 1, Fischler&Leclerc= 2
	dInitialStep = 0.025;
	dFinalStep = 0.0015;
	dStepReduction = 0.5;
	iPreConfigured = 2;   //Speed= 0, Accuracy= 1, Functional =2
	IncrementsOfVariables.clear(); // Automatically recalculated
};


CDataOptHC::~CDataOptHC(){
};
